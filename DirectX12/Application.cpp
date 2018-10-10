#include <Windows.h>	//ウィンドウ表示するのに必要
#include "Application.h"
#include <d3d12.h>		//DirectX12を使うのに必要
#include "d3dx12.h"
#include <dxgi1_4.h>	//DXGIを扱うのに必要(DX12ではDXGI1.4が使われてる)
#include <D3Dcompiler.h>//シェーダコンパイラで必要
#include <DirectXMath.h>//数学系の便利なのが入ってるヘッダ
#include <tchar.h>
#include <vector>
#include <memory>
#include <wrl/client.h>

#include"DX12Init.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//頂点構造体
struct Vertex {
	DirectX::XMFLOAT3 pos;//座標
	DirectX::XMFLOAT2 uv;//uv座標
};

using namespace Microsoft::WRL;
using namespace DirectX;

#define WIN_WIDTH	(640)	//ウィンドウサイズ幅
#define WIN_HEIGTH	(480)	//ウィンドウサイズ高

#define Release(X)	{if((X) != nullptr) (X)->Release();}

Application::Application()
{
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);//WindowsのDLLに働きかける準備
}


Application::~Application()
{
	CoUninitialize();
}

bool
Application::Run(HWND hwnd) {


	ShowWindow(hwnd, SW_SHOW);

	HRESULT result = S_OK;
	//エラーを出力に表示させる
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (FAILED(result))
		int i = 0;
	debug->EnableDebugLayer();
	//Release(debug);
	debug = nullptr;
#endif

	ID3D12Device* device = nullptr;
	std::shared_ptr<DX12Init> dx12;
	dx12.reset(new DX12Init(hwnd, device));
	dx12->Initialize();
	device = dx12->GetDevice();
	//ループ
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {//OSから投げられてるメッセージをmsgに格納
			TranslateMessage(&msg);	//仮想キー関連の変換
			DispatchMessage(&msg);	//処理されなかったメッセージをOSに返す
		}
		dx12->Draw();
	}
	dx12.reset();

	ID3D12DebugDevice* debugInterface = nullptr;
	result = device->QueryInterface(&debugInterface);
	debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	Release(debugInterface);

	return true;
}
