#include <Windows.h>

void gdi_write_string(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cchString, LPRECT lprc, UINT dwDTFormat);
void gdi_invalidate(const HWND hWnd, const RECT *lpRect);
unsigned char *gdi_get_buffer();