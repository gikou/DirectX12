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
	DirectX::XMMATRIX world;//���[���h 
	DirectX::XMMATRIX viewproj;//�r���[�v���W�F 

};

struct Material {
	DirectX::XMFLOAT3 diffuse;//��{�F(�g�U���ːF) 
	float alpha;//;�A���t�@�F 
	float specularity;//�X�y�L�������� 
	DirectX::XMFLOAT3 specular;//�X�y�L����(���ːF) 
	DirectX::XMFLOAT3 mirror;//�A���r�G���g 
	unsigned char toonIdx;//�g�D�[��̃C���f�b�N�X 
	unsigned char edgeFlg;//�֊s���t���O 
	int texflag;
};

class DX12Init
{
private:
	HRESULT result;
	HWND _hwnd;
	BaseMatrixes* matrixAddress;
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
	
	ComPtr<ID3D12DescriptorHeap> registerDescHeap;	//�e�N�X�`����������A�A�A�萔�o�b�t�@��������A�A�A
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;
	
	ComPtr<ID3D12Resource> _constantBuffer;
	ComPtr<ID3D12Resource> _depthBuffer;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW indexView;
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> _indexBuffer;

	ComPtr<ID3D12Resource> textureBuffer;


	std::vector<ComPtr<ID3D12Resource>> _materialsBuffer;
	ComPtr<ID3D12DescriptorHeap> materialDescHeap;	//�e�N�X�`����������A�A�A�萔�o�b�t�@��������A�A�A
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

