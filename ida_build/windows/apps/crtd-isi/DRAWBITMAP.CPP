#include "stdafx.h"

void DrawBitmap(CDC *DC, int x, int y, CBitmap *Bitmap)
	{
	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(Bitmap);
	BITMAP bm;
	Bitmap->GetBitmap(&bm);
	DC->BitBlt( x, y, bm.bmWidth, bm.bmHeight, &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(CBitmapOld);
	MemDC.DeleteDC();
	}
