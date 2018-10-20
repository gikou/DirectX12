#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include<vector>
#include<memory>


using namespace Microsoft::WRL;



class Dx12BufferManager;
class PMDModel;
class PMXModel;

struct BaseMatrixes {
	DirectX::XMMATRIX world;//ワールド 
	DirectX::XMMATRIX viewproj;//ビュープロジェ 
	DirectX::XMFLOAT4 eye;
};

struct Material {
	DirectX::XMFLOAT4 diffuse;//基本色(拡散反射色) 
	DirectX::XMFLOAT4 specular;//スペキュラ(反射色) 
	DirectX::XMFLOAT4 ambient;//アンビエント 
};

class DX12Init
{
private:
	HRESULT result;
	HWND _hwnd;
	BaseMatrixes* matrixAddress;
	DirectX::XMFLOAT3 eye;
	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX projection;

	
	Material* mapped;

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
	
	ComPtr<ID3D12DescriptorHeap> registerDescHeap;	//テクスチャだったり、、、定数バッファだったり、、、
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;
	
	ComPtr<ID3D12Resource> _constantBuffer;
	ComPtr<ID3D12Resource> _depthBuffer;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW indexView;
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> _indexBuffer;

	ComPtr<ID3D12Resource> textureBuffer;
	std::vector<ComPtr<ID3D12Resource>> modelTextureBuffer;
	std::vector<ComPtr<ID3D12Resource>> toonTextureBuffer;
	ComPtr<ID3D12Resource> whiteTexBuffer;
	ComPtr<ID3D12Resource> blackTexBuffer;


	std::vector<ComPtr<ID3D12Resource>> _materialsBuffer;
	ComPtr<ID3D12DescriptorHeap> materialDescHeap;	//マテリアル用
	ComPtr<ID3D12DescriptorHeap> specularDescHeap;	//マテリアル用
	//std::shared_ptr<Dx12BufferManager> buffer;
	std::shared_ptr<PMDModel> model;
	std::shared_ptr<PMXModel> pmxmodel;

	HRESULT CreateDevice();
	HRESULT CreateCommand();
	HRESULT CreateFence();
	HRESULT CreateSwapChain();
	HRESULT CreateRenderTarget();
	HRESULT CreateRootSgnature();
	HRESULT CretaeTexture();
	HRESULT CretaeToonTexture();
	HRESULT CreateModelTextures();
	void CreateWhiteTexBuffer();
	void CreateBlackTexBuffer();
	std::string GetToon(int index);
	HRESULT CreateShader();
	HRESULT CreateVertex();
	HRESULT CreateIndeis();
	HRESULT CreateConstantBuffer();
	HRESULT CreateMaterialBuffer();
	HRESULT CreateDepth();
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

