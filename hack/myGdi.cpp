#include "stdafx.h"
#include "myGdi.h"
#include FT_GLYPH_H

unsigned char gdi_buffer[2048*2048*4];
RECT invalidateRects[1024];
int invalidateRectsCount = 0;

FT_Library ftlib;
FT_Face ftarial;

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
	FT_Set_Pixel_Sizes(ftarial, 0, 16);
	COLORREF c = GetTextColor(hdc);
	
	for (int i = 0; i < cchString; i++)
	{
		if (lpString[i] == ' ')
		{
			nXStart += 4;
			continue;
		}
		FT_Glyph t;
		int j, k;
		FT_Load_Glyph(ftarial, FT_Get_Char_Index(ftarial, lpString[i]), FT_LOAD_DEFAULT);
		FT_Get_Glyph(ftarial->glyph, &t);
		FT_Glyph_To_Bitmap(&t, ft_render_mode_normal, 0, 1);
		FT_Bitmap &bitmap = ((FT_BitmapGlyph)t)->bitmap;

		for (j = 0; j < bitmap.width; j++)
			for (k = 0; k < bitmap.rows; k++)
			{
				gdi_buffer[(tex_w * (k + nYStart) + (j + nXStart)) * 4] = bitmap.buffer[k * bitmap.pitch + j] * ((c & 0x000000FF) / 255.f);
				gdi_buffer[(tex_w * (k + nYStart) + (j + nXStart)) * 4 + 1] = bitmap.buffer[k * bitmap.pitch + j] * (((c & 0x0000FF00) >> 8) / 255.f);
				gdi_buffer[(tex_w * (k + nYStart) + (j + nXStart)) * 4 + 2] = bitmap.buffer[k * bitmap.pitch + j] * (((c & 0x00FF0000) >> 16) / 255.f);
				gdi_buffer[(tex_w * (k + nYStart) + (j + nXStart)) * 4 + 3] = bitmap.buffer[k * bitmap.pitch + j];
			}
		FT_Done_Glyph(t);
		nXStart += j;
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