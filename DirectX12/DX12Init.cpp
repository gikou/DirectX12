#include "DX12Init.h"
#include "d3dx12.h"
#include<tchar.h>
#define WIN_WIDTH	(640)	//ウィンドウサイズ幅
#define WIN_HEIGTH	(480)	//ウィンドウサイズ高


DX12Init::DX12Init(HWND hwnd):_hwnd(hwnd)
{
	
}


DX12Init::~DX12Init()
{
	swapChain.Reset();
	device.Reset();
	_commandList.Reset();
	_commandAllocator.Reset();
	_commandQueue.Reset();

	factory.Reset();
	fence.Reset();
	
}

HRESULT 
DX12Init::CreateDevice() {
	//初期化
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = {};
	HRESULT result = E_FAIL;

	for (auto i : levels) {	//マシンにあったバージョンを選ぶ
		result = D3D12CreateDevice(nullptr, i, IID_PPV_ARGS(device.GetAddressOf()));
		if (result == S_OK) {
			level = i;
			break;
		}
	}
	return result;
}


HRESULT 
DX12Init::CreateCommand() {
	result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_commandAllocator.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}
	//コマンドリストの作成
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(_commandList.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}
	_commandList->Close();
	//コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = 0;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(_commandQueue.GetAddressOf()));
	return result;
}


HRESULT 
DX12Init::CreateSwapChain() {
	//スワップチェイン周り
	result = CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = WIN_WIDTH;
	swapChainDesc.Height = WIN_HEIGTH;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	result = factory->CreateSwapChainForHwnd(_commandQueue.Get(),
		_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(swapChain.GetAddressOf()));
	return result;
}

HRESULT
DX12Init::CreateFence() {
	result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	fenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	return result;
}

HRESULT 
DX12Init::Initialize() {
	if (FAILED(CreateDevice())) {
		MessageBox(nullptr, _T("デバイス生成に失敗しました"), _T("Init"), MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateCommand())) {
		MessageBox(nullptr, _T("コマンド生成に失敗しました"), _T("Init"), MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateSwapChain())) {
		MessageBox(nullptr, _T("スワップチェイン生成に失敗しました"), _T("Init"), MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateFence())) {
		MessageBox(nullptr, _T("フェンス生成に失敗しました"), _T("Init"), MB_OK | MB_ICONEXCLAMATION);
	}
	return S_OK;
}

HRESULT 
DX12Init::ResourceBarrier(std::vector<ID3D12Resource*> recource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(recource[swapChain->GetCurrentBackBufferIndex()], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	return S_OK;
}

UINT64 frames_ = 0;

HRESULT 
DX12Init::Wait() {

	const UINT64 fenceValue = frames_;
	result = _commandQueue->Signal(fence.Get(), frames_);
	++frames_;

	while (fence->GetCompletedValue() < fenceValue) {
		result = fence->SetEventOnCompletion(fenceValue, fenceEvent);
	}


	return S_OK;
}

ID3D12Device*
DX12Init::GetDevice() {
	return device.Get();
}
ID3D12GraphicsCommandList*
DX12Init::GetList() {
	return _commandList.Get();
}

ID3D12CommandQueue*
DX12Init::GetQueue() {
	return _commandQueue.Get();
}
ID3D12CommandAllocator*
DX12Init::GetAllocator() {
	return _commandAllocator.Get();
}
IDXGISwapChain3*
DX12Init::GetSwap() {
	return swapChain.Get();
}