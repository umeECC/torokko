#include <memory>
#include <sstream>
#include <imgui.h>

#include "Framework.h"
#include "System/Audio.h"
#include "System/Input.h"
#include "System/Graphics.h"
#include "System/ImGuiRenderer.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "EffectManager.h"

// 垂直同期間隔設定
static const int syncInterval = 1;

// 課題内容
void DrawTaskGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 970, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Once);

	ImGui::SetNextWindowPos(ImVec2(pos.x + 490, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Once);
}

// コンストラクタ
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
{
	hDC = GetDC(hWnd);

	// オーディオ初期化
	Audio::Instance().Initialize();

	// インプット初期化
	Input::Instance().Initialize(hWnd);

	// グラフィックス初期化
	Graphics::Instance().Initialize(hWnd);

	// IMGUI初期化
	ImGuiRenderer::Initialize(hWnd, Graphics::Instance().GetDevice(), Graphics::Instance().GetDeviceContext());

	// エフェクトマネージャー初期化
	EffectManager::Instance().Initialize();

	// シーン初期化
	SceneManager::Instance().ChangeScene(new SceneTitle);
}

// デストラクタ
Framework::~Framework()
{
	// シーン終了化
	SceneManager::Instance().Clear();

	// エフェクトマネージャー終了化
	EffectManager::Instance().Finalize();

	// IMGUI終了化
	ImGuiRenderer::Finalize();

	// オーディオ終了化
	Audio::Instance().Finalize();

	ReleaseDC(hWnd, hDC);
}


void Framework::ToggleFullScreen()
{
	isFullScreen_ = !isFullScreen_;

	if (isFullScreen_)
	{
		// 現在のウィンドウスタイルと位置を保存
		windowStyle_ = GetWindowLong(hWnd, GWL_STYLE);
		GetWindowPlacement(hWnd, &windowPlacement_);

		// 枠（タイトルバー・境界線）を削除
		SetWindowLong(hWnd, GWL_STYLE, windowStyle_ & ~WS_OVERLAPPEDWINDOW);

		// モニター情報を取得
		HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hMonitor, &mi);

		// ウィンドウをモニター全体にリサイズ・移動
		SetWindowPos(hWnd, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	else
	{
		// スタイルとサイズを元に戻す
		SetWindowLong(hWnd, GWL_STYLE, windowStyle_);
		SetWindowPlacement(hWnd, &windowPlacement_);
		SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}




// 更新処理
void Framework::Update(float elapsedTime)
{
	// インプット更新処理
	Input::Instance().Update();

	// IMGUIフレーム開始処理	
	ImGuiRenderer::NewFrame();

	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);
}

// 描画処理
void Framework::Render(float elapsedTime)
{
	// 別スレッド中にデバイスコンテキストが使われていた場合に
	// 同時アクセスしないように排他制御する
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	// 画面クリア
	Graphics::Instance().Clear(0, 0, 1, 1);

	// レンダーターゲット設定
	Graphics::Instance().SetRenderTargets();

	// シーン描画処理
	SceneManager::Instance().Render();

	// シーンGUI描画処理
	SceneManager::Instance().DrawGUI();

	// 課題内容描画
	DrawTaskGUI();
#if 0
	// IMGUIデモウインドウ描画（IMGUI機能テスト用）
	ImGui::ShowDemoWindow();
#endif
	// IMGUI描画
	ImGuiRenderer::Render(dc);

	// 画面表示
	Graphics::Instance().Present(syncInterval);
}

// フレームレート計算
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// アプリケーションループ
int Framework::Run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// --- F11キーで仮想フルスクリーン切り替え ---
			if (GetAsyncKeyState(VK_F11) & 0x0001)  // 1回だけのトリガー
			{
				ToggleFullScreen();  // ← Frameworkクラスに実装しておいた関数
			}

			timer.Tick();
			CalculateFrameStats();

			float elapsedTime = syncInterval == 0
				? timer.TimeInterval()
				: syncInterval / static_cast<float>(GetDeviceCaps(hDC, VREFRESH))
				;
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}
	return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGuiRenderer::HandleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
