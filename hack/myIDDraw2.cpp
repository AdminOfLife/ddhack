#include "StdAfx.h"
#include <varargs.h>


myIDDraw2::myIDDraw2()
{
	logf("myIDDraw2 Constructor");
}


myIDDraw2::~myIDDraw2(void)
{
	logf("myIDDraw2 Destructor");
}


HRESULT __stdcall myIDDraw2::QueryInterface (REFIID a, LPVOID FAR * b)
{	
	char *iid = "?";
	if (a == IID_IDirectDraw) iid = "IID_IDirectDraw";
	if (a == IID_IDirectDraw2) iid = "IID_IDirectDraw2";
//	if (a == IID_IDirectDraw4) iid = "IID_IDirectDraw4";
	if (a == IID_IDirectDraw7) iid = "IID_IDirectDraw7";

	logf("myIDDraw2::QueryInterface(%s,%08x)",iid,b);

	if (a == IID_IDirectDraw2)
	{
		*b = this;
		return NOERROR;
	}

	if (a == IID_IDirectDraw)
	{
		*b = new myIDDraw1();
		return NOERROR;
	}

	if (a == IID_IDirectDraw7)
	{
		*b = new myIDDraw7();
		return NOERROR;
	}
	return DDERR_UNSUPPORTED;
}


ULONG   __stdcall myIDDraw2::AddRef(void)
{
	logf("myIDDraw2::AddRef");
	return 1;
}


ULONG   __stdcall myIDDraw2::Release(void)
{
	logf("myIDDraw2::Release");
	delete this;
	
	return 0;
}


HRESULT  __stdcall myIDDraw2::Compact(void)
{
	logf("myIDDraw2::Compact");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR* c)
{
	logf("myIDDraw2::CreateClipper");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d)
{
	*c = new myIDDrawPalette(a, b);
	logf("myIDDraw2::CreatePalette(%d,%08x,%08x,%08x)",a,b,c,d);
	return NOERROR;
}


HRESULT  __stdcall myIDDraw2::CreateSurface(LPDDSURFACEDESC a, LPDIRECTDRAWSURFACE FAR* b, IUnknown FAR* c)
{
	logf("myIDDraw2::CreateSurface([%d,0x%x,%d,%d,%d,%d,%d], %08x, %08x)", a->dwSize, a->dwFlags, a->dwWidth, a->dwHeight, a->lPitch, a->dwBackBufferCount, a->ddsCaps, b, c); 

	*b = new myIDDrawSurface1(a);

	return NOERROR;
}


HRESULT  __stdcall myIDDraw2::DuplicateSurface(LPDIRECTDRAWSURFACE a, LPDIRECTDRAWSURFACE FAR* b)
{
	logf("myIDDraw2::DuplicateSurface");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::EnumDisplayModes(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMMODESCALLBACK d)
{
	logf("myIDDraw2::EnumDisplayModes");

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


HRESULT  __stdcall myIDDraw2::EnumSurfaces(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMSURFACESCALLBACK d)
{
	logf("myIDDraw2::EnumSurfaces");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::FlipToGDISurface(void)
{
	logf("myIDDraw2::FlipToGDISurface");
	return DD_OK;
}


HRESULT  __stdcall myIDDraw2::GetCaps(LPDDCAPS a, LPDDCAPS b)
{
	logf("myIDDraw2::GetCaps");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::GetDisplayMode(LPDDSURFACEDESC a)
{
	logf("myIDDraw2::GetDisplayMode");
	
	static int pixelformats[4][5] = {
		{ 8, DDPF_RGB | DDPF_PALETTEINDEXED8, 0x00000000, 0x00000000, 0x00000000 },
		{ 16, DDPF_RGB, 0x0000f800, 0x000007e0, 0x0000001f },
		{ 32, DDPF_RGB, 0x00ff0000, 0x0000ff00, 0x000000ff },
		{NULL, NULL, NULL, NULL, NULL }
	};

	int j = (gScreenBits == 8) ? 0 : (gScreenBits == 16) ? 1 : (gScreenBits == 32) ? 2 : 3;

	memset(a, 0, sizeof(DDSURFACEDESC));
	a->dwSize = sizeof(DDSURFACEDESC);
	a->dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_PITCH | DDSD_REFRESHRATE;
	a->dwWidth = gScreenWidth;
	a->dwHeight = gScreenHeight;
	a->lPitch = gScreenWidth * gScreenBits / 8;
	a->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	a->ddpfPixelFormat.dwFlags = pixelformats[j][1];
	a->ddpfPixelFormat.dwRGBBitCount = pixelformats[j][0];
	a->ddpfPixelFormat.dwRBitMask = pixelformats[j][2];
	a->ddpfPixelFormat.dwGBitMask = pixelformats[j][3];
	a->ddpfPixelFormat.dwBBitMask = pixelformats[j][4];

	return DD_OK;
}


HRESULT  __stdcall myIDDraw2::GetFourCCCodes(LPDWORD a, LPDWORD b)
{
	logf("myIDDraw2::GetFourCCCodes");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::GetGDISurface(LPDIRECTDRAWSURFACE FAR * a)
{
	logf("myIDDraw2::GetGDISurface");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::GetMonitorFrequency(LPDWORD a)
{
	logf("myIDDraw2::GetMonitorFrequency");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::GetScanLine(LPDWORD a)
{
	logf("myIDDraw2::GetScanLine");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::GetVerticalBlankStatus(LPBOOL a)
{
	logf("myIDDraw2::GetVerticalBlankStatus");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::Initialize(GUID FAR* a)
{
	logf("myIDDraw2::Initialize");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw2::RestoreDisplayMode(void)
{
	logf("myIDDraw2::RestoreDisplayMode");
	return NOERROR;
}


HRESULT  __stdcall myIDDraw2::SetCooperativeLevel(HWND a, DWORD b)
{
	logf("myIDDraw2::SetCooperativeLevel(%08x, %d)", a, b);

	gHwnd = a;

	return NOERROR;
}


HRESULT  __stdcall myIDDraw2::SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e)
{
	logf("myIDDraw2::SetDisplayMode(%d, %d, %d, %d, %d)",a,b,c,d,e);

	gScreenWidth = a;
	gScreenHeight = b;
	gScreenBits = c;
	gGDI = 0;
	
	return NOERROR; 
}


HRESULT  __stdcall myIDDraw2::WaitForVerticalBlank(DWORD a, HANDLE b)
{
	logf("myIDDraw2::WaitForVerticalBlank(%d,%d)",a,b);

	updatescreen();

	return NOERROR;
}


HRESULT  __stdcall myIDDraw2::GetAvailableVidMem(LPDDSCAPS a, LPDWORD b, LPDWORD c)
{
	logf("myIDDraw2::GetAvailableVidMem");
	return DDERR_UNSUPPORTED;
}



