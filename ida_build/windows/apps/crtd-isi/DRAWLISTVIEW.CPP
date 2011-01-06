#include "stdafx.h"

BOOL CalcStringEllipsis(HDC hdc, LPTSTR lpszString, int cchMax, UINT uColWidth);

void DrawItemColumn(HDC hdc, LPTSTR lpsz, LPRECT prcClip);

//
//  FUNCTION:   DrawListViewItem(const LPDRAWITEMSTRUCT)
//
//  PURPOSE:    Draws one item in the listview control.
//
//  PARAMETERS:
//      lpDrawItem - Pointer to the information needed to draw the item.  The
//                   item number is in the itemID member.
//


void DrawListViewItem(LPDRAWITEMSTRUCT lpDrawItem, int nColumns, BOOL bBoldFont=FALSE)
	{
    HIMAGELIST himl;
    LV_ITEM lvi;
    int cxImage = 0, cyImage = 0;
    UINT uFirstColWidth;
    RECT rcClip;
    int iColumn = 1,i,g_cx;
	UINT uiFlags = ILD_TRANSPARENT;
	char cColumnText[128];
	HFONT hFont, hOldFont;


    // Get the item image to be displayed
    lvi.mask = LVIF_IMAGE | LVIF_STATE;
    lvi.iItem = lpDrawItem->itemID;
    lvi.iSubItem = 0;
    ListView_GetItem(lpDrawItem->hwndItem, &lvi);


	if(bBoldFont)
		{
		hFont = (HFONT)GetStockObject(SYSTEM_FONT);
		hOldFont = (HFONT)SelectObject(lpDrawItem->hDC, hFont);
		}

    // Check to see if this item is selected
    if (lpDrawItem->itemState & ODS_SELECTED)
		{
        // Set the text background and foreground colors
        SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
        SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHT));

		// Also add the ILD_BLEND50 so the images come out selected
		uiFlags |= ILD_BLEND50;
		}
    else
		{
        // Set the text background and foreground colors to the standard window
        // colors
        SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOW));
		}

    // Get the image list and draw the image
    himl = ListView_GetImageList(lpDrawItem->hwndItem, LVSIL_STATE);
    if (himl)
    {
    WORD state = ListView_GetItemState(lpDrawItem->hwndItem, 
                                              lpDrawItem->itemID, 
                                              LVIS_STATEIMAGEMASK);

    state = (state == 0x1000) ? 0 : 0x1;

        ImageList_Draw(himl, state, lpDrawItem->hDC,
                       lpDrawItem->rcItem.left, lpDrawItem->rcItem.top,
                       uiFlags);

        // Find out how big the image we just drew was
        ImageList_GetIconSize(himl, &cxImage, &cyImage);
    }

    // Calculate the width of the first column after the image width.  If
    // There was no image, then cxImage will be zero.

	g_cx=ListView_GetColumnWidth(lpDrawItem->hwndItem,0);

    uFirstColWidth = g_cx - cxImage;

    // Set up the new clipping rect for the first column text and draw it
    rcClip.left = lpDrawItem->rcItem.left + cxImage;
    rcClip.right = lpDrawItem->rcItem.left + g_cx;
    rcClip.top = lpDrawItem->rcItem.top;
    rcClip.bottom = lpDrawItem->rcItem.bottom;

	*cColumnText=0;
	ListView_GetItemText(lpDrawItem->hwndItem,lpDrawItem->itemID,0,cColumnText,sizeof(cColumnText));

    DrawItemColumn(lpDrawItem->hDC, cColumnText,&rcClip);

	for(i=1;i<nColumns;++i)
		{
    // Update the clip rect to the Symbol column
		g_cx=ListView_GetColumnWidth(lpDrawItem->hwndItem,i);

		*cColumnText=0;
		ListView_GetItemText(lpDrawItem->hwndItem,lpDrawItem->itemID,i,cColumnText,sizeof(cColumnText));
		rcClip.left = rcClip.right;
		rcClip.right = rcClip.left + g_cx;
		DrawItemColumn(lpDrawItem->hDC, cColumnText,&rcClip);
		}


    // If we changed the colors for the selected item, undo it
    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        // Set the text background and foreground colors
        SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOW));
    }

    // If the item is focused, now draw a focus rect around the entire row
    if (lpDrawItem->itemState & ODS_FOCUS)
    {
        // Adjust the left edge to exclude the image
        rcClip = lpDrawItem->rcItem;
        rcClip.left += cxImage;

        // Draw the focus rect
        DrawFocusRect(lpDrawItem->hDC, &rcClip);
    }

	if(bBoldFont)
		{
		SelectObject(lpDrawItem->hDC, hOldFont);
		}

    return;
	}


//
//  FUNCTION:   DrawItemColumn(HDC, LPTSTR, LPRECT)
//
//  PURPOSE:    Draws the text for one of the columns in the list view.
//
//  PARAMETERS:
//      hdc     - Handle of the DC to draw the text into.
//      lpsz    - String to draw.
//      prcClip - Rectangle to clip the string to.
//

void DrawItemColumn(HDC hdc, LPTSTR lpsz, LPRECT prcClip)
{
    TCHAR szString[256];

    // Check to see if the string fits in the clip rect.  If not, truncate
    // the string and add "...".
    lstrcpy(szString, lpsz);
    CalcStringEllipsis(hdc, szString, 256, prcClip->right - prcClip->left);

    // print the text
    ExtTextOut(hdc, prcClip->left + 2, prcClip->top + 1, ETO_CLIPPED | ETO_OPAQUE,
               prcClip, szString, lstrlen(szString), NULL);

}


//
//  FUNCTION:   CalcStringEllipsis(HDC, LPTSTR, int, UINT)
//
//  PURPOSE:    Determines whether the specified string is too wide to fit in
//              an allotted space, and if not truncates the string and adds some
//              points of ellipsis to the end of the string.
//
//  PARAMETERS:
//      hdc        - Handle of the DC the string will be drawn on.
//      lpszString - Pointer to the string to verify
//      cchMax     - Maximum size of the lpszString buffer.
//      uColWidth  - Width of the space in pixels to fit the string into.
//
//  RETURN VALUE:
//      Returns TRUE if the string needed to be truncated, or FALSE if it fit
//      into uColWidth.
//

BOOL CalcStringEllipsis(HDC hdc, LPTSTR lpszString, int cchMax, UINT uColWidth)
{
    const TCHAR szEllipsis[] = TEXT("...");
    SIZE   sizeString;
    SIZE   sizeEllipsis;
    int    cbString;
    LPTSTR lpszTemp;
    BOOL   fSuccess = FALSE;

    // Adjust the column width to take into account the edges
    uColWidth -= 4;

    __try
    {
        // Allocate a string for us to work with.  This way we can mangle the
        // string and still preserve the return value
        lpszTemp = (LPTSTR) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cchMax);
        if (!lpszTemp)
        {
            __leave;
        }
        lstrcpy(lpszTemp, lpszString);

        // Get the width of the string in pixels
        cbString = lstrlen(lpszTemp);
        if (!GetTextExtentPoint32(hdc, lpszTemp, cbString, &sizeString))
        {
            __leave;
        }

        // If the width of the string is greater than the column width shave
        // the string and add the ellipsis
        if ((ULONG)sizeString.cx > uColWidth)
        {
            if (!GetTextExtentPoint32(hdc, szEllipsis, lstrlen(szEllipsis),
                                       &sizeEllipsis))
            {
                __leave;
            }

            while (cbString > 0)
            {
                lpszTemp[--cbString] = 0;
                if (!GetTextExtentPoint32(hdc, lpszTemp, cbString, &sizeString))
                {
                    __leave;
                }

                if ((ULONG)(sizeString.cx + sizeEllipsis.cx) <= uColWidth)
                {
                    // The string with the ellipsis finally fits, now make sure
                    // there is enough room in the string for the ellipsis
                    if (cchMax >= (cbString + lstrlen(szEllipsis)))
                    {
                        // Concatenate the two strings and break out of the loop
                        lstrcat(lpszTemp, szEllipsis);
                        lstrcpy(lpszString, lpszTemp);
                        fSuccess = TRUE;
                        __leave;
                    }
                }
            }
        }
        else
        {
            // No need to do anything, everything fits great.
            fSuccess = TRUE;
        }
    }
    __finally
    {
        // Free the memory
        HeapFree(GetProcessHeap(), 0, (LPVOID)lpszTemp);
        return (fSuccess);
    }
}
