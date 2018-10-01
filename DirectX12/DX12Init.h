#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include<vector>

using namespace Microsoft::WRL;

#pragma pack(1)
struct PMDVertex
{
	DirectX::XMFLOAT3 pos;//座標(12バイト) 
	DirectX::XMFLOAT3 normal;//法線(12バイト) 
	DirectX::XMFLOAT2 uv;//UV(8バイト) 
	unsigned short bornNum[2];//ボーン番号(4バイト) 
	unsigned char bornWeight;//ウェイト(1バイト) 
	unsigned char edgeFlag;//輪郭線フラグ(1バイト) 
};
#pragma pack(0)


class DX12Init
{
private:
	HRESULT result;
	HWND _hwnd;
	DirectX::XMMATRIX* matrixAddress;
	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX projection;

	std::vector<PMDVertex> vertices;

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
	HRESULT ResourceBarrier(std::vector<ID3D12Resource*> recource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	HRESULT Wait();
	void ClearRenderTarget(unsigned int bbindex);

public:
	DX12Init(HWND hwnd, ID3D12Device* device);
	~DX12Init();

	HRESULT Initialize();
	void Draw();
	void Delete();

	ID3D12Device* GetDevice();
	

};

