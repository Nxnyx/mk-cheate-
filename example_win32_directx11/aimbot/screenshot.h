#pragma once
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include "defines.h"

class screenshot {
public:
	screenshot();
	~screenshot();
	cv::Mat& get();

private:
	bool captureForegroundWindow();
	bool captureDesktop();

	HDC m_hWDC = nullptr;
	HDC m_hScreen = nullptr;
	HBITMAP m_hBitmap = nullptr;
	BITMAPINFO m_bitmapinfo = {};
	HGDIOBJ m_hGDI_temp = nullptr;

	int m_width = 0;
	int m_height = 0;
	int m_left = 0;
	int m_top = 0;

	char* m_data = nullptr;
	cv::Mat* m_screen = nullptr;
};
