#pragma once

inline HDC GetDC2(HWND hWnd)
{
	return GetDC(hWnd);
}

class myIDDrawPalette;

class myIDDraw1 : public IDirectDraw
{
public:
    myIDDraw1();
    virtual ~myIDDraw1(void);

	// The original DDraw function definitions BEGIN
    HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);
    
    HRESULT  __stdcall Compact(void);
    HRESULT  __stdcall CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR* c);
	HRESULT  __stdcall CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d);
	HRESULT  __stdcall CreateSurface(LPDDSURFACEDESC a, LPDIRECTDRAWSURFACE FAR* b, IUnknown FAR* c);
	HRESULT  __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE a, LPDIRECTDRAWSURFACE FAR* b);
	HRESULT  __stdcall EnumDisplayModes(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMMODESCALLBACK d);
	HRESULT  __stdcall EnumSurfaces(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMSURFACESCALLBACK d);
	HRESULT  __stdcall FlipToGDISurface(void);
	HRESULT  __stdcall GetCaps(LPDDCAPS a, LPDDCAPS b);
    HRESULT  __stdcall GetDisplayMode(LPDDSURFACEDESC a);
    HRESULT  __stdcall GetFourCCCodes(LPDWORD a, LPDWORD b);
    HRESULT  __stdcall GetGDISurface(LPDIRECTDRAWSURFACE FAR * a);
    HRESULT  __stdcall GetMonitorFrequency(LPDWORD a);
    HRESULT  __stdcall GetScanLine(LPDWORD a);
    HRESULT  __stdcall GetVerticalBlankStatus(LPBOOL a);
    HRESULT  __stdcall Initialize(GUID FAR* a);
    HRESULT  __stdcall RestoreDisplayMode(void);
	HRESULT  __stdcall SetCooperativeLevel(HWND a, DWORD b);
	HRESULT  __stdcall SetDisplayMode(DWORD a, DWORD b, DWORD c);
	HRESULT  __stdcall WaitForVerticalBlank(DWORD a, HANDLE b);

};


class myIDDraw2 : public IDirectDraw2
{
public:
    myIDDraw2();
    virtual ~myIDDraw2(void);

	// The original DDraw function definitions BEGIN
    HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);
    
    HRESULT  __stdcall Compact(void);
    HRESULT  __stdcall CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR* c);
	HRESULT  __stdcall CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d);
	HRESULT  __stdcall CreateSurface(LPDDSURFACEDESC a, LPDIRECTDRAWSURFACE FAR* b, IUnknown FAR* c);
	HRESULT  __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE a, LPDIRECTDRAWSURFACE FAR* b);
	HRESULT  __stdcall EnumDisplayModes(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMMODESCALLBACK d);
	HRESULT  __stdcall EnumSurfaces(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMSURFACESCALLBACK d);
	HRESULT  __stdcall FlipToGDISurface(void);
	HRESULT  __stdcall GetCaps(LPDDCAPS a, LPDDCAPS b);
    HRESULT  __stdcall GetDisplayMode(LPDDSURFACEDESC a);
    HRESULT  __stdcall GetFourCCCodes(LPDWORD a, LPDWORD b);
    HRESULT  __stdcall GetGDISurface(LPDIRECTDRAWSURFACE FAR * a);
    HRESULT  __stdcall GetMonitorFrequency(LPDWORD a);
    HRESULT  __stdcall GetScanLine(LPDWORD a);
    HRESULT  __stdcall GetVerticalBlankStatus(LPBOOL a);
    HRESULT  __stdcall Initialize(GUID FAR* a);
    HRESULT  __stdcall RestoreDisplayMode(void);
	HRESULT  __stdcall SetCooperativeLevel(HWND a, DWORD b);
	HRESULT  __stdcall SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e);
	HRESULT  __stdcall WaitForVerticalBlank(DWORD a, HANDLE b);

	/*** Added in the v2 interface ***/
    HRESULT  __stdcall GetAvailableVidMem(LPDDSCAPS a, LPDWORD b, LPDWORD c);

};

class myIDDraw7 : public IDirectDraw7
{
public:
    myIDDraw7();
    virtual ~myIDDraw7(void);

	// The original DDraw function definitions BEGIN
    HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);
    
    HRESULT  __stdcall Compact(void);
    HRESULT  __stdcall CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR* c);
	HRESULT  __stdcall CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d);
	HRESULT  __stdcall CreateSurface(LPDDSURFACEDESC2 a, LPDIRECTDRAWSURFACE7 FAR* b, IUnknown FAR* c);
	HRESULT  __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE7 a, LPDIRECTDRAWSURFACE7 FAR* b);
	HRESULT  __stdcall EnumDisplayModes(DWORD a, LPDDSURFACEDESC2 b, LPVOID c, LPDDENUMMODESCALLBACK2 d);
	HRESULT  __stdcall EnumSurfaces(DWORD a, LPDDSURFACEDESC2 b, LPVOID c, LPDDENUMSURFACESCALLBACK7 d);
	HRESULT  __stdcall FlipToGDISurface(void);
	HRESULT  __stdcall GetCaps(LPDDCAPS a, LPDDCAPS b);
    HRESULT  __stdcall GetDisplayMode(LPDDSURFACEDESC2 a);
    HRESULT  __stdcall GetFourCCCodes(LPDWORD a, LPDWORD b);
    HRESULT  __stdcall GetGDISurface(LPDIRECTDRAWSURFACE7 FAR * a);
    HRESULT  __stdcall GetMonitorFrequency(LPDWORD a);
    HRESULT  __stdcall GetScanLine(LPDWORD a);
    HRESULT  __stdcall GetVerticalBlankStatus(LPBOOL a);
    HRESULT  __stdcall Initialize(GUID FAR* a);
    HRESULT  __stdcall RestoreDisplayMode(void);
	HRESULT  __stdcall SetCooperativeLevel(HWND a, DWORD b);
	HRESULT  __stdcall SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e);
	HRESULT  __stdcall WaitForVerticalBlank(DWORD a, HANDLE b);

	/*** Added in the v2 interface ***/
    HRESULT  __stdcall GetAvailableVidMem(LPDDSCAPS2 a, LPDWORD b, LPDWORD c);

	/*** Added in the V4 Interface ***/
    HRESULT  __stdcall GetSurfaceFromDC(HDC a, LPDIRECTDRAWSURFACE7 *b);
    HRESULT  __stdcall RestoreAllSurfaces(void);
    HRESULT  __stdcall TestCooperativeLevel(void);
    HRESULT  __stdcall GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 a, DWORD b);
    HRESULT  __stdcall StartModeTest(LPSIZE a, DWORD b, DWORD c);
    HRESULT  __stdcall EvaluateMode(DWORD a, DWORD *b);
};

class myIDDrawSurface_Generic
{
	public:
	virtual ~myIDDrawSurface_Generic() {};
	virtual myIDDrawPalette *getCurrentPalette() const = 0;
	virtual unsigned char * getSurfaceData() const = 0;
	virtual int getPitch() const = 0;
};

class myIDDrawSurface1 : public IDirectDrawSurface, public myIDDrawSurface_Generic
{
public:
	myIDDrawSurface1(LPDDSURFACEDESC a);
    virtual ~myIDDrawSurface1(void);

	// The original DDraw function definitions BEGIN
    HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);

	/*** IDirectDrawSurface methods ***/
    HRESULT  __stdcall AddAttachedSurface(LPDIRECTDRAWSURFACE);
    HRESULT  __stdcall AddOverlayDirtyRect(LPRECT);
    HRESULT  __stdcall Blt(LPRECT,LPDIRECTDRAWSURFACE, LPRECT,DWORD, LPDDBLTFX);
    HRESULT  __stdcall BltBatch(LPDDBLTBATCH, DWORD, DWORD );
    HRESULT  __stdcall BltFast(DWORD,DWORD,LPDIRECTDRAWSURFACE, LPRECT,DWORD);
    HRESULT  __stdcall DeleteAttachedSurface(DWORD,LPDIRECTDRAWSURFACE);
    HRESULT  __stdcall EnumAttachedSurfaces(LPVOID,LPDDENUMSURFACESCALLBACK);
    HRESULT  __stdcall EnumOverlayZOrders(DWORD,LPVOID,LPDDENUMSURFACESCALLBACK);
    HRESULT  __stdcall Flip(LPDIRECTDRAWSURFACE, DWORD);
    HRESULT  __stdcall GetAttachedSurface(LPDDSCAPS, LPDIRECTDRAWSURFACE FAR *);
    HRESULT  __stdcall GetBltStatus(DWORD);
    HRESULT  __stdcall GetCaps(LPDDSCAPS);
    HRESULT  __stdcall GetClipper(LPDIRECTDRAWCLIPPER FAR*);
    HRESULT  __stdcall GetColorKey(DWORD, LPDDCOLORKEY);
    HRESULT  __stdcall GetDC(HDC FAR *);
    HRESULT  __stdcall GetFlipStatus(DWORD);
    HRESULT  __stdcall GetOverlayPosition(LPLONG, LPLONG );
    HRESULT  __stdcall GetPalette(LPDIRECTDRAWPALETTE FAR*);
    HRESULT  __stdcall GetPixelFormat(LPDDPIXELFORMAT);
    HRESULT  __stdcall GetSurfaceDesc(LPDDSURFACEDESC);
    HRESULT  __stdcall Initialize(LPDIRECTDRAW, LPDDSURFACEDESC);
    HRESULT  __stdcall IsLost();
    HRESULT  __stdcall Lock(LPRECT,LPDDSURFACEDESC,DWORD,HANDLE);
    HRESULT  __stdcall ReleaseDC(HDC);
    HRESULT  __stdcall Restore();
    HRESULT  __stdcall SetClipper(LPDIRECTDRAWCLIPPER);
    HRESULT  __stdcall SetColorKey(DWORD, LPDDCOLORKEY);
    HRESULT  __stdcall SetOverlayPosition(LONG, LONG );
    HRESULT  __stdcall SetPalette(LPDIRECTDRAWPALETTE);
    HRESULT  __stdcall Unlock(LPVOID);
    HRESULT  __stdcall UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE,LPRECT,DWORD, LPDDOVERLAYFX);
    HRESULT  __stdcall UpdateOverlayDisplay(DWORD);
    HRESULT  __stdcall UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE);
	
	virtual myIDDrawPalette *getCurrentPalette() const { return mCurrentPalette; }
	virtual unsigned char * getSurfaceData() const { return mSurfaceData; }
	virtual int getPitch() const { return mPitch; }

	DDSURFACEDESC mSurfaceDesc;
	myIDDrawPalette *mCurrentPalette;
	unsigned char * mSurfaceData;
	unsigned char * mRealSurfaceData;
	int mWidth;
	int mHeight;
	int mPitch;
};

class myIDDrawSurface7 : public IDirectDrawSurface7, public myIDDrawSurface_Generic
{
public:
	myIDDrawSurface7(LPDDSURFACEDESC2 a);
    virtual ~myIDDrawSurface7(void);

	// The original DDraw function definitions BEGIN
	HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
	ULONG   __stdcall AddRef(void);
	ULONG   __stdcall Release(void);

	/*** IDirectDrawSurface methods ***/
	HRESULT  __stdcall AddAttachedSurface(LPDIRECTDRAWSURFACE7);
	HRESULT  __stdcall AddOverlayDirtyRect(LPRECT);
	HRESULT  __stdcall Blt(LPRECT,LPDIRECTDRAWSURFACE7, LPRECT,DWORD, LPDDBLTFX);
	HRESULT  __stdcall BltBatch(LPDDBLTBATCH, DWORD, DWORD );
	HRESULT  __stdcall BltFast(DWORD,DWORD,LPDIRECTDRAWSURFACE7, LPRECT,DWORD);
	HRESULT  __stdcall DeleteAttachedSurface(DWORD,LPDIRECTDRAWSURFACE7);
	HRESULT  __stdcall EnumAttachedSurfaces(LPVOID,LPDDENUMSURFACESCALLBACK7);
	HRESULT  __stdcall EnumOverlayZOrders(DWORD,LPVOID,LPDDENUMSURFACESCALLBACK7);
	HRESULT  __stdcall Flip(LPDIRECTDRAWSURFACE7, DWORD);
	HRESULT  __stdcall GetAttachedSurface(LPDDSCAPS2, LPDIRECTDRAWSURFACE7 FAR *);
	HRESULT  __stdcall GetBltStatus(DWORD);
	HRESULT  __stdcall GetCaps(LPDDSCAPS2);
	HRESULT  __stdcall GetClipper(LPDIRECTDRAWCLIPPER FAR*);
	HRESULT  __stdcall GetColorKey(DWORD, LPDDCOLORKEY);
	HRESULT  __stdcall GetDC(HDC FAR *);
	HRESULT  __stdcall GetFlipStatus(DWORD);
	HRESULT  __stdcall GetOverlayPosition(LPLONG, LPLONG );
	HRESULT  __stdcall GetPalette(LPDIRECTDRAWPALETTE FAR*);
	HRESULT  __stdcall GetPixelFormat(LPDDPIXELFORMAT);
	HRESULT  __stdcall GetSurfaceDesc(LPDDSURFACEDESC2);
	HRESULT  __stdcall Initialize(LPDIRECTDRAW, LPDDSURFACEDESC2);
	HRESULT  __stdcall IsLost();
	HRESULT  __stdcall Lock(LPRECT,LPDDSURFACEDESC2,DWORD,HANDLE);
	HRESULT  __stdcall ReleaseDC(HDC);
	HRESULT  __stdcall Restore();
	HRESULT  __stdcall SetClipper(LPDIRECTDRAWCLIPPER);
	HRESULT  __stdcall SetColorKey(DWORD, LPDDCOLORKEY);
	HRESULT  __stdcall SetOverlayPosition(LONG, LONG );
	HRESULT  __stdcall SetPalette(LPDIRECTDRAWPALETTE);
	HRESULT  __stdcall Unlock(LPRECT);
	HRESULT  __stdcall UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE7,LPRECT,DWORD, LPDDOVERLAYFX);
	HRESULT  __stdcall UpdateOverlayDisplay(DWORD);
	HRESULT  __stdcall UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE7);

	/*** Added in the v2 interface ***/
	HRESULT  __stdcall GetDDInterface(LPVOID FAR *);
	HRESULT  __stdcall PageLock(DWORD);
	HRESULT  __stdcall PageUnlock(DWORD);

	/*** Added in the V3 interface ***/
	HRESULT  __stdcall SetSurfaceDesc(LPDDSURFACEDESC2, DWORD);

	/*** Added in the v4 interface ***/
	HRESULT  __stdcall SetPrivateData(REFGUID, LPVOID, DWORD, DWORD);
	HRESULT  __stdcall GetPrivateData(REFGUID, LPVOID, LPDWORD);
	HRESULT  __stdcall FreePrivateData(REFGUID);
	HRESULT  __stdcall GetUniquenessValue(LPDWORD);
	HRESULT  __stdcall ChangeUniquenessValue();

	/*** Moved Texture7 methods here ***/
	HRESULT  __stdcall SetPriority(DWORD);
	HRESULT  __stdcall GetPriority(LPDWORD);
	HRESULT  __stdcall SetLOD(DWORD);
	HRESULT  __stdcall GetLOD(LPDWORD);

	virtual myIDDrawPalette *getCurrentPalette() const { return mCurrentPalette; }
	virtual unsigned char * getSurfaceData() const { return mSurfaceData; }
	virtual int getPitch() const { return mPitch; }

	DDSURFACEDESC2 mSurfaceDesc;
	myIDDrawPalette *mCurrentPalette;
	unsigned char * mSurfaceData;
	unsigned char * mRealSurfaceData;
	int mWidth;
	int mHeight;
	int mPitch;
};

class myIDDrawPalette : public IDirectDrawPalette
{
public:
    myIDDrawPalette(DWORD a, LPPALETTEENTRY b);
    virtual ~myIDDrawPalette(void);

	// The original DDraw function definitions BEGIN
    HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);
    
    /*** IDirectDrawPalette methods ***/
    HRESULT  __stdcall GetCaps(LPDWORD);
    HRESULT  __stdcall GetEntries(DWORD,DWORD,DWORD,LPPALETTEENTRY);
    HRESULT  __stdcall Initialize(LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
    HRESULT  __stdcall SetEntries(DWORD,DWORD,DWORD,LPPALETTEENTRY);

	PALETTEENTRY mPal[256];
};

class myIDDrawClipper : public IDirectDrawClipper
{
public:
	myIDDrawClipper(void);
	virtual ~myIDDrawClipper(void);

	// The original DDraw function definitions BEGIN
	HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
	ULONG   __stdcall AddRef(void);
	ULONG   __stdcall Release(void);

	/*** IDirectDrawClipper methods ***/
	HRESULT __stdcall GetClipList(LPRECT, LPRGNDATA, LPDWORD);
	HRESULT __stdcall GetHWnd(HWND FAR *);
	HRESULT __stdcall Initialize(LPDIRECTDRAW, DWORD);
	HRESULT __stdcall IsClipListChanged(BOOL FAR *);
	HRESULT __stdcall SetClipList(LPRGNDATA, DWORD);
	HRESULT __stdcall SetHWnd(DWORD, HWND);
	
	HWND mHwnd;
};
