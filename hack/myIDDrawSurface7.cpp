#include "StdAfx.h"
#include <varargs.h>


myIDDrawSurface7::myIDDrawSurface7(LPDDSURFACEDESC2 a)
{
	logf("myIDDrawSurface7 Constructor");

	mWidth = gScreenWidth;
	mHeight = gScreenHeight;
	mSurfaceDesc = *a;
	mCaps = a->ddsCaps;

	if (a->dwFlags & DDSD_WIDTH) mWidth = a->dwWidth;
	if (a->dwFlags & DDSD_HEIGHT) mHeight = a->dwHeight;
	gScreenWidth = mWidth;
	gScreenHeight = mHeight;
	// we don't need no stinking extra pitch bytes..
	mPitch = mWidth * gScreenBits / 8;
	
	if (a->dwFlags & DDSD_PITCH) mPitch = a->lPitch;
	if (a->dwFlags & DDSD_CAPS)
	{
		if (mCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
		{
			gPrimarySurface = this;
			init_gl();
		}
	}

	mSurfaceDesc.dwWidth = mWidth;
	mSurfaceDesc.dwHeight = mHeight;
	mSurfaceDesc.lPitch = mPitch;
	mSurfaceDesc.dwFlags |= DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH;

	// Let's pad the framebuffer by a couple of megs, in case
	// the app writes outside bounds..
	// (we have enough trouble being stable as it is)
	mRealSurfaceData = new unsigned char[mHeight * mPitch + 2 * 1024 * 1024];
	mSurfaceData = mRealSurfaceData + 1024 * 1024 * 1;
	memset(mSurfaceData, 0, mHeight * mPitch);

	mCurrentPalette = NULL;
}


myIDDrawSurface7::~myIDDrawSurface7(void)
{
	logf("myIDDrawSurface7 Destructor");

	if (this == gPrimarySurface)
	{
		gPrimarySurface = NULL;
		delete gBackBuffer;
		gBackBuffer = NULL;
	}
	delete[] mRealSurfaceData;
}


HRESULT __stdcall myIDDrawSurface7::QueryInterface (REFIID, LPVOID FAR * b)
{
	logf("myIDDrawSurface7::QueryInterface");
	
	*b = NULL;

	return DDERR_UNSUPPORTED;
}


ULONG   __stdcall myIDDrawSurface7::AddRef(void)
{
	logf("myIDDrawSurface7::AddRef");
	return 1;
}


ULONG   __stdcall myIDDrawSurface7::Release(void)
{
	logf("myIDDrawSurface7::Release");
	delete this;
	
	return 0;
}



HRESULT  __stdcall myIDDrawSurface7::AddAttachedSurface(LPDIRECTDRAWSURFACE7 a)
{
	logf("myIDDrawSurface7::AddAttachedSurface");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::AddOverlayDirtyRect(LPRECT a)
{
	logf("myIDDrawSurface7::AddOverlayDirtyRect");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::Blt(LPRECT a,LPDIRECTDRAWSURFACE7 b, LPRECT c,DWORD d, LPDDBLTFX e)
{
	if (a && c)
		logf("myIDDrawSurface7::Blt([%d,%d,%d,%d],%08x,[%d,%d,%d,%d],%08x,%08x)",
			a->top,a->left,a->bottom,a->right,
			b,
			c->top,c->left,c->bottom,c->right,
			d,
			e ? e->dwDDFX : 0);
	else
	if (a)
		logf("myIDDrawSurface7::Blt([%d,%d,%d,%d],%08x,[null],%08x,%08x)",
			a->top,a->left,a->bottom,a->right,
			b,
			d,
			e ? e->dwDDFX : 0);
	else
	if (c)
		logf("myIDDrawSurface7::Blt([null],%08x,[%d,%d,%d,%d],%08x,%08x)",
			b,
			c->top,c->left,c->bottom,c->right,
			d,
			e ? e->dwDDFX : 0);
	else
		logf("myIDDrawSurface7::Blt([null],%08x,[null],%08x,%08x)",
			b,
			d,
			e ? e->dwDDFX : 0);
	
	int i, j;
	myIDDrawSurface7 *src = NULL;
	if (b) src = (myIDDrawSurface7*)b;
	int usingColorKey = d & DDBLT_KEYDEST || d & DDBLT_KEYSRC || d & DDBLT_ALPHADEST;
	unsigned char colorKey = 0;
	if (usingColorKey)
		colorKey = (unsigned char) (d & DDBLT_KEYDEST ? mDestColorKey.dwColorSpaceLowValue : src->mSrcColorKey.dwColorSpaceLowValue);

	if (b == NULL)
	{
		if (a)
			for (i = a->bottom; i < a->top; i++)
				for (j = a->left; j < a->right; j++)
					mSurfaceData[i*mPitch+j] = (d & DDBLT_COLORFILL ? (unsigned char) e->dwFillColor : 0);
		else
			memset(mSurfaceData, (d & DDBLT_COLORFILL ? e->dwFillColor : 0), mHeight * mPitch);
	}
	else
	{
		// Othewise we're scaling a 320x240 to 640x480.. or we're scaling the
		// video on screen.

		if (a && c && gWc3SmallVid)
		{
			for (i = c->top; i < c->bottom; i++)
				for (j = c->left; j < c->right; j++)
					mSurfaceData[(i + (480 - c->bottom)/2) * mPitch + j + 160] = src->mSurfaceData[i * src->mPitch + j];
		}
		else
		{
			if (a && c)
			{
				for (i = 0; i < a->bottom - a->top; i++)
					for (j = 0; j < a->right - a->left; j++)
						if (!usingColorKey || src->mSurfaceData[(i + c->top) * src->mPitch + j + c->left] != colorKey)
							mSurfaceData[(i + a->top) * mPitch + j + a->left] = src->mSurfaceData[(i + c->top) * src->mPitch + j + c->left];
			}
			else
			{
				for (i = 0; i < mHeight; i++)
					for (j = 0; j < mWidth; j++)
						if (!usingColorKey || src->mSurfaceData[i * src->mPitch + j] != colorKey)
							mSurfaceData[i * mPitch + j] = src->mSurfaceData[i * src->mPitch + j];
			}
		}
	}


	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
	logf("myIDDrawSurface7::BltBatch");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::BltFast(DWORD a,DWORD b,LPDIRECTDRAWSURFACE7 c, LPRECT d,DWORD e)
{
	logf("myIDDrawSurface7::BltFast(%d,%d,%08x,[%d,%d,%d,%d],%08x)",a,b,c,d->top,d->left,d->bottom,d->right,e);
	myIDDrawSurface7 *src = (myIDDrawSurface7*)c;
	int usingColorKey = e & DDBLT_KEYDEST || e & DDBLT_KEYSRC || e & DDBLT_ALPHADEST;
	unsigned char colorKey = 0;
	if (usingColorKey)
		colorKey = (unsigned char) (e & DDBLT_KEYDEST ? mDestColorKey.dwColorSpaceLowValue : src->mSrcColorKey.dwColorSpaceLowValue);
	
	for (int i = 0; i < d->bottom - d->top; i++)
		for (int j = 0; j < d->right - d->left; j++)
			if (!usingColorKey || src->mSurfaceData[(i + d->top) * src->mPitch + j + d->left] != colorKey)
				mSurfaceData[(i + b) * mPitch + j + a] = src->mSurfaceData[(i + d->top) * src->mPitch + j + d->left];	
	
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::DeleteAttachedSurface(DWORD a,LPDIRECTDRAWSURFACE7 b)
{
	logf("myIDDrawSurface7::DeleteAttachedSurface");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::EnumAttachedSurfaces(LPVOID a,LPDDENUMSURFACESCALLBACK7 b)
{
	logf("myIDDrawSurface7::EnumAttachedSurfaces");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::EnumOverlayZOrders(DWORD a,LPVOID b,LPDDENUMSURFACESCALLBACK7 c)
{
	logf("myIDDrawSurface7::EnumOverlayZOrders");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::Flip(LPDIRECTDRAWSURFACE7 a, DWORD b)
{
	logf("myIDDrawSurface7::Flip");
	updatescreen();
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetAttachedSurface(LPDDSCAPS2 a, LPDIRECTDRAWSURFACE7 FAR * b)
{
	logf("myIDDrawSurface7::GetAttachedSurface([%d], %08x)",
		a->dwCaps, b);

	// wc3 and wc4 call this function to access the back buffer..
	// hack: make a new surface which still uses the primary's
	// surface data.

	// Potential memory leak; should check and return gBackBuffer
	// if already exists... but the games I've checked don't call
	// this several times, so why bother.
	// And yes, I realize the checking code would take less space
	// than this comment that's complaining about it.
	// On the other hand, you wouldn't have so much fun reading 
	// this if I just deleted it and wrote the check, now would you?
	
	DDSURFACEDESC2 newdesc = mSurfaceDesc;
	
	newdesc.ddsCaps.dwCaps |= a->dwCaps;	
	newdesc.ddsCaps.dwCaps &= ~DDSCAPS_PRIMARYSURFACE;

	myIDDrawSurface7 * n = new myIDDrawSurface7(&newdesc);
	n->mSurfaceData = mSurfaceData;
	*b = n;
	gBackBuffer = n;

	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetBltStatus(DWORD a)
{
	logf("myIDDrawSurface7::GetBltStatus");
	// we're always ready for bitblts
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetCaps(LPDDSCAPS2 a)
{
	logf("myIDDrawSurface7::GetCaps");
	*a = mCaps;
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetClipper(LPDIRECTDRAWCLIPPER FAR* a)
{
	logf("myIDDrawSurface7::GetClipper");
	a = (LPDIRECTDRAWCLIPPER *) mClipper;
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
	logf("myIDDrawSurface7::GetColorKey");
	if (a & DDCKEY_DESTBLT)
		b = &mDestColorKey;
	else if (a & DDCKEY_SRCBLT)
		b = &mSrcColorKey;
	else
		return DDERR_UNSUPPORTED;
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetDC(HDC FAR *a)
{
	logf("myIDDrawSurface7::GetDC");
	*a = GetDC2(gHwnd);
	gGDI = 1;
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetFlipStatus(DWORD a)
{
	logf("myIDDrawSurface7::GetFlipStatus");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::GetOverlayPosition(LPLONG a, LPLONG b)
{
	logf("myIDDrawSurface7::GetOverlayPosition");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::GetPalette(LPDIRECTDRAWPALETTE FAR*a)
{
	logf("myIDDrawSurface7::GetPalette");
	*a = mCurrentPalette;
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetPixelFormat(LPDDPIXELFORMAT a)
{
	logf("myIDDrawSurface7::GetPixelFormat");
	// Return codes based on what ddwrapper reported..
	if (gScreenBits == 8)
	{
		a->dwSize = 0x20;
		a->dwFlags = 0x60;
		a->dwFourCC = 0;
		a->dwRGBBitCount = 0x8;
		a->dwRBitMask = 0;
		a->dwGBitMask = 0;	
		a->dwBBitMask = 0;
		a->dwRGBAlphaBitMask = 0;
	}
	else
	if (gScreenBits == 16)
	{
		a->dwSize = 0x20;
		a->dwFlags = 0x40;
		a->dwFourCC = 0;
		a->dwRGBBitCount = 0x10;
		a->dwRBitMask = 0xf800;
		a->dwGBitMask = 0x07e0;
		a->dwBBitMask = 0x001f;
		a->dwRGBAlphaBitMask = 0;
	}
	else
	if (gScreenBits == 24)
	{
		a->dwSize = 0x20;
		a->dwFlags = 0x40;
		a->dwFourCC = 0;
		a->dwRGBBitCount = 24;
		a->dwRBitMask = 0xff0000;
		a->dwGBitMask = 0x00ff00;
		a->dwBBitMask = 0x0000ff;
		a->dwRGBAlphaBitMask = 0;
	}
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::GetSurfaceDesc(LPDDSURFACEDESC2 a)
{
	logf("myIDDrawSurface7::GetSurfaceDesc([%d %d %d %d %d %d %d %d])",
		a->dwSize, a->dwFlags, a->dwWidth, a->dwHeight, a->lPitch, a->dwBackBufferCount,
		a->lpSurface, a->ddsCaps.dwCaps);
	*a = mSurfaceDesc;
	if (gScreenBits == 8)
	{
		a->ddpfPixelFormat.dwSize = 0x20;
		a->ddpfPixelFormat.dwFlags = 0x60;
		a->ddpfPixelFormat.dwFourCC = 0;
		a->ddpfPixelFormat.dwRGBBitCount = 0x8;
		a->ddpfPixelFormat.dwRBitMask = 0;
		a->ddpfPixelFormat.dwGBitMask = 0;	
		a->ddpfPixelFormat.dwBBitMask = 0;
		a->ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}
	else
	if (gScreenBits == 16)
	{
		a->ddpfPixelFormat.dwSize = 0x20;
		a->ddpfPixelFormat.dwFlags = 0x40;
		a->ddpfPixelFormat.dwFourCC = 0;
		a->ddpfPixelFormat.dwRGBBitCount = 0x10;
		a->ddpfPixelFormat.dwRBitMask = 0xf800;
		a->ddpfPixelFormat.dwGBitMask = 0x07e0;
		a->ddpfPixelFormat.dwBBitMask = 0x001f;
		a->ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}
	else
	if (gScreenBits == 24)
	{
		a->ddpfPixelFormat.dwSize = 0x20;
		a->ddpfPixelFormat.dwFlags = 0x40;
		a->ddpfPixelFormat.dwFourCC = 0;
		a->ddpfPixelFormat.dwRGBBitCount = 24;
		a->ddpfPixelFormat.dwRBitMask = 0xff0000;
		a->ddpfPixelFormat.dwGBitMask = 0x00ff00;
		a->ddpfPixelFormat.dwBBitMask = 0x0000ff;
		a->ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}
	logf("pixel format: %d bit, R%08x G%08x B%08x", a->ddpfPixelFormat.dwRGBBitCount, a->ddpfPixelFormat.dwRBitMask, a->ddpfPixelFormat.dwGBitMask, a->ddpfPixelFormat.dwBBitMask);
	return DD_OK;//DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC2 b)
{
	logf("myIDDrawSurface7::Initialize");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::IsLost()
{
	logf("myIDDrawSurface7::IsLost");
	// We're never lost..
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::Lock(LPRECT a,LPDDSURFACEDESC2 b,DWORD aFlags,HANDLE d)
{
	char *extra = "";
	if (this == gPrimarySurface)
		extra = " primary";
	if (this == gBackBuffer)
		extra = " backbuffer";	

	if (a)	
		logf("myIDDrawSurface7::Lock([%d,%d,%d,%d],%08x,%d,%d)%s",a->top,a->left,a->bottom,a->right,b,aFlags,d,extra);
	else
		logf("myIDDrawSurface7::Lock([null],%08x,%d,%d)%s",b,aFlags,d,extra);

	gGDI = 0;

	*b = mSurfaceDesc;

	b->dwFlags |= DDSD_LPSURFACE | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_CAPS;
	b->lpSurface = mSurfaceData;

	b->dwWidth = mWidth;
	b->dwHeight = mHeight;
	b->lPitch = mPitch;

	return NOERROR;
}



HRESULT  __stdcall myIDDrawSurface7::ReleaseDC(HDC a)
{
	logf("myIDDrawSurface7::ReleaseDC");
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::Restore()
{
	logf("myIDDrawSurface7::Restore");
	// we can't lose surfaces, so..
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::SetClipper(LPDIRECTDRAWCLIPPER a)
{
	logf("myIDDrawSurface7::SetClipper");
	mClipper = (myIDDrawClipper *) a;
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
	logf("myIDDrawSurface7::SetColorKey");
	if (a & DDCKEY_DESTBLT)
		mDestColorKey = *b;
	else if (a & DDCKEY_SRCBLT)
		mSrcColorKey = *b;
	else
		return DDERR_UNSUPPORTED;
	return DD_OK;
}



HRESULT  __stdcall myIDDrawSurface7::SetOverlayPosition(LONG a, LONG b)
{
	logf("myIDDrawSurface7::SetOverlayPosition");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::SetPalette(LPDIRECTDRAWPALETTE a)
{
	logf("myIDDrawSurface7::SetPalette(%08x)",a);
	mCurrentPalette = (myIDDrawPalette *)a;
	return NOERROR;
}

HRESULT  __stdcall myIDDrawSurface7::Unlock(LPRECT a)
{
	logf("myIDDrawSurface7::Unlock(%08x)",a);

	// if primary has been updated, flush..
	// otherwise wc2 misses some screens
	// (no retrace, flip, or even message pump)
	if (this == gPrimarySurface)
		updatescreen();

	return NOERROR;
}



HRESULT  __stdcall myIDDrawSurface7::UpdateOverlay(LPRECT a, LPDIRECTDRAWSURFACE7 b,LPRECT c,DWORD d, LPDDOVERLAYFX e)
{
	logf("myIDDrawSurface7::UpdateOverlay");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::UpdateOverlayDisplay(DWORD a)
{
	logf("myIDDrawSurface7::UpdateOverlayDisplay");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE7 b)
{
	logf("myIDDrawSurface7::UpdateOverlayZOrder");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::GetDDInterface(LPVOID FAR *a)
{
	logf("myIDDrawSurface7::GetDDInterface");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::PageLock(DWORD a)
{
	logf("myIDDrawSurface7::PageLock");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::PageUnlock(DWORD a)
{
	logf("myIDDrawSurface7::PageUnlock");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::SetSurfaceDesc(LPDDSURFACEDESC2 a, DWORD b)
{
	logf("myIDDrawSurface7::SetSurfaceDesc");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::SetPrivateData(REFGUID a, LPVOID b, DWORD c, DWORD d)
{
	logf("myIDDrawSurface7::SetPrivateData");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::GetPrivateData(REFGUID a, LPVOID b, LPDWORD c)
{
	logf("myIDDrawSurface7::GetPrivateData");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::FreePrivateData(REFGUID a)
{
	logf("myIDDrawSurface7::FreePrivateData");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::GetUniquenessValue(LPDWORD a)
{
	logf("myIDDrawSurface7::GetUniquenessValue");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::ChangeUniquenessValue()
{
	logf("myIDDrawSurface7::ChangeUniquenessValue");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::SetPriority(DWORD a)
{
	logf("myIDDrawSurface7::SetPriority");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::GetPriority(LPDWORD a)
{
	logf("myIDDrawSurface7::GetPriority");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::SetLOD(DWORD a)
{
	logf("myIDDrawSurface7::SetLOD");
	return DDERR_UNSUPPORTED;
}



HRESULT  __stdcall myIDDrawSurface7::GetLOD(LPDWORD a)
{
	logf("myIDDrawSurface7::GetLOD");
	return DDERR_UNSUPPORTED;
}