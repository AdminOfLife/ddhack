#include "StdAfx.h"
#include <varargs.h>
#include <Windows.h>


HRESULT WINAPI DirectDrawCreate(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter)
{
	logf("DirectDrawCreate(%08X,%08X,%08X)", lpGUID, lplpDD, pUnkOuter);

	*lplpDD = (LPDIRECTDRAW) new myIDDraw1();

	return 0;
}


HRESULT WINAPI DirectDrawCreateEx(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid, IUnknown FAR *pUnkOuter)
{
	logf("DirectDrawCreateEx(%08X,%08X,(guid),%08X)",lpGuid, lplpDD, pUnkOuter);

	// Hooking DDRAW interface from Original Library IDirectDraw *pDD; 

	if (iid == IID_IDirectDraw)
	{
		*lplpDD = (LPVOID) new myIDDraw1();
		
		return 0;
	}

	if (iid == IID_IDirectDraw2)
	{
		*lplpDD = (LPVOID) new myIDDraw2();

		return 0;
	}

	if (iid == IID_IDirectDraw4)
	{
		logf("IDDRAW4 requested");
	}

	if (iid == IID_IDirectDraw7)
	{
		*lplpDD = (LPVOID) new myIDDraw7();
		
		return 0;
	}

	logf("Unsupported ddraw interface version");
    ::ExitProcess(0);	
		
}


HRESULT WINAPI DirectDrawCreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter)
{
	logf("Exported function DirectDrawCreateClipper");
	
	*lplpDDClipper = new myIDDrawClipper();
	return (DD_OK);
}


HRESULT WINAPI DirectDrawEnumerateW(LPDDENUMCALLBACKW lpCallback, LPVOID lpContext)
{
	logf("Exported function DirectDrawEnumerateW");
	
	(*lpCallback)(NULL, L"Primary Display Driver", L"display", lpContext);
	return (DD_OK);
}


HRESULT WINAPI DirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback, LPVOID lpContext)
{
	logf("Exported function DirectDrawEnumerateA");
	
	
	(*lpCallback)(NULL, "Primary Display Driver", "display", lpContext);
	return (DD_OK);
}


HRESULT WINAPI DirectDrawEnumerateExW(LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags)
{
	logf("Exported function DirectDrawEnumerateExW");
	
	(*lpCallback)(NULL, L"Primary Display Driver", L"display", lpContext, 0);
	return (DD_OK);
}


HRESULT WINAPI DirectDrawEnumerateExA(LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags)
{
	logf("Exported function DirectDrawEnumerateExA");
	
	(*lpCallback)(NULL, "Primary Display Driver", "display", lpContext, 0);
	return (DD_OK);
}


VOID WINAPI AcquireDDThreadLock()
{
	logf("Exported function AcquireDDThreadLock");
}


VOID WINAPI ReleaseDDThreadLock()
{
	logf("Exported function ReleaseDDThreadLock");
	
}


DWORD WINAPI D3DParseUnknownCommand(LPVOID lpCmd, LPVOID *lpRetCmd)
{
	logf("Exported function D3DParseUnknownCommand");
	
	return(0);
}


HRESULT WINAPI DllCanUnloadNow(void)
{
	logf("Exported function DllCanUnloadNow");
	
	return(DDERR_UNSUPPORTED);
}

HRESULT WINAPI DllGetClassObject (const CLSID &rclsid, const IID &riid, void **ppv)
{
	logf("Exported function DllGetClassObject");
	
	return(DDERR_UNSUPPORTED);
}
