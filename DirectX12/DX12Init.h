#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include<vector>

using namespace Microsoft::WRL;

class DX12Init
{
private:
	HRESULT result;
	HWND _hwnd;

	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandAllocator> _commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> _commandList;
	ComPtr<ID3D12CommandQueue> _commandQueue;
	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	ComPtr<IDXGISwapChain3> swapChain;
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	//std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTarget;
	//std::vector<D3D12_CPU_DESCRIPTOR_HANDLE > rtvHandle;

	ComPtr<ID3D12Resource> _vertexBuffer;
	ComPtr<ID3D12RootSignature> root_sugnature_;
	D3D12_VERTEX_BUFFER_VIEW _vbView;
	ComPtr<ID3DBlob> vertex_shader{};
	ComPtr<ID3DBlob> pixel_shader{};

	HRESULT CreateFactory();
	HRESULT CreateCommand();
	HRESULT CreateFence();
	HRESULT CreateSwapChain();
	HRESULT CreateDescriptor();
	HRESULT CreateRenderTarget();

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect;
public:
	DX12Init(HWND hwnd);
	~DX12Init();

	HRESULT Initialize();

	HRESULT ResourceBarrier(std::vector<ID3D12Resource*> recource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	HRESULT Wait();

	ID3D12Device* GetDevice();
	ID3D12GraphicsCommandList* GetList();
	ID3D12CommandQueue* GetQueue();
	ID3D12CommandAllocator* GetAllocator();
	IDXGISwapChain3* GetSwap();
	//std::vector<D3D12_CPU_DESCRIPTOR_HANDLE > GetRenderTargetHandle();


};

