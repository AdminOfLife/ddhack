#include "stdafx.h"
#include "myGdi.h"
#include FT_GLYPH_H

unsigned char gdi_buffer[2048*2048*4];
RECT invalidateRects[1024];
int invalidateRectsCount = 0;

FT_Library ftlib;
FT_Face ftarial;

/*struct {
	FT_Face face;
	char name[32];
} fontlist[1024];*/


void gdi_setup()
{
	// TODO: generate a list of available fonts

	int e = FT_Init_FreeType(&ftlib);
	
	if (e)
	{
		logf("failed to initiate freetype");
		::ExitProcess(0);
	}

	e = FT_New_Face(ftlib, "C:/Windows/Fonts/ARIAL.TTF", 0, &ftarial);

	if (e)
	{
		logf("failed to initiate Arial font");
		::ExitProcess(0);
	}

	FT_Select_Charmap(ftarial, FT_ENCODING_UNICODE);

	gdi_clear_all();
}

void gdi_write_string(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cchString)
{
	LOGFONT lf;
	GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(lf), &lf);
	int height = lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight * 64 / 72;
	FT_Set_Pixel_Sizes(ftarial, 0, height);
	COLORREF c = GetTextColor(hdc);

	FT_Bool hk = FT_HAS_KERNING(ftarial);
	FT_UInt lc = 0;
	
	for (int i = 0; i < cchString; i++)
	{
		FT_Glyph t;
		FT_UInt cc = FT_Get_Char_Index(ftarial, lpString[i]);
		FT_Load_Glyph(ftarial, cc, FT_LOAD_DEFAULT);
		FT_Get_Glyph(ftarial->glyph, &t);
		FT_Vector d;
		if (hk)
		{
			FT_Get_Kerning(ftarial, lc, cc, FT_KERNING_DEFAULT, &d);
			nXStart += d.x >> 16;
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