#include "StdAfx.h"
#include <varargs.h>

myIDDraw7::myIDDraw7()
{
	logf("myIDDraw7 Constructor");
}


myIDDraw7::~myIDDraw7(void)
{
	logf("myIDDraw7 Destructor");
}


HRESULT __stdcall myIDDraw7::QueryInterface (REFIID a, LPVOID FAR * b)
{	
	char *iid = "?";
	if (a == IID_IDirectDraw) iid = "IID_IDirectDraw";
	if (a == IID_IDirectDraw2) iid = "IID_IDirectDraw2";
//	if (a == IID_IDirectDraw4) iid = "IID_IDirectDraw4";
	if (a == IID_IDirectDraw7) iid = "IID_IDirectDraw7";

	logf("myIDDraw7::QueryInterface(%s,%08x)",iid,b);

	if (a == IID_IDirectDraw7)
	{
		*b = this;
		return NOERROR;
	}

	if (a == IID_IDirectDraw2)
	{
		*b = new myIDDraw2();
		return NOERROR;
	}

	if (a == IID_IDirectDraw)
	{
		*b = new myIDDraw1();
		return NOERROR;
	}
	return DDERR_UNSUPPORTED;
}


ULONG   __stdcall myIDDraw7::AddRef(void)
{
	logf("myIDDraw7::AddRef");
	return 1;
}


ULONG   __stdcall myIDDraw7::Release(void)
{
	logf("myIDDraw7::Release");
	delete this;
	
	return 0;
}


HRESULT  __stdcall myIDDraw7::Compact(void)
{
	logf("myIDDraw7::Compact");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR* c)
{
	logf("myIDDraw7::CreateClipper");
	*b = new myIDDrawClipper();
	return DD_OK;
}


HRESULT  __stdcall myIDDraw7::CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d)
{
	*c = new myIDDrawPalette(a, b);
	logf("myIDDraw7::CreatePalette(%d,%08x,%08x,%08x)",a,b,c,d);
	return DD_OK;
}


HRESULT  __stdcall myIDDraw7::CreateSurface(LPDDSURFACEDESC2 a, LPDIRECTDRAWSURFACE7 FAR* b, IUnknown FAR* c)
{
	logf("myIDDraw7::CreateSurface([%d,0x%x,%d,%d,%d,%d,%d], %08x, %08x)", a->dwSize, a->dwFlags, a->dwWidth, a->dwHeight, a->lPitch, a->dwBackBufferCount, a->ddsCaps, b, c); 

	*b = new myIDDrawSurface7(a);

	return DD_OK;
}


HRESULT  __stdcall myIDDraw7::DuplicateSurface(LPDIRECTDRAWSURFACE7 a, LPDIRECTDRAWSURFACE7 FAR* b)
{
	logf("myIDDraw7::DuplicateSurface");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::EnumDisplayModes(DWORD a, LPDDSURFACEDESC2 b, LPVOID c, LPDDENUMMODESCALLBACK2 d)
{
	logf("myIDDraw7::EnumDisplayModes");

	// Send a bunch of modes, most modern systems should support all of these
	
	static int resolutions[21][2] = {
		{640, 480},
		{720, 480},
		{720, 576},
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

			DDSURFACEDESC2 temp;
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


HRESULT  __stdcall myIDDraw7::EnumSurfaces(DWORD a, LPDDSURFACEDESC2 b, LPVOID c, LPDDENUMSURFACESCALLBACK7 d)
{
	logf("myIDDraw7::EnumSurfaces");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::FlipToGDISurface(void)
{
	logf("myIDDraw7::FlipToGDISurface");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetCaps(LPDDCAPS a, LPDDCAPS b)
{
	logf("myIDDraw7::GetCaps");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetDisplayMode(LPDDSURFACEDESC2 a)
{
	logf("myIDDraw7::GetDisplayMode");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetFourCCCodes(LPDWORD a, LPDWORD b)
{
	logf("myIDDraw7::GetFourCCCodes");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetGDISurface(LPDIRECTDRAWSURFACE7 FAR * a)
{
	logf("myIDDraw7::GetGDISurface");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetMonitorFrequency(LPDWORD a)
{
	logf("myIDDraw7::GetMonitorFrequency");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetScanLine(LPDWORD a)
{
	logf("myIDDraw7::GetScanLine");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetVerticalBlankStatus(LPBOOL a)
{
	logf("myIDDraw7::GetVerticalBlankStatus");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::Initialize(GUID FAR* a)
{
	logf("myIDDraw7::Initialize");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::RestoreDisplayMode(void)
{
	logf("myIDDraw7::RestoreDisplayMode");
	return NOERROR;
}


HRESULT  __stdcall myIDDraw7::SetCooperativeLevel(HWND a, DWORD b)
{
	logf("myIDDraw7::SetCooperativeLevel(%08x, %d)", a, b);

	gHwnd = a;

	return NOERROR;
}


HRESULT  __stdcall myIDDraw7::SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e)
{
	logf("myIDDraw7::SetDisplayMode(%d, %d, %d, %d, %d)",a,b,c,d,e);

	gScreenWidth = a;
	gScreenHeight = b;
	gScreenBits = c;
	gGDI = 0;
	
	return NOERROR; 
}


HRESULT  __stdcall myIDDraw7::WaitForVerticalBlank(DWORD a, HANDLE b)
{
	logf("myIDDraw7::WaitForVerticalBlank(%d,%d)",a,b);

	updatescreen();

	return NOERROR;
}


HRESULT  __stdcall myIDDraw7::GetAvailableVidMem(LPDDSCAPS2 a, LPDWORD b, LPDWORD c)
{
	logf("myIDDraw7::GetAvailableVidMem");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetSurfaceFromDC(HDC a, LPDIRECTDRAWSURFACE7 *b)
{
	logf("myIDDraw7::GetSurfaceFromDC");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::RestoreAllSurfaces(void)
{
	logf("myIDDraw7::RestoreAllSurfaces");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::TestCooperativeLevel(void)
{
	logf("myIDDraw7::TestCooperativeLevel");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 a, DWORD b)
{
	logf("myIDDraw7::GetDeviceIdentifier");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::StartModeTest(LPSIZE a, DWORD b, DWORD c)
{
	logf("myIDDraw7::StartModeTest");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDraw7::EvaluateMode(DWORD a, DWORD *b)
{
	logf("myIDDraw7::EvaluateMode");
	return DDERR_UNSUPPORTED;
}
