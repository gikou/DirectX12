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
	DirectX::XMMATRIX* matrixAddress;

	unsigned char* pData;
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandAllocator> _commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> _commandList;
	ComPtr<ID3D12CommandQueue> _commandQueue;
	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	std::vector<ID3D12Resource*> renderTarget;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW indexView;
	ComPtr<ID3D12Resource> _indexBuffer;
	ComPtr<ID3D12DescriptorHeap> registerDescHeap;	//テクスチャだったり、、、定数バッファだったり、、、
	ComPtr<ID3D12Resource> _constantBuffer;
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> textureBuffer;


	HRESULT CreateDevice();
	HRESULT CreateCommand();
	HRESULT CreateFence();
	HRESULT CreateSwapChain();
	HRESULT CreateRenderTarget();
	HRESULT CreateRootSgnature();
	HRESULT CretaeTexture();
	HRESULT CreateShader();
	HRESULT CreateVertex();
	HRESULT CreateIndeis();
	HRESULT CreateConstantBuffer();



public:
	DX12Init(HWND hwnd, ID3D12Device* device);
	~DX12Init();

	HRESULT Initialize();

	HRESULT ResourceBarrier(std::vector<ID3D12Resource*> recource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	HRESULT Wait();
	void ClearRenderTarget(unsigned int bbindex);
	void Draw();
	ID3D12Device* GetDevice();
	void Delete();
	//ID3D12GraphicsCommandList* GetList();
	//ID3D12CommandQueue* GetQueue();
	//ID3D12CommandAllocator* GetAllocator();


};

