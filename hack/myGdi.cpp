#include "stdafx.h"
#include "myGdi.h"
#include <string>
#include <map>
#include <cstring>
#include FT_GLYPH_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H

unsigned char gdi_buffer[2048*2048*4];
RECT invalidateRects[1024];
int invalidateRectsCount = 0;

FT_Library ftlib;
FT_Face ftarial;

std::map<std::string, std::string> fontlist;

void add_font_to_list(char *fontname, char *filename)
{
	fontlist.insert(std::pair<std::string, std::string>(std::string(fontname), std::string(filename)));
}

void populate_font_list()
{
	// 1. from the registry, EnumFontFamiliesEx doesn't give us a filename and freetype needs one, so we do it this way
	HKEY key;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &key) == ERROR_SUCCESS)
	{
		char fontname[256];
		DWORD fontname_size = 256;
		DWORD type;
		int i = 0;
		while (RegEnumValue(key, i++, fontname, &fontname_size, 0, &type, 0, 0) != ERROR_NO_MORE_ITEMS)
		{
			if (type != REG_SZ)
				continue;
			fontname_size = 256;
			char *offset = strstr(fontname, " (TrueType)");

			if (!offset)
				continue;

			char filename[256];
			DWORD filename_size = sizeof(filename);

			if (RegQueryValueEx(key, fontname, 0, 0, (LPBYTE) filename, &filename_size) == ERROR_SUCCESS)
			{
				char fullpath[1024];
				*offset = 0;
				_snprintf(fullpath, 1024, "C:/Windows/Fonts/%s", filename);
				add_font_to_list(fontname, fullpath);
			}
		}
	}
	// 2. fonts in the "fonts" folder of the active directory
	WIN32_FIND_DATA filedat;
	HANDLE file = FindFirstFile("fonts\\*.ttf", &filedat);

	do
	{
		char filename[256];
		_snprintf(filename, 256, "fonts/%s", filedat.cFileName);
		FT_Face f;
		int e = FT_New_Face(ftlib, filename, 0, &f);

		if (!e)
		{
			int total = FT_Get_Sfnt_Name_Count(f);
			char fontname[256];
			char familyname[64] = { 0 };
			char subname[64] = { 0 };

			for (int i = 0; i < total; i++)
			{
				FT_SfntName data;
				FT_Get_Sfnt_Name(f, i, &data);
				unsigned int j;

				if (data.platform_id != TT_PLATFORM_MICROSOFT || data.language_id != TT_MS_LANGID_ENGLISH_UNITED_STATES)
					continue;

				else if (data.name_id == TT_NAME_ID_FONT_FAMILY)
				{
					// utf-16 encoded, but the names are basically always ascii-only
					for (j = 0; j < data.string_len / 2 && j < 64; j++)
						familyname[j] = data.string[j*2+1];
					familyname[j+1] = 0;
					if (*subname != 0)
						goto combine;
				}
				else if (data.name_id == TT_NAME_ID_FONT_SUBFAMILY)
				{
					for (j = 0; j < data.string_len / 2 && j < 64; j++)
						subname[j] = data.string[j*2+1];
					subname[j+1] = 0;
					if (*familyname != 0)
						goto combine;
				}
				continue;
combine:
				if (strcmp(subname, "Regular") == 0)
					add_font_to_list(familyname, filename);
				else
				{
					_snprintf(fontname, 256, "%s %s", familyname, subname);
					add_font_to_list(fontname, filename);
				}
				break;
			}

			FT_Done_Face(f);
		}
	} while (FindNextFile(file, &filedat));
}

void gdi_setup()
{
	// TODO: generate a list of available fonts

	int e = FT_Init_FreeType(&ftlib);
	
	if (e)
	{
		logf("failed to initiate freetype");
		::ExitProcess(0);
	}

	// fallback font. if the system doesn't have Arial, then we have bigger problems
	e = FT_New_Face(ftlib, "C:/Windows/Fonts/arial.ttf", 0, &ftarial);

	if (e)
	{
		logf("failed to initiate Arial font");
		::ExitProcess(0);
	}

	FT_Select_Charmap(ftarial, FT_ENCODING_UNICODE);
	populate_font_list();
	gdi_clear_all();
}

void gdi_write_string(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cchString)
{
	LOGFONT lf;
	TEXTMETRIC tm;
	FT_Face f = 0;
	GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(lf), &lf);
	GetTextMetrics(hdc, &tm);
	std::string name(lf.lfFaceName);
	name += (lf.lfWeight > 400) ? " Bold" : "";
	name += lf.lfItalic ? " Italic" : "";
	if (fontlist.find(name) == fontlist.end() || FT_New_Face(ftlib, fontlist[name].c_str(), 0, &f))
		// try to fallback to regular font if bold/italic varieties can't be found
		if (fontlist.find(std::string(lf.lfFaceName)) == fontlist.end() || FT_New_Face(ftlib, fontlist[std::string(lf.lfFaceName)].c_str(), 0, &f))
			f = ftarial;
	int height = lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight;
	FT_Set_Pixel_Sizes(f, 0, height);
	COLORREF c = GetTextColor(hdc);

	FT_Bool hk = FT_HAS_KERNING(f);
	FT_UInt lc = 0;
	
	for (int i = 0; i < cchString; i++)
	{
		FT_Glyph t;
		FT_UInt cc = FT_Get_Char_Index(f, lpString[i]);
		FT_Load_Glyph(f, cc, FT_LOAD_DEFAULT);
		FT_Get_Glyph(f->glyph, &t);
		FT_Vector d;
		if (hk)
		{
			FT_Get_Kerning(f, lc, cc, FT_KERNING_DEFAULT, &d);
			nXStart -= d.x >> 16;
		}
		FT_Glyph_To_Bitmap(&t, FT_RENDER_MODE_MONO, 0, 1);
		FT_BitmapGlyph bg = (FT_BitmapGlyph) t;
		FT_Bitmap bitmap = bg->bitmap;
		for (int j = 0; j < bitmap.width; j++)
			for (int k = 0; k < bitmap.rows; k++)
			{
				unsigned char pixel = ((bitmap.buffer[(k * bitmap.pitch) + (j / 8)] >> (7 - j & 7)) & 1) * 0xFF;
				if (!pixel) continue;
				int gdi_offset = (tex_w * (k + nYStart - bg->top + height) + (j + nXStart)) * 4;
				gdi_buffer[gdi_offset] = (unsigned char) (pixel * ((c & 0x000000FF) / 255.f));
				gdi_buffer[gdi_offset + 1] = (unsigned char) (pixel * (((c & 0x0000FF00) >> 8) / 255.f));
				gdi_buffer[gdi_offset + 2] = (unsigned char) (pixel * (((c & 0x00FF0000) >> 16) / 255.f));
				gdi_buffer[gdi_offset + 3] = pixel;
			}

		FT_Done_Glyph(t);
		nXStart += t->advance.x >> 16;
		lc = c;
	}

	if (f != ftarial)
		FT_Done_Face(f);
}

void gdi_clear(const RECT *lpRect)
{
	if (tex_w == 0 || tex_h == 0)
		return;
	if (lpRect == 0)
		return;
	for (int i = lpRect->top; i <= lpRect->bottom; i++)
		memset(&gdi_buffer[(i * tex_w + lpRect->left) * 4], 0, (lpRect->right - lpRect->left) * 4);
}

void gdi_clear_all()
{
	invalidateRectsCount = 0;
	memset(gdi_buffer, 0, 2048*2048*4);
}

unsigned char *gdi_get_buffer()
{
	return gdi_buffer;
}

void gdi_invalidate(const RECT *lpRect)
{
	if (lpRect == 0)
		return;
	if (invalidateRectsCount >= 1024)
		return;
	memcpy(&invalidateRects[invalidateRectsCount++], lpRect, sizeof(RECT));
}

void gdi_run_invalidations()
{
	if (invalidateRectsCount)
		for (;invalidateRectsCount >= 0; --invalidateRectsCount)
			gdi_clear(&invalidateRects[invalidateRectsCount]);

	gdi_clear_invalidations();
}

void gdi_clear_invalidations()
{
	invalidateRectsCount = 0;
}