#include "StdAfx.h"
#include <varargs.h>


myIDDrawClipper::myIDDrawClipper(void)
{
}


myIDDrawClipper::~myIDDrawClipper(void)
{
}


HRESULT __stdcall myIDDrawClipper::QueryInterface (REFIID, LPVOID FAR * b)
{
	logf("myIDDrawClipper::QueryInterface");
	
	*b = NULL;

	return DDERR_UNSUPPORTED;
}


ULONG   __stdcall myIDDrawClipper::AddRef(void)
{
	logf("myIDDrawClipper::AddRef");
	return 1;
}


ULONG   __stdcall myIDDrawClipper::Release(void)
{
	logf("myIDDrawClipper::Release");
	delete this;
	
	return 0;
}


HRESULT  __stdcall myIDDrawClipper::GetClipList(LPRECT a, LPRGNDATA b, LPDWORD c)
{
	logf("myIDDrawClipper::GetClipList");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDrawClipper::GetHWnd(HWND FAR *a)
{
	logf("myIDDrawClipper::GetHWnd");
	a = &mHwnd;
	return DD_OK;
}


HRESULT  __stdcall myIDDrawClipper::Initialize(LPDIRECTDRAW a, DWORD b)
{
	logf("myIDDrawClipper::Initialize");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDrawClipper::IsClipListChanged(BOOL FAR *a)
{
	logf("myIDDrawClipper::IsClipListChanged");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDrawClipper::SetClipList(LPRGNDATA a, DWORD b)
{
	logf("myIDDrawClipper::SetClipList");
	return DDERR_UNSUPPORTED;
}


HRESULT  __stdcall myIDDrawClipper::SetHWnd(DWORD a, HWND b)
{
	logf("myIDDrawClipper::SetHWnd");
	memcpy(&mHwnd, &a, sizeof(mHwnd));
	return DD_OK;
}
