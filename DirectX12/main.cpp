#include <Windows.h>	//ウィンドウ表示するのに必要
#include "Application.h"
#include <tchar.h>


//プロトタイプ宣言
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;//コールバック関数の指定
	w.lpszClassName = _T("DirectX12");//アプリケーションクラス名
	w.hInstance = GetModuleHandle(0);//ハンドルの取得
	RegisterClassEx(&w);//アプリケーションクラス

						//ウィンドウサイズ設定
	RECT wrc = { 0, 0, 640, 480 };//ウィンドウサイズを設定
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//補正

													   //ウィンドウ生成
	HWND hwnd = CreateWindow(w.lpszClassName,//クラス名指定
		_T("DirectX12"),			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,		//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,			//表示X座標はOSにお任せ
		CW_USEDEFAULT,			//表示Y座標はOSにお任せ
		wrc.right - wrc.left,		//ウィンドウ幅
		wrc.bottom - wrc.top,		//ウィンドウ高
		nullptr,					//親ウィンドウのハンドル
		nullptr,					//メニューハンドル
		w.hInstance,				//呼び出しアプリケーションハンドル
		nullptr);				//追加パラメータ

								//ウィンドウ表示
	if (!Application::Instance().Run(hwnd)) {
		return 0;
	}
	Application::Instance().Delete();
}

//ウィンドウプロシージャ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {	//ウィンドウが破棄されたら呼ばれる
		PostQuitMessage(0);	//OSに対してアプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//規定の処理を行う
}
