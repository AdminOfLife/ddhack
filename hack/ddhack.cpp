/*  DirectDraw Hack
 *  version 1.0, August 6th, 2010
 *
 *  Copyright (C) 2010 Jari Komppa
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 *
 * Jari Komppa http://iki.fi/sol/
 *
 *************************************
 *
 * Based on a zlib/libpng licensed source code found on the net, 
 * http://www.mikoweb.eu/index.php?node=28
 * re-worked so much that there's fairly little left of the original.
 */
#include "stdafx.h"
#include <varargs.h>
#include <windowsx.h>
#include <detours/detours.h>
#include "logo.h"
#include "cursor.h"
#include "myGdi.h"

//#define LOG_DLL_ATTACH

// global variables
#pragma data_seg (".ddraw_shared")
HINSTANCE gHinst;
HWND gHwnd;        
myIDDrawSurface_Generic * gPrimarySurface;
myIDDrawSurface_Generic * gBackBuffer;
int gScreenWidth;
int gScreenHeight;
int gScreenBits;
int gRealScreenWidth;
int gRealScreenHeight;
unsigned int texdata[2048*2048];
WNDPROC origfunc = NULL;
HDC gWindowDC;
int gLastUpdate = -1;
int gAllowResize = 0;
int gSmooth = 0;
int gShowLogo = 0;
int gHalfAndHalf = 0;
int gOldLCD = 0;
int gScanDouble = 0;
int gAltWinPos = 0;
int gBlurWc3Video = 0;
int gWc3SmallVid = 0;
int gIgnoreAspect = 0;
int gGDI = 0;
int gVsync = 0;
int tex_w = 0;
int tex_h = 0;
int xPos = 0;
int yPos = 0;
int gSoftCursor = 0;
unsigned int temp[2048*2048];

#pragma data_seg ()

HWND (WINAPI *CreateWindowEx_fn)(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = CreateWindowEx;
BOOL (WINAPI *TextOutA_fn)(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cchString) = TextOutA;
BOOL (WINAPI *InvalidateRect_fn)(HWND hWnd, const RECT *lpRect, BOOL bErase) = InvalidateRect;
BOOL (WINAPI *ValidateRect_fn)(HWND hWnd, const RECT *lpRect) = ValidateRect;

void logf(char *msg, ...)
{
#if 0
	va_list argp;
	FILE * f = fopen("ddhack.log","a");
	static int t = -1;
	if (t == -1)
	t = GetTickCount();
	int tn = GetTickCount();

	fprintf(f,"[%+6dms] (%08x) ", tn-t, 0);
	t = tn;

	va_start(argp, msg);
	vfprintf(f, msg, argp);
	va_end(argp);

	fprintf(f,"\n");

	fclose(f);
#elif defined(_DEBUG)
	va_list argp;
	static int t = -1;
	char temp1[256];
	char temp2[256];
	if (t == -1)
		t = GetTickCount();
	int tn = GetTickCount();
		
	va_start(argp, msg);
	vsprintf(temp2, msg, argp);
	va_end(argp);

	sprintf(temp1,"[%08d %+6dms] %s\n", t, tn-t, temp2);
	t = tn;

	OutputDebugString(temp1);
#endif
}

HWND WINAPI myCreateWindowEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	logf("CreateWindowEx");
	logf(" nWidth: %d", nWidth);
	logf(" nHeight: %d", nHeight);
	logf(" dwExStyle: %08x", dwExStyle);
	logf(" dwStyle: %08x", dwStyle);
	dwExStyle &= ~(WS_EX_TOPMOST);

	if (dwStyle & WS_POPUP)
		dwStyle &= ~(WS_MAXIMIZE);

	HWND r = CreateWindowEx_fn(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	logf(" return: %08x", r);
	return r;
}

BOOL WINAPI myTextOutA(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cchString)
{
	char temp[1024];
	memcpy(temp, lpString, cchString);
	temp[cchString] = 0;
	logf("TextOutA_fn");
	logf(" hdc:       %08x", hdc);
	logf(" nXStart:   %d", nXStart);
	logf(" nYStart:   %d", nYStart);
	logf(" lpString:  \"%s\"", temp);
	logf(" cchString: %08x", cchString);

	
	gdi_run_invalidations();
	gdi_write_string(hdc, nXStart, nYStart, lpString, cchString);

	return 1;
}

BOOL WINAPI myInvalidateRect(HWND hWnd, const RECT *lpRect, BOOL bErase)
{
	logf("InvalidateRect");
	logf(" hWnd:   %08x", hWnd);
	if (lpRect != 0)
		logf(" lpRect: [%d,%d,%d,%d]", lpRect->top, lpRect->right, lpRect->bottom, lpRect->left);
	else
		logf(" lpRect: [null]");
	logf(" bErase: %d", bErase);

	if (!hWnd)
		{} // do nothing
	else if (bErase)
		gdi_clear(lpRect);
	else
		gdi_invalidate(lpRect);

	return InvalidateRect_fn(hWnd, lpRect, bErase);
}

BOOL WINAPI myValidateRect(HWND hWnd, const RECT *lpRect)
{
	logf("ValidateRect");
	logf(" hWnd:   %08x", hWnd);
	if (lpRect != 0)
		logf(" lpRect: [%d,%d,%d,%d]", lpRect->top, lpRect->right, lpRect->bottom, lpRect->left);
	else
		logf(" lpRect: [null]");

	gdi_clear_invalidations();

	return ValidateRect_fn(hWnd, lpRect);
}

void getgdibitmap()
{
#if 0
	HDC hDC = CreateCompatibleDC(gWindowDC);
	HBITMAP tempbitmap = CreateCompatibleBitmap(gWindowDC,gScreenWidth,gScreenHeight);
	memset(temp, 0xFF, sizeof(int)*2048*2048);
	SelectObject(hDC, tempbitmap);
	BitBlt(hDC,0,0,gScreenWidth,gScreenHeight,gWindowDC,0,0,SRCCOPY);

	// assumption: 32bpp desktop mode
	GetBitmapBits(tempbitmap,gScreenWidth*gScreenHeight*4,temp);	  

	/*for (i = 0; i < gScreenWidth; i++)
	{
		for (j = 0; j < gScreenHeight; j++)
		{
			if (temp[j*gScreenWidth+i] != 0)
				temp[j*gScreenWidth+i] |= 0xff000000;
			if (!i)
				temp[j] = 0x00ffffff;
		}
	}*/
	
	DeleteDC(hDC);
	DeleteObject(tempbitmap);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = gScreenWidth;
	r.bottom = gScreenHeight;
	
    glTexImage2D(GL_TEXTURE_2D,    // target
                 0,                // level
                 GL_RGBA,          // internalformat
                 gScreenWidth,     // width
                 gScreenHeight,    // height
                 0,                // border
                 GL_BGRA_EXT,      // format
                 GL_UNSIGNED_BYTE, // type
                 temp);         // texels
	glBlendFunc (GL_SRC_COLOR, GL_ONE);
	glEnable(GL_BLEND);

    float u = (float)gScreenWidth / (float)gScreenWidth;
    float v = (float)gScreenHeight / (float)gScreenHeight;
    
	// Next, we want to retain aspect ratio of 4/3, so we'll
	// end up with black bars on the sides or top and bottom
	// if the window size doesn't match.

    float w = 1, h = 1;
	float aspect = 4.0f / 3.0f;

    w = (gRealScreenHeight * aspect) / gRealScreenWidth;
    h = (gRealScreenWidth * (1 / aspect)) / gRealScreenHeight;

    if (w > h) w = 1; else h = 1;

	if (gIgnoreAspect)
	{
		w = (float)gScreenWidth / (float)gScreenWidth;
		h = (float)gScreenHeight / (float)gScreenHeight;
		// Do the actual rendering.
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0,0);              glVertex2f(-1,  1);
		glTexCoord2f(w,0);        glVertex2f( 1,  1);
		glTexCoord2f(w,h);  glVertex2f( 1, -1); 
		glTexCoord2f(0,h);        glVertex2f(-1, -1);
		glEnd();
	}
	else
	{
		// Do the actual rendering.
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0,0); glVertex2f( -w,  h);
		glTexCoord2f(u,0); glVertex2f(  w,  h);
		glTexCoord2f(u,v); glVertex2f(  w, -h); 
		glTexCoord2f(0,v); glVertex2f( -w, -h);
		glEnd();

	}
	::FillRect(gWindowDC, &r, (HBRUSH) GetStockObject(BLACK_BRUSH));
#endif
}


void updatescreen()
{
	int wc3video = 0;
	static int firsttick = -1;

	logf("updatescreen()");

	int tick = GetTickCount();
	if (firsttick == -1)
	{
		firsttick = tick;
		memset(texdata, 0, sizeof(int) * 2048 * 1024);
	}

	// If we're not set up yet, or it's been less or equal than 10ms since
	// the last update, skip the update.
	if (gPrimarySurface == NULL ||
		(gScreenBits == 8 && gPrimarySurface->getCurrentPalette() == NULL) ||
		(tick - gLastUpdate) <= 10)
	{
		if (gPrimarySurface == NULL)
			logf("primary surface is NULL");
		else
		if (gPrimarySurface->getCurrentPalette() == NULL)
			logf("primary palette is NULL");
		if ((tick - gLastUpdate) <= 10)
			logf("less than 10ms since last update");
		gLastUpdate = tick;
		return;
	}

// The dump/loadframe block can be used for image
// quality testing with fixed source frame.. I used
// it to play with video filtering.

//#define DUMPFRAME
//#define LOADFRAME

#ifdef DUMPFRAME
	static int frame = 0;
	frame++;
//	if (frame == 20)
	{
		FILE * f = fopen("frame100dump.dat","wb");
		fwrite(gPrimarySurface->getSurfaceData(),640*480,1,f);
		fwrite(gPrimarySurface->getCurrentPalette()->mPal,256*4,1,f);
		fclose(f);
	}
#elif defined(LOADFRAME)
		FILE * f = fopen("frame100dump.dat","rb");
		fread(gPrimarySurface->getSurfaceData(),640*480,1,f);
		fread(gPrimarySurface->getCurrentPalette()->mPal,256*4,1,f);
		fclose(f);

#endif

	gLastUpdate = tick;
	
	// texture size stuff probably should be global
	// and calculated once per mode init..
	tex_w = 512;
	tex_h = 256;	
	int i, j;

	if (gScreenWidth > 320 || gHalfAndHalf)
	{
		tex_w *= 2;
		tex_h *= 2;
	}

	if (tex_w < gScreenWidth || tex_h < gScreenHeight)
	{
		tex_w = (gScreenWidth + 3) & ~(3);
		tex_h = (gScreenHeight + 3) & ~(3);
	}

	if (gGDI)
	{
		// In GDI mode we'll skip most of the processing..
		//getgdibitmap();
	}
	else
	{
		wc3video = 1;

		for (i = 0; wc3video && i < gScreenWidth; i++)
		{
			// in wc3, only places where these two horizontal spans are
			// black is when we're viewing video.
			if (gPrimarySurface->getSurfaceData()[gPrimarySurface->getPitch() * 70 + i])
				wc3video = 0;
			if (gPrimarySurface->getSurfaceData()[gPrimarySurface->getPitch() * (gScreenHeight - 70) + i])
				wc3video = 0;
		}

		if (gScanDouble)
		{
			if (wc3video)
			{
				for (i = 71; i < gScreenHeight - 70; i+=2)
					memcpy(gPrimarySurface->getSurfaceData() + gPrimarySurface->getPitch() * (i-1),
					gPrimarySurface->getSurfaceData() + gPrimarySurface->getPitch() * i,
					gScreenWidth);			
			}
		}


		switch (gScreenBits)
		{
		case 8:		
			if (!gHalfAndHalf || gScreenWidth > 320)
			{
				for (i = 0; i < gScreenHeight; i++)
				{
					for (j = 0; j < gScreenWidth; j++)
					{
						int pix = gPrimarySurface->getSurfaceData()[gPrimarySurface->getPitch() * i + j];
						texdata[i*tex_w+j] = *(int*)&(gPrimarySurface->getCurrentPalette()->mPal[pix]);
					}
				}
			}
			else
			{
				// half'n'half mode - scale up 2x with software, let
				// hardware scale up to desktop resolution with bilinear
				for (i = 0; i < gScreenHeight * 2; i++)
				{
					for (j = 0; j < gScreenWidth * 2; j++)
					{
						int pix = gPrimarySurface->getSurfaceData()[gPrimarySurface->getPitch() * (i / 2) + (j / 2)];
						texdata[i*tex_w+j] = *(int*)&(gPrimarySurface->getCurrentPalette()->mPal[pix]);
					}
				}
			}
			break;
		case 16:
			{
				// wc4 16bit mode is actually 15bit - 1:5:5:5
				unsigned short * surf = (unsigned short *)gPrimarySurface->getSurfaceData();
				int pitch = gPrimarySurface->getPitch() / 2;
				for (i = 0; i < gScreenHeight; i++)
				{
					for (j = 0; j < gScreenWidth; j++)
					{
						int pix = surf[pitch * i + j];				
							
						int red   = (pix >> 10) & ((1 << 5) - 1);
						int green = (pix >>  5) & ((1 << 5) - 1);
						int blue  = (pix >>  0) & ((1 << 5) - 1);
							
						// fill bottom bits
						red = (red << 3) | (red >> 2);
						green = (green << 3) | (green >> 2);
						blue = (blue << 3) | (blue >> 2);
							

						texdata[i*tex_w+j] = (blue << 16) | (green << 8) | red;
					}
				}
			}
			break;
		case 24:
			{
				// the "24 bit" graphics mode in wc4 is actually 15 bits with
				// 9 bits of padding!
				char * surf = (char *)gPrimarySurface->getSurfaceData();
				int pitch = gPrimarySurface->getPitch() / 3;
				for (i = 0; i < gScreenHeight; i++)
				{
					for (j = 0; j < gScreenWidth; j++)
					{
						int pix = *(short*)(surf + (pitch * i + j) * 3);				
							
						int red   = (pix >> 10) & ((1 << 5) - 1);
						int green = (pix >>  5) & ((1 << 5) - 1);
						int blue  = (pix >>  0) & ((1 << 5) - 1);
							
						// fill bottom bits
						red = (red << 3) | (red >> 2);
						green = (green << 3) | (green >> 2);
						blue = (blue << 3) | (blue >> 2);

						texdata[i*tex_w+j] = (blue << 16) | (green << 8) | red;
					}
				}
			}
			break;
		}

		if (wc3video && gBlurWc3Video)
		{
			// The video blur is basically a 3x3 matrix
			// which emphasizes the center a lot.
			// Implemented in a lazy manner, directly
			// to the same buffer - yes, I know..
			// Doesn't matter much in practise.
			for (i = 70; i < gScreenHeight - 70; i++)
			{
				for (j = 1; j < gScreenWidth - 1; j++)
				{
					int pix =
						((texdata[i * tex_w + j         - 1] & 0xf8f8f8) >> 3) +
						((texdata[i * tex_w + j         + 1] & 0xf8f8f8) >> 3) +
						((texdata[i * tex_w + j - tex_w    ] & 0xf8f8f8) >> 3) +
						((texdata[i * tex_w + j + tex_w    ] & 0xf8f8f8) >> 3) +
						((texdata[i * tex_w + j - tex_w - 1] & 0xf8f8f8) >> 3) +
						((texdata[i * tex_w + j - tex_w + 1] & 0xf8f8f8) >> 3) +
						((texdata[i * tex_w + j + tex_w - 1] & 0xf8f8f8) >> 3) +
						((texdata[i * tex_w + j + tex_w + 1] & 0xf8f8f8) >> 3);
					texdata[i * tex_w + j] = 
						((texdata[i * tex_w + j] & 0xfefefe) >> 1) +
											((pix & 0xfefefe) >> 1);
				}
			}
		}
	}

	// Logo is "watermarked" over the framebuffer.. so its size
	// depends on the resolution.
	if (tick - firsttick < 2000 || gShowLogo)
	{
		for (i = 1; i < 63; i++)
		{
			for (j = 1; j < 63; j++)
			{
				if (logo[(63-i)*64+j])
				{
					texdata[i*tex_w+j] = (texdata[i*tex_w+j] & 0xffffff) |
										(((texdata[i*tex_w+j] & 0xff) + 0xff) >> 2);
				}
				else
				if (logo[(63-i+1)*64+j-1])
				{
					texdata[i*tex_w+j] = (texdata[i*tex_w+j] & 0xfcfcfcfc) >> 1;									
				}
			}
		}
	}

	//getgdibitmap();

	if (gSoftCursor && xPos >= 0 && yPos >= 0)
	{
		for (i = 0; i < 12 && xPos + i < tex_w; i++)
		{
			for (j = 0; j < 19 && yPos + j < tex_h; j++)
			{
				unsigned int pixel = cursor[j*12+i];

				if (pixel != 0xFF000000)
				{
					texdata[(yPos+j)*tex_w+xPos+i] = pixel;
				}
			}
		}
	}
	
    // upload texture
    glTexImage2D(GL_TEXTURE_2D,    // target
                 0,                // level
                 GL_RGB,           // internalformat
                 tex_w,            // width
                 tex_h,            // height
                 0,                // border
                 GL_RGBA,          // format
                 GL_UNSIGNED_BYTE, // type
                 texdata);         // texels
    // render

    if (gSmooth)
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }

    glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0);
	glViewport(0,0,gRealScreenWidth,gRealScreenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// In case we're in half'n'half mode, make sure the calculations go right..
	if (gHalfAndHalf && gScreenWidth <= 320)
	{
		tex_w /= 2;
		tex_h /= 2;
	}

	// Handle the fact that while our texture is a power of two,
	// the area we're using isn't.

    float u = (float)gScreenWidth / (float)tex_w;
    float v = (float)gScreenHeight / (float)tex_h;
    
	// Next, we want to retain aspect ratio of 4/3, so we'll
	// end up with black bars on the sides or top and bottom
	// if the window size doesn't match.

    float w = 1, h = 1;
	float aspect = 4.0f / 3.0f;

    w = (gRealScreenHeight * aspect) / gRealScreenWidth;
    h = (gRealScreenWidth * (1 / aspect)) / gRealScreenHeight;

    if (w > h) w = 1; else h = 1;
    
	// The "old LCD" effect is created by rendering 
	// with alpha blending

	if (gOldLCD)
	{
		glEnable(GL_BLEND);		
		glColor4f(1.0f,1.0f,1.0f,1.5f / (gOldLCD + 1.0f)); 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_BLEND);
        glColor3f(1.0f,1.0f,1.0f); 
	}

	// Do the actual rendering.
	if (gIgnoreAspect)
	{
		w = (float)gScreenWidth / (float)tex_w;
		h = (float)gScreenHeight / (float)tex_h;
		// Do the actual rendering.
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0,0);              glVertex2f(-1,  1);
		glTexCoord2f(w,0);        glVertex2f( 1,  1);
		glTexCoord2f(w,h);  glVertex2f( 1, -1); 
		glTexCoord2f(0,h);        glVertex2f(-1, -1);
		glEnd();
	}
	else
	{
		// Do the actual rendering.
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0,0); glVertex2f( -w,  h);
		glTexCoord2f(u,0); glVertex2f(  w,  h);
		glTexCoord2f(u,v); glVertex2f(  w, -h); 
		glTexCoord2f(0,v); glVertex2f( -w, -h);
		glEnd();

	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, gdi_get_buffer());
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor3f(1.0f,1.0f,1.0f); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (gIgnoreAspect)
	{
		w = (float)gScreenWidth / (float)tex_w;
		h = (float)gScreenHeight / (float)tex_h;
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0,0);              glVertex2f(-1,  1);
		glTexCoord2f(w,0);        glVertex2f( 1,  1);
		glTexCoord2f(w,h);  glVertex2f( 1, -1); 
		glTexCoord2f(0,h);        glVertex2f(-1, -1);
		glEnd();
	}
	else
	{
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0,0); glVertex2f( -w,  h);
		glTexCoord2f(u,0); glVertex2f(  w,  h);
		glTexCoord2f(u,v); glVertex2f(  w, -h); 
		glTexCoord2f(0,v); glVertex2f( -w, -h);
		glEnd();
	}

	getgdibitmap();

	SwapBuffers(gWindowDC);
}


LRESULT CALLBACK newwinproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	static int focus = 1;
	int tick = GetTickCount();
	if (gLastUpdate == -1)
	{
		gLastUpdate = tick;
	}

	// Force updates at at least 40Hz so that some bits which do not
	// update the screen explicitly show up, including the GDI mode..
	if ((tick - gLastUpdate) > (1000 / 40))
	{
		logf("winproc: too long since last update");
		updatescreen();
	}

	//SetCapture(hWnd); // causes horrible input lag, so let's not use it
	switch (uMsg)
	{
	case WM_KILLFOCUS:
		{
			focus = 0;
			ShowCursor(1);
			ClipCursor(NULL);
		}
		break;
	case WM_SETFOCUS: 
		{
			focus = 1;
			RECT r;
			r.top = 0;
			r.left = 0;
			r.bottom = gScreenHeight;
			r.right = gScreenWidth;
			ClipCursor(&r);
			SetCursorPos(gScreenWidth / 2, gScreenHeight / 2);
			ShowCursor(0); // clipcursor doesn't work with cursor disabled. Yay.

			break;
		}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		{
			if (!focus)
				return 0;
			// fix the mouse cursor position..
			if (!gAltWinPos)
				lParam -= 480 << 16;
			xPos = GET_X_LPARAM(lParam);
			yPos = GET_Y_LPARAM(lParam);
			break;
		}
	case WM_WINDOWPOSCHANGING:
		if (!gAllowResize)
		{
			// WC4 tries to resize and -position the window.
			// Disallow this. The gAllowResize global is so that
			// we can resize ourselves.
			WINDOWPOS * p = (WINDOWPOS *)lParam;
			p->flags |= SWP_NOSIZE | SWP_NOMOVE;
			return 0;		
		}
		break;
	}

	// Pass control to the application..
	return origfunc(hWnd, uMsg, wParam, lParam);
}

void init_gl()
{	
	RECT r;
	// Only init once..
	if (!origfunc)
	{
		// Get the application's window procedure..
		origfunc = (WNDPROC)GetWindowLong(gHwnd, GWL_WNDPROC);
		// ..and replace it with our own.
		SetWindowLong(gHwnd, GWL_WNDPROC, (long)newwinproc);
	
		r.top = 0;
		r.left = 0;
		// lowest fallback: 640x480 window
		r.bottom = 480;
		r.right = 640;

		// next up: work area for primary display
		SystemParametersInfo(SPI_GETWORKAREA,0,&r,0);

		// best: full screen
		MONITORINFO mon;
		mon.cbSize = sizeof(mon);

		if (GetMonitorInfo(MonitorFromWindow(gHwnd,MONITOR_DEFAULTTOPRIMARY),&mon))
		{
			r = mon.rcMonitor;
		}

		gRealScreenWidth = r.right;
		gRealScreenHeight = r.bottom;

		gAllowResize = 1;
		// Go full screen..
		MoveWindow(gHwnd, 0, -480 * (1 - gAltWinPos), gRealScreenWidth, 
				   gRealScreenHeight + 480 * (1 - gAltWinPos), 0);
		// Set position just in case..
		SetWindowPos(gHwnd, NULL, 0, -480 * (1 - gAltWinPos), 0, 0, SWP_NOSIZE);
		gAllowResize = 0;

		PIXELFORMATDESCRIPTOR pfd;
		pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);                             // Size 
		pfd.nVersion=1;                                                      // Version
		pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;  // Selected flags
		pfd.iPixelType=PFD_TYPE_RGBA;                                        // Pixelformat
		pfd.cColorBits=16;                                                   // Pixel depth
		pfd.cDepthBits=16;                                                   // Zbuffer depth
		pfd.iLayerType=PFD_MAIN_PLANE;                                       // Place the pixelformat on the main plane
	
		HGLRC gOpenGLRC = NULL;
		// this is a bit heavy-handed, but the delay dll loading
		// does seem to require a bit of work on win7..
		// (and delay-loading the opengl dll is required to work in xp)
		// Oddly enough, doing LoadLibrary on opengl didn't work in xp.
		do {
			Sleep(50);
			gWindowDC=GetDC(gHwnd);
			int pf=ChoosePixelFormat(gWindowDC, &pfd);
			SetPixelFormat(gWindowDC, pf, &pfd);
			gOpenGLRC = wglCreateContext(gWindowDC);
		} while (!gOpenGLRC);

		wglMakeCurrent(gWindowDC, gOpenGLRC);
		char *glext = (char *)glGetString(GL_EXTENSIONS);
		if(glext && strstr(glext, "WGL_EXT_swap_control"))
		{
			BOOL (APIENTRY *wglSwapIntervalEXT)(int) = (BOOL (APIENTRY *)(int))wglGetProcAddress("wglSwapIntervalEXT");
			if(wglSwapIntervalEXT)
			{
				if(gVsync)
				{
					wglSwapIntervalEXT(1);
				}
				else
				{
					wglSwapIntervalEXT(0);
				}
			}
		}


		ShowWindow(gHwnd, SW_SHOW);
		SetForegroundWindow(gHwnd);

		// Create a timer so we'll get some events all the time
		SetTimer(gHwnd, 1, 10, NULL);
	}

	r.top = 0;
	r.left = 0;
	r.bottom = gScreenHeight;
	r.right = gScreenWidth;
	ClipCursor(&r);
	
	gdi_clear_all();
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    LPVOID lpDummy = lpReserved;
    lpDummy = NULL;
    
    switch (ul_reason_for_call)
	{
	    case DLL_PROCESS_ATTACH: 
#ifdef LOG_DLL_ATTACH
			logf("DllMain(%08X,%08X,%08X) DLL_PROCESS_ATTACH",hModule, ul_reason_for_call, lpReserved); 
#endif
			InitInstance(hModule); break;
	    case DLL_PROCESS_DETACH: 
#ifdef LOG_DLL_ATTACH
			logf("DllMain(%08X,%08X,%08X) DLL_PROCESS_DETACH",hModule, ul_reason_for_call, lpReserved);
#endif
			ExitInstance(); break;
        
        case DLL_THREAD_ATTACH:  
#ifdef LOG_DLL_ATTACH
			logf("DllMain(%08X,%08X,%08X) DLL_THREAD_ATTACH",hModule, ul_reason_for_call, lpReserved); 
#endif
			break;
	    case DLL_THREAD_DETACH:  
#ifdef LOG_DLL_ATTACH
			logf("DllMain(%08X,%08X,%08X) DLL_THREAD_DETACH",hModule, ul_reason_for_call, lpReserved); 
#endif
			break;
		default:
			logf("DllMain(%08X,%08X,%08X) unknown reason",hModule, ul_reason_for_call, lpReserved);
	}
    return(true);
}

#define INI_FILE "./ddhack.ini"
#define INI_READ_STRING(section,key,default,buf,size) GetPrivateProfileStringA(section,key,default,buf,size,INI_FILE)
#define INI_READ_INT(section,key,default) GetPrivateProfileIntA(section,key,default,INI_FILE)
void InitInstance(HANDLE hModule) 
{
	logf("InitInstance.");
	// Our extremely simple config file handling..
	gSmooth=INI_READ_INT("Rendering","bilinear_filter",0);
	gSmooth=gHalfAndHalf=INI_READ_INT("Rendering","halfnhalf",0);
	gShowLogo=INI_READ_INT("Rendering","show_logo",0);
	gOldLCD=INI_READ_INT("Rendering","old_lcd_level",0);
	gScanDouble=INI_READ_INT("Rendering","wc3scandouble",0);
	gBlurWc3Video=INI_READ_INT("Rendering","wc3blurvideo",0);
	gWc3SmallVid=INI_READ_INT("Rendering","wc3smallvid",0);
	gAltWinPos=INI_READ_INT("Rendering","altwinpos",0);
	gIgnoreAspect=INI_READ_INT("Rendering","ignore_aspect_ratio",0);
	gVsync=INI_READ_INT("Rendering","vsync",0);
	gSoftCursor=INI_READ_INT("Rendering","softcursor",0);


	// Init some defaults..
	gHinst = NULL;
	gHwnd = NULL;
	gPrimarySurface = NULL;
	gScreenWidth = 640;
	gScreenHeight = 480;
	gScreenBits = 8;
	
	// Store Instance handle into global var
	gHinst = (HINSTANCE)  hModule;

	DisableThreadLibraryCalls((HMODULE) hModule);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)TextOutA_fn, myTextOutA);
	if(DetourTransactionCommit() != NO_ERROR)
	{
		logf("Could not hook TextOutA");
		::ExitProcess(0);
	}
	DetourTransactionBegin();
	DetourAttach(&(PVOID&)CreateWindowEx_fn, myCreateWindowEx);
	if(DetourTransactionCommit() != NO_ERROR)
	{
		logf("Could not hook CreateWindowEx");
		::ExitProcess(0);
	}
	DetourTransactionBegin();
	DetourAttach(&(PVOID&)InvalidateRect_fn, myInvalidateRect);
	if(DetourTransactionCommit() != NO_ERROR)
	{
		logf("Could not hook InvalidateRect");
		::ExitProcess(0);
	}
	DetourTransactionBegin();
	DetourAttach(&(PVOID&)ValidateRect_fn, myValidateRect);
	if(DetourTransactionCommit() != NO_ERROR)
	{
		logf("Could not hook ValidateRect");
		::ExitProcess(0);
	}
	// We'll get the hWnd from setcooperativemode later.
}

void ExitInstance() 
{
	logf("ExitInstance.");
}
