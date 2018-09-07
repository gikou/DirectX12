#include "DX12Init.h"
#include "d3dx12.h"
#define WIN_WIDTH	(640)	//�E�B���h�E�T�C�Y��
#define WIN_HEIGTH	(480)	//�E�B���h�E�T�C�Y��


DX12Init::DX12Init(HWND hwnd)
{	//������
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = {};
	HRESULT result = S_OK;

	for (auto i : levels) {	//�}�V���ɂ������o�[�W������I��
		result = D3D12CreateDevice(nullptr, i, IID_PPV_ARGS(device.GetAddressOf()));
		if (result == S_OK) {
			level = i;
			break;
		}
	}

	
	result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_commandAllocator.GetAddressOf()));
	//�R�}���h���X�g�̍쐬
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(_commandList.GetAddressOf()));
	_commandList->Close();
	//�R�}���h�L���[�̍쐬
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = 0;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(_commandQueue.GetAddressOf()));

	//�X���b�v�`�F�C������
	result = CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf()));
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
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(swapChain.GetAddressOf()));

	int reanderNum = swapChainDesc.BufferCount;

	result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	fenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

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

	_vertexBuffer.Reset();
	root_sugnature_.Reset();

	vertex_shader.Reset();
	pixel_shader.Reset();
}


HRESULT 
DX12Init::Initialize() {
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