#include "StdAfx.h"
#include <varargs.h>

myIDDraw1::myIDDraw1()
{
	logf("myIDDraw1 Constructor");
}


myIDDraw1::~myIDDraw1(void)
{
	logf("myIDDraw1 Destructor");
}


HRESULT __stdcall myIDDraw1::QueryInterface (REFIID a, LPVOID FAR * b)
{
	char *iid = "?";
	if (a == IID_IDirectDraw) iid = "IID_IDirectDraw";
	if (a == IID_IDirectDraw2) iid = "IID_IDirectDraw2";
//	if (a == IID_IDirectDraw4) iid = "IID_IDirectDraw4";
	if (a == IID_IDirectDraw7) iid = "IID_IDirectDraw7";

	logf("myIDDraw1::QueryInterface(%s,%08x)",iid,b);

	if (a == IID_IDirectDraw)
	{
		*b = this;
		return NOERROR;
	}

	if (a == IID_IDirectDraw2)
	{
		*b = new myIDDraw2();
		return NOERROR;
	}

	if (a == IID_IDirectDraw7)
	{
		*b = new myIDDraw7();
		return NOERROR;
	}

	return DDERR_UNSUPPORTED;
}


ULONG   __stdcall myIDDraw1::AddRef(void)
{
	logf("myIDDraw1::AddRef");
	return 1;
}


ULONG   __stdcall myIDDraw1::Release(void)
{
	logf("myIDDraw1::Release");
	delete this;
	return (0);
}


HRESULT  __stdcall myIDDraw1::Compact(void)
{
	logf("myIDDraw1::Compact");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR* c)
{
	logf("myIDDraw1::CreateClipper");
	*b = new myIDDrawClipper();
	return DD_OK;
}


HRESULT  __stdcall myIDDraw1::CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d)
{
	*c = new myIDDrawPalette(a,b);
	logf("myIDDraw1::CreatePalette(%d,%08x,%08x,%08x)",a,b,c,d);
	return NOERROR;
}


HRESULT  __stdcall myIDDraw1::CreateSurface(LPDDSURFACEDESC a, LPDIRECTDRAWSURFACE FAR* b, IUnknown FAR* c)
{
	logf("myIDDraw1::CreateSurface([%d,0x%x,%d,%d,%d,%d,%d], %08x, %08x)", a->dwSize, a->dwFlags, a->dwWidth, a->dwHeight, a->lPitch, a->dwBackBufferCount, a->ddsCaps, b, c); 
	*b = new myIDDrawSurface1(a);
	
	return NOERROR;
}


HRESULT  __stdcall myIDDraw1::DuplicateSurface(LPDIRECTDRAWSURFACE a, LPDIRECTDRAWSURFACE FAR* b)
{
	logf("myIDDraw1::DuplicateSurface");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::EnumDisplayModes(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMMODESCALLBACK d)
{
	logf("myIDDraw1::EnumDisplayModes");
	
	// Send a bunch of modes, most modern systems should support all of these
	
	static int resolutions[22][2] = {
		{640, 480},
		{720, 480},
		{720, 576},
		{800, 600},
		{1024, 768},
		{1152, 864},
		{1176, 664},
		{1280, 720},
		{1280, 768},
		{1280, 800},
		{1280, 960},
		{1280, 1024},
		{1360, 768},
		{1366, 768},
		{1600, 900},
		{1600, 1024},
		{1600, 1200},
		{1600, 1050},
		{1768, 992},
		{1920, 1080},
		{1920, 1200},
		{NULL, NULL}
	};

	static int pixelformats[4][5] = {
		{ 8, DDPF_RGB | DDPF_PALETTEINDEXED8, 0x00000000, 0x00000000, 0x00000000 },
		{ 16, DDPF_RGB, 0x0000f800, 0x000007e0, 0x0000001f },
		{ 32, DDPF_RGB, 0x00ff0000, 0x0000ff00, 0x000000ff },
		{NULL, NULL, NULL, NULL, NULL }
	};

	for (int i = 0; ; i++)
	{
		if (resolutions[i][0] == NULL) break;

		for (int j = 0; ; j++)
		{
			if (pixelformats[j][0] == NULL) break;

			DDSURFACEDESC temp;
			memset(&temp, 0, sizeof(temp));
			temp.dwSize = sizeof(temp);
			temp.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_PITCH | DDSD_REFRESHRATE;
			temp.dwWidth = resolutions[i][0];
			temp.dwHeight = resolutions[i][1];
			temp.lPitch = temp.dwWidth * pixelformats[j][0] / 8;
			temp.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			temp.ddpfPixelFormat.dwFlags = pixelformats[j][1];
			temp.ddpfPixelFormat.dwRGBBitCount = pixelformats[j][0];
			temp.ddpfPixelFormat.dwRBitMask = pixelformats[j][2];
			temp.ddpfPixelFormat.dwGBitMask = pixelformats[j][3];
			temp.ddpfPixelFormat.dwBBitMask = pixelformats[j][4];
			(*d)(&temp, c);
		}
	}

	return DD_OK;
}


HRESULT  __stdcall myIDDraw1::EnumSurfaces(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMSURFACESCALLBACK d)
{
	logf("myIDDraw1::EnumSurfaces");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::FlipToGDISurface(void)
{
	logf("myIDDraw1::FlipToGDISurface");
	gGDI = 1;
	// WC4 *most of the time* pumps FlipToGDISurface when
	// the GDI surface is updated.. but not always.
	updatescreen();
	return DD_OK;
}


HRESULT  __stdcall myIDDraw1::GetCaps(LPDDCAPS a, LPDDCAPS b)
{
	logf("myIDDraw1::GetCaps(%08x, %08x)", a, b);

	LPDDCAPS p;

	if (a) 
	{
		p = a;
	}
	else
	{
		p = b;
	}

	int size = p->dwSize;
	memset(p, 0, size);
	p->dwSize = size;

// results from ddwrapper:
//[    +0ms] (006a8160) surface caps: 85d27fc1 a06ab270 210 3aad54e0 0 0 0 fe548000 fddf5000 0 0 0 0 0 304672fc 21fc8
//[   +16ms] (006a8160) HEL caps: f4c08241 1 200 3fce3 0 347 0 0 0 0 0 0 0 0 421350 21fc8

	if (a)
	{
		a->dwCaps = 0x85d27fc1;
		a->dwCaps2 = 0xa06ab270;
		a->dwCKeyCaps = 0x210;
		a->dwFXCaps = 0x3aad54e0;
		a->dwFXAlphaCaps = 0;
		a->dwPalCaps = 0;
		a->dwSVCaps = 0;
		a->dwVidMemTotal = 0xfe548000;
		a->dwVidMemFree = 0xfddf5000;
		a->dwAlignBoundarySrc = 0;
		a->dwAlignSizeSrc = 0;
		a->dwAlignBoundaryDest = 0;
		a->dwAlignSizeDest = 0;
		a->dwAlignStrideAlign = 0x304672fc;
		a->ddsCaps.dwCaps = 0x21fc8;
	}
	if (b)
	{
		b->dwCaps = 0xf4c08241;
		b->dwCaps2 = 1;
		b->dwCKeyCaps = 0x200;
		b->dwFXCaps = 0x3fce3;
		b->dwFXAlphaCaps = 0;
		b->dwPalCaps = 347;
		b->dwSVCaps = 0;
		b->dwVidMemTotal = 0;
		b->dwVidMemFree = 0;
		b->dwAlignBoundarySrc = 0;
		b->dwAlignSizeSrc = 0;
		b->dwAlignBoundaryDest = 0;
		b->dwAlignSizeDest = 0;
		b->dwAlignStrideAlign = 0x421350;
		b->ddsCaps.dwCaps = 0x21fc8;
	}

	return DD_OK;
}


HRESULT  __stdcall myIDDraw1::GetDisplayMode(LPDDSURFACEDESC a)
{
	logf("myIDDraw1::GetDisplayMode");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::GetFourCCCodes(LPDWORD a, LPDWORD b)
{
	logf("myIDDraw1::GetFourCCCodes");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::GetGDISurface(LPDIRECTDRAWSURFACE FAR * a)
{
	logf("myIDDraw1::GetGDISurface");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::GetMonitorFrequency(LPDWORD a)
{
	logf("myIDDraw1::GetMonitorFrequency");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::GetScanLine(LPDWORD a)
{
	logf("myIDDraw1::GetScanLine");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::GetVerticalBlankStatus(LPBOOL a)
{
	logf("myIDDraw1::GetVerticalBlankStatus");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::Initialize(GUID FAR* a)
{
	logf("myIDDraw1::Initialize");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw1::RestoreDisplayMode(void)
{
	logf("myIDDraw1::RestoreDisplayMode");
	return NOERROR;
}


HRESULT  __stdcall myIDDraw1::SetCooperativeLevel(HWND a, DWORD b)
{
	logf("myIDDraw1::SetCooperativeLevel(%08x, %d)", a, b);
	
	gHwnd = a;

	return NOERROR;
}


HRESULT  __stdcall myIDDraw1::SetDisplayMode(DWORD a, DWORD b, DWORD c)
{
	logf("myIDDraw1::SetDisplayMode(%d, %d, %d)",a,b,c);	

	gScreenWidth = a;
	gScreenHeight = b;
	gScreenBits = c;

	// reset to "non-gdi mode" by default
	gGDI = 0;

	return NOERROR;
}



HRESULT  __stdcall myIDDraw1::WaitForVerticalBlank(DWORD a, HANDLE b)
{
	logf("myIDDraw1::WaitForVerticalBlank");

	updatescreen();

	return NOERROR;
}


