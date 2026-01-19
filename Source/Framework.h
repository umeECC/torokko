#pragma once

#include <windows.h>
#include "System/HighResolutionTimer.h"

class Framework
{
public:
	Framework(HWND hWnd);
	~Framework();

private:
	void Update(float elapsedTime);
	void Render(float elapsedTime);

	void CalculateFrameStats();

	HWND hWnd_;
	WINDOWPLACEMENT windowPlacement_ = { };
	LONG windowStyle_ = 0;
	bool isFullScreen_ = false;

public:
	int Run();
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void ToggleFullScreen();           // フルスクリーン切替関数

private:
	const HWND				hWnd;
	HDC						hDC;
	HighResolutionTimer		timer;
};

