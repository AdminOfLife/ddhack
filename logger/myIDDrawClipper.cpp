#include "StdAfx.h"
#include <varargs.h>



myIDDrawClipper::myIDDrawClipper(LPDIRECTDRAWCLIPPER pOriginal)
{
	logf(this, "myIDDrawClipper Constructor");
	m_pIDDrawClipper = pOriginal;
}


myIDDrawClipper::~myIDDrawClipper(void)
{
	logf(this, "myIDDrawClipper Destructor");
}


HRESULT __stdcall myIDDrawClipper::QueryInterface (REFIID, LPVOID FAR * b)
{
	logf(this, "myIDDrawClipper::QueryInterface");
	
	*b = NULL;

	return E_NOTIMPL;
}


ULONG   __stdcall myIDDrawClipper::AddRef(void)
{
	logf(this, "myIDDrawClipper::AddRef");
	return(m_pIDDrawClipper->AddRef());
}


ULONG   __stdcall myIDDrawClipper::Release(void)
{
	logf(this, "myIDDrawClipper::Release");
	
	// call original routine
	ULONG count = m_pIDDrawClipper->Release();
	
	logf(this, "Object Release."); 

    // in case no further Ref is there, the Original Object has deleted itself
	// so do we here
	if (count == 0) 
	{
		m_pIDDrawClipper = NULL;		
		delete(this); 
	}

	return(count);
}



HRESULT  __stdcall myIDDrawClipper::GetClipList(LPRECT a, LPRGNDATA b, LPDWORD c)
{
	logf(this, "myIDDrawClipper::GetClipList");
	return m_pIDDrawClipper->GetClipList(a,b,c);
}



HRESULT  __stdcall myIDDrawClipper::GetHWnd(HWND FAR *a)
{
	logf(this, "myIDDrawClipper::GetHWnd");
	return m_pIDDrawClipper->GetHWnd(a);
}



HRESULT  __stdcall myIDDrawClipper::Initialize(LPDIRECTDRAW a, DWORD b)
{
	logf(this, "myIDDrawClipper::Initialize");
	return m_pIDDrawClipper->Initialize(((myIDDrawInterface*)a)->getAsDirectDraw(),b);
}



HRESULT  __stdcall myIDDrawClipper::IsClipListChanged(BOOL FAR *a)
{
	logf(this, "myIDDrawClipper::IsClipListChanged");
	return m_pIDDrawClipper->IsClipListChanged(a);
}



HRESULT  __stdcall myIDDrawClipper::SetClipList(LPRGNDATA a, DWORD b)
{
	logf(this, "myIDDrawClipper::SetClipList");
	return m_pIDDrawClipper->SetClipList(a,b);
}



HRESULT  __stdcall myIDDrawClipper::SetHWnd(DWORD a, HWND b)
{
	logf(this, "myIDDrawClipper::SetHWnd");
	return m_pIDDrawClipper->SetHWnd(a,b);
}



