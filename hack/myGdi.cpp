#include "stdafx.h"
#include "myGdi.h"
#include "fixed.h"

#include <string>
#include <hash_map>

void gdi_write_string(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cchString, LPRECT lprc, UINT dwDTFormat)
{
	if (open_dcs.find(hdc) == open_dcs.end())
		return;
	std::hash_map<short, int> kernings;
	char lc = 0;
	MAT2 m2;
	m2.eM11.fract = 0;
	m2.eM11.value = 1;
	m2.eM12.fract = 0;
	m2.eM12.value = 0;
	m2.eM21.fract = 0;
	m2.eM21.value = 0;
	m2.eM22.fract = 0;
	m2.eM22.value = 1;
	GLYPHMETRICS t;
	LOGFONT lf;
	GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(lf), &lf);
	COLORREF c = GetTextColor(hdc);
	if (GetGlyphOutline(hdc, 'A', GGO_BITMAP, &t, 0, 0, &m2) == GDI_ERROR)
		goto fallback;
	// hack: draw child window text to parent, calculate offset here
	HWND hWnd = WindowFromDC(hdc);
	logf("gdi_write_string");
	logf("hWdc: %08x", hWnd);
	if (hWnd != NULL)
	{
		POINT p1 = {0}, p2 = {0};
		GetViewportOrgEx(hdc, &p1);
		GetViewportOrgEx(GetDC(gHwnd), &p2);
		nXStart += p1.x - p2.x;
		nYStart += p1.y - p2.y;
	}
	logf("newX: %3d, newY: %3d", nXStart, nYStart);

	int height = lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight;
	unsigned char **buffers = new unsigned char*[cchString];
	GLYPHMETRICS *gms = new GLYPHMETRICS[cchString];
	int *kerns = new int[cchString];
	int i;
	logf(" facename: %s", lf.lfFaceName);

	// get kerning pairs
	int numpairs = GetKerningPairs(hdc, 0, 0);

	if (numpairs)
	{
		KERNINGPAIR *pairs = new KERNINGPAIR[numpairs];
		GetKerningPairs(hdc, numpairs, pairs);

		for (int i = 0; i < numpairs; i++)
		{
			if (pairs[i].wFirst > 255 || pairs[i].wSecond > 255)
				continue;
			kernings[pairs[i].wFirst | (pairs[i].wSecond << 8)] = pairs[i].iKernAmount;
		}

		delete [] pairs;
	}
	logf("1");
	int totalwidth = 0, totalheight = 0;

	for (i = 0; i < cchString; i++)
	{
		int size = GetGlyphOutline(hdc, lpString[i], GGO_BITMAP, &gms[i], 0, 0, &m2);
		buffers[i] = new unsigned char[size];
		GetGlyphOutline(hdc, lpString[i], GGO_BITMAP, &gms[i], size, buffers[i], &m2);
		kerns[i] = kernings[lc | (lpString[i] << 8)];
		lc = lpString[i];
	}
	
	logf("2");
	SIZE s;
	GetTextExtentPoint32(hdc, lpString, cchString, &s);
	totalwidth = s.cx;
	totalheight = s.cy;
	UINT align = GetTextAlign(hdc);

	if (lprc != NULL)
	{
		nXStart += lprc->left;
		if (dwDTFormat & DT_CENTER)
			nXStart += (lprc->right - lprc->left - totalwidth) / 2;
		nYStart += lprc->top;
		if (dwDTFormat & DT_BOTTOM)
			nYStart += (lprc->bottom - lprc->top - totalheight);
	}
	else if (align & TA_RIGHT)
		nXStart -= totalwidth;
	else if (align & TA_CENTER)
		nXStart -= totalwidth / 2;

	for (i = 0; i < cchString; i++)
	{
		nXStart += kerns[i];
		DWORD pitch = ((gms[i].gmBlackBoxX / 8) + 3) & ~3;
		if (!pitch) pitch = 4;
		for (DWORD j = 0; j < gms[i].gmBlackBoxX; j++)
		{
			if ((int) j + nXStart < 0)
				continue;
			if ((int) j + nXStart > open_dcs[hdc]->getWidth())
				break;
			for (DWORD k = 0; k < gms[i].gmBlackBoxY; k++)
			{
				if ((int) k + nYStart < 0)
					continue;
				if ((int) k + nYStart > open_dcs[hdc]->getHeight())
					break;
				unsigned char pixel= ((buffers[i][(k * pitch) + (j / 8)] >> ((7 - (j + 8)) & 7)) & 1) * 0xFF;
				if (!pixel) continue;
				int bpp = open_dcs[hdc]->getPitch() / open_dcs[hdc]->getWidth();
				int gdi_offset = (open_dcs[hdc]->getWidth() * (k + nYStart - gms[i].gmptGlyphOrigin.y + height) + (j + nXStart)) * bpp;
				unsigned char *dest = open_dcs[hdc]->getSurfaceData();
				switch (bpp)
				{
				case 4:
					dest[gdi_offset] = (unsigned char) (pixel * ((c & 0x000000FF) / 255.f));
					dest[gdi_offset + 1] = (unsigned char) (pixel * (((c & 0x0000FF00) >> 8) / 255.f));
					dest[gdi_offset + 2] = (unsigned char) (pixel * (((c & 0x00FF0000) >> 16) / 255.f));
					dest[gdi_offset + 3] = pixel;
					break;
				case 1:
					dest[gdi_offset] = color2palette(c);
					break;
				}
			}
		}

		delete [] buffers[i];
		nXStart += gms[i].gmCellIncX;
	}
	delete [] buffers;
	delete [] gms;
	delete [] kerns;
	logf("done");
	return;

	// we can't use GetGlyphOutline for bitmap fonts, so we have this hackish workaround. :(
fallback:
	
	if (lprc != NULL)
	{
		nXStart += lprc->left;
		nYStart += lprc->top;
	}

	for (int i = 0; i < cchString; i++)
	{
		char ch = lpString[i];
		nXStart += fixed_kernings[lc | (ch << 8)];
		ch -= 0x20;

		for (unsigned int j = 0; j < fixed_width[ch]; j++)
		{
			if ((int) j + nXStart < 0)
				continue;
			if ((int) j + nXStart > open_dcs[hdc]->getWidth())
				break;
			for (unsigned int k = 0; k < fixed_height[ch]; k++)
			{
				if ((int) k + nYStart < 0)
					continue;
				if ((int) k + nYStart > open_dcs[hdc]->getHeight())
					break;
				unsigned int pixel= fixed_data[ch][k * fixed_width[ch] + j] * c;
				if (!pixel) continue;
				int bpp = open_dcs[hdc]->getPitch() / open_dcs[hdc]->getWidth();
				int gdi_offset = (open_dcs[hdc]->getWidth() * (k + nYStart) + (j + nXStart)) * bpp;
				unsigned char *dest = open_dcs[hdc]->getSurfaceData();
				switch (bpp)
				{
				case 4:
					*(unsigned int *)(dest[gdi_offset]) = pixel;
					break;
				case 1:
					dest[gdi_offset] = color2palette(c);
					break;
				}
			}
		}
		nXStart += fixed_aw[ch];
	}
}

/*void gdi_clear(const RECT *lpRect)
{
	if (tex_w == 0 || tex_h == 0)
		return;
	if (lpRect == NULL)
	{
		for(std::unordered_set<myIDDrawSurface_Generic *>::iterator it = full_surfaces.begin(); it != full_surfaces.end(); it++)
			memset((*it)->getGdiBuffer(), 0, tex_w * tex_h * 4);
		return;
	}
	for(std::unordered_set<myIDDrawSurface_Generic *>::iterator it = full_surfaces.begin(); it != full_surfaces.end(); it++)
		for (int i = lpRect->top; i <= lpRect->bottom; i++)
			memset(&(*it)->getGdiBuffer()[(i * tex_w + lpRect->left) * 4], 0, (lpRect->right - lpRect->left) * 4);
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
}*/
