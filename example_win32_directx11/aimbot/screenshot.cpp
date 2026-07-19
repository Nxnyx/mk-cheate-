#include "screenshot.h"

screenshot::screenshot() {
	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_left = static_cast<int>(screenWidth / 2 - ACTIVATION_RANGE / 2);
	m_top = static_cast<int>(screenHeight / 2 - ACTIVATION_RANGE / 2);
	m_width = ACTIVATION_RANGE;
	m_height = ACTIVATION_RANGE;

	HWND hwnd = GetDesktopWindow();
	m_hWDC = GetWindowDC(hwnd);
	m_hScreen = CreateCompatibleDC(m_hWDC);
	m_hBitmap = CreateCompatibleBitmap(m_hWDC, m_width, m_height);
	m_hGDI_temp = SelectObject(m_hScreen, m_hBitmap);

	m_bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bitmapinfo.bmiHeader.biWidth = m_width;
	m_bitmapinfo.bmiHeader.biHeight = -m_height;
	m_bitmapinfo.bmiHeader.biPlanes = 1;
	m_bitmapinfo.bmiHeader.biBitCount = 24;
	m_bitmapinfo.bmiHeader.biCompression = BI_RGB;

	const int step = static_cast<int>(ceil(m_width * 3 / static_cast<double>(4))) * 4;
	m_data = new char[step * m_height];
	m_screen = new cv::Mat(m_height, m_width, CV_8UC3, m_data, step);
}

screenshot::~screenshot() {
	SelectObject(m_hScreen, m_hGDI_temp);
	DeleteObject(m_hBitmap);
	DeleteDC(m_hScreen);
	ReleaseDC(GetDesktopWindow(), m_hWDC);
	delete m_screen;
	delete[] m_data;
}

bool screenshot::captureForegroundWindow()
{
	HWND foreground = GetForegroundWindow();
	if (!foreground || foreground == GetDesktopWindow())
		return false;

	RECT clientRect = {};
	if (!GetClientRect(foreground, &clientRect))
		return false;

	const int winWidth = clientRect.right - clientRect.left;
	const int winHeight = clientRect.bottom - clientRect.top;
	if (winWidth < m_width || winHeight < m_height)
		return false;

	HDC windowDC = GetDC(foreground);
	if (!windowDC)
		return false;

	HDC memDC = CreateCompatibleDC(windowDC);
	HBITMAP fullBitmap = CreateCompatibleBitmap(windowDC, winWidth, winHeight);
	if (!memDC || !fullBitmap)
	{
		if (fullBitmap) DeleteObject(fullBitmap);
		if (memDC) DeleteDC(memDC);
		ReleaseDC(foreground, windowDC);
		return false;
	}

	HGDIOBJ oldBitmap = SelectObject(memDC, fullBitmap);
	const BOOL printed = PrintWindow(foreground, memDC, PW_RENDERFULLCONTENT);
	if (printed)
	{
		const int cropLeft = (winWidth - m_width) / 2;
		const int cropTop = (winHeight - m_height) / 2;
		BitBlt(m_hScreen, 0, 0, m_width, m_height, memDC, cropLeft, cropTop, SRCCOPY);
		GetDIBits(m_hScreen, m_hBitmap, 0, m_height, m_data, &m_bitmapinfo, DIB_RGB_COLORS);
	}

	SelectObject(memDC, oldBitmap);
	DeleteObject(fullBitmap);
	DeleteDC(memDC);
	ReleaseDC(foreground, windowDC);
	return printed != FALSE;
}

bool screenshot::captureDesktop()
{
	BitBlt(m_hScreen, 0, 0, m_width, m_height, m_hWDC, m_left, m_top, SRCCOPY);
	GetDIBits(m_hScreen, m_hBitmap, 0, m_height, m_data, &m_bitmapinfo, DIB_RGB_COLORS);
	return true;
}

cv::Mat& screenshot::get() {
	if (!captureForegroundWindow())
		captureDesktop();
	return *m_screen;
}
