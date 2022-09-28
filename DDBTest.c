#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <strsafe.h>

#define CX_ICON 16
#define CY_ICON 16

HICON s_hOldIcon = NULL;
HICON s_hNewIcon = NULL;

// Borrowed from old kbswitch code
static HICON
CreateTrayIcon(LPTSTR szLCID)
{
    LANGID LangID;
    TCHAR szBuf[3];
    HDC hdc;
    HBITMAP hbmColor, hbmMono, hBmpOld;
    RECT rect;
    HFONT hFontOld, hFont;
    ICONINFO IconInfo;
    HICON hIcon;
    LOGFONT lf;

    /* Getting "EN", "FR", etc. from English, French, ... */
    LangID = (LANGID)_tcstoul(szLCID, NULL, 16);
    if (!GetLocaleInfo(LangID, LOCALE_SISO639LANGNAME, szBuf, ARRAYSIZE(szBuf)))
    {
        StringCchCopy(szBuf, ARRAYSIZE(szBuf), _T("??"));
    }
    CharUpper(szBuf);

    /* Create hdc, hbmColor and hbmMono */
    hdc = CreateCompatibleDC(NULL);
    hbmColor = CreateCompatibleBitmap(hdc, CX_ICON, CY_ICON);
    hbmMono = CreateBitmap(CX_ICON, CY_ICON, 1, 1, NULL);

    /* Create a font */
    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = -11;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfWeight = FW_NORMAL;
    StringCchCopy(lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), _T("Tahoma"));
    hFont = CreateFontIndirect(&lf);

    /* Checking NULL */
    if (!hdc || !hbmColor || !hbmMono || !hFont)
    {
        if (hdc)
            DeleteDC(hdc);
        if (hbmColor)
            DeleteObject(hbmColor);
        if (hbmMono)
            DeleteObject(hbmMono);
        if (hFont)
            DeleteObject(hFont);
        return NULL;
    }

    SetRect(&rect, 0, 0, CX_ICON, CY_ICON);

    /* Draw hbmColor */
    hBmpOld = SelectObject(hdc, hbmColor);
    SetDCBrushColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
    hFontOld = SelectObject(hdc, hFont);
    SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
    SetBkMode(hdc, TRANSPARENT);
    DrawText(hdc, szBuf, 2, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    SelectObject(hdc, hFontOld);
    SelectObject(hdc, hBmpOld);

    /* Fill hbmMono by black */
    hBmpOld = SelectObject(hdc, hbmMono);
    PatBlt(hdc, 0, 0, CX_ICON, CY_ICON, BLACKNESS);
    SelectObject(hdc, hBmpOld);

    /* Create an icon from hbmColor and hbmMono */
    IconInfo.hbmColor = hbmColor;
    IconInfo.hbmMask = hbmMono;
    IconInfo.fIcon = TRUE;
    hIcon = CreateIconIndirect(&IconInfo);

    /* Clean up */
    DeleteObject(hbmColor);
    DeleteObject(hbmMono);
    DeleteObject(hFont);
    DeleteDC(hdc);

    return hIcon;
}

/* New icon */
static HICON
CreateTrayIconNew(LPTSTR szLCID)
{
    LANGID LangID;
    TCHAR szBuf[4];
    HDC hdc;
    HBITMAP hbmColor, hbmMono, hBmpOld;
    RECT rect;
    HFONT hFontOld, hFont;
    ICONINFO IconInfo;
    HICON hIcon;
    LOGFONT lf;
    BITMAPINFO bmi;

    /* Getting "EN", "FR", etc. from English, French, ... */
    LangID = LOWORD(_tcstoul(szLCID, NULL, 16));
    if (!GetLocaleInfo(LangID, LOCALE_SABBREVLANGNAME | LOCALE_NOUSEROVERRIDE,
                       szBuf, ARRAYSIZE(szBuf)))
    {
        StringCchCopy(szBuf, ARRAYSIZE(szBuf), _T("??"));
    }
    szBuf[2] = 0; /* Truncate the identifiers to two characters: "ENG" --> "EN" etc. */

    /* Prepare for DIB (device-independent bitmap) */
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = CX_ICON;
    bmi.bmiHeader.biHeight = CY_ICON;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;

    /* Create hdc, hbmColor and hbmMono */
    hdc = CreateCompatibleDC(NULL);
    hbmColor = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, NULL, NULL, 0);
    hbmMono = CreateBitmap(CX_ICON, CY_ICON, 1, 1, NULL);

    /* Create a font */
    if (SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0))
        hFont = CreateFontIndirect(&lf);
    else
        hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    /* Checking NULL */
    if (!hdc || !hbmColor || !hbmMono || !hFont)
    {
        if (hdc)
            DeleteDC(hdc);
        if (hbmColor)
            DeleteObject(hbmColor);
        if (hbmMono)
            DeleteObject(hbmMono);
        if (hFont)
            DeleteObject(hFont);
        return NULL;
    }

    SetRect(&rect, 0, 0, CX_ICON, CY_ICON);

    /* Draw hbmColor */
    hBmpOld = SelectObject(hdc, hbmColor);
    SetDCBrushColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
    hFontOld = SelectObject(hdc, hFont);
    SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
    SetBkMode(hdc, TRANSPARENT);
    DrawText(hdc, szBuf, 2, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    SelectObject(hdc, hFontOld);
    SelectObject(hdc, hBmpOld);

    /* Fill hbmMono by black */
    hBmpOld = SelectObject(hdc, hbmMono);
    PatBlt(hdc, 0, 0, CX_ICON, CY_ICON, BLACKNESS);
    SelectObject(hdc, hBmpOld);

    /* Create an icon from hbmColor and hbmMono */
    IconInfo.hbmColor = hbmColor;
    IconInfo.hbmMask = hbmMono;
    IconInfo.fIcon = TRUE;
    hIcon = CreateIconIndirect(&IconInfo);

    /* Clean up */
    DeleteObject(hbmColor);
    DeleteObject(hbmMono);
    DeleteObject(hFont);
    DeleteDC(hdc);

    return hIcon;
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    s_hOldIcon = CreateTrayIcon(TEXT("04110411"));
    s_hNewIcon = CreateTrayIconNew(TEXT("04110411"));
    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    }
}

void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint(hwnd, &ps);
    if (hdc)
    {
        DrawIconEx(hdc, 4, 4, s_hOldIcon, 16, 16, 0, NULL, DI_NORMAL);
        DrawIconEx(hdc, 4, 30, s_hNewIcon, 16, 16, 0, NULL, DI_NORMAL);

        EndPaint(hwnd, &ps);
    }
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    InitCommonControls();
    DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
    return 0;
}
