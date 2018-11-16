#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include<vector>
#include<memory>
#include<map>

using namespace Microsoft::WRL;



class Dx12BufferManager;
class PMDModel;
class PMXModel;
class LoadMotion;


//���[���h�r���[�v���W�F�N�g�̍\����
struct BaseMatrixes {
	DirectX::XMMATRIX world;//���[���h 
	DirectX::XMMATRIX viewproj;//�r���[�v���W�F 
	DirectX::XMFLOAT4 eye;
};
//�}�e���A�����\����
struct Material {
	DirectX::XMFLOAT4 diffuse;//��{�F(�g�U���ːF) 
	DirectX::XMFLOAT4 specular;//�X�y�L����(���ːF) 
	DirectX::XMFLOAT4 ambient;//�A���r�G���g 
};
//�{�[�����̍\����
struct BoneNode {
	int boneIdx;//�{�[���s��z��ƑΉ� 
	DirectX::XMFLOAT3 startPos;//�{�[���n�_(�֐ߏ������W) 
	DirectX::XMFLOAT3 endPos;//�{�[���I�_(���̊֐ߍ��W) 
	std::vector<BoneNode*> children;//�q�������ւ̃����N 
};

class DX12Init
{
private:
	HRESULT result;
	HWND _hwnd;
	

	
	Material* mapped;


	unsigned char* pData;
	unsigned char* _1stData;
	unsigned char* _2ndData;
	//DirectX12�������ϐ�
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

	//WVP�p
	BaseMatrixes* matrixAddress;
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 target;
	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX projection;
	ComPtr<ID3D12DescriptorHeap> registerDescHeap;	//�e�N�X�`����������A�A�A�萔�o�b�t�@��������A�A�A
	ComPtr<ID3D12Resource> _constantBuffer;

	//�[�x�o�b�t�@�p
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;
	ComPtr<ID3D12Resource> _depthBuffer;
	
	//���_���
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW indexView;
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> _indexBuffer;

	//�e�N�X�`���p
	ComPtr<ID3D12Resource> textureBuffer;
	std::vector<ComPtr<ID3D12Resource>> modelTextureBuffer;
	std::vector<ComPtr<ID3D12Resource>> sphirTextureBuffer;
	std::vector<ComPtr<ID3D12Resource>> toonTextureBuffer;
	ComPtr<ID3D12Resource> whiteTexBuffer;
	ComPtr<ID3D12Resource> blackTexBuffer;
	ComPtr<ID3D12Resource> normalTexBuffer;

	//�}�e���A���p
	std::vector<ComPtr<ID3D12Resource>> _materialsBuffer;
	ComPtr<ID3D12DescriptorHeap> materialDescHeap;	//�}�e���A���p
	std::shared_ptr<PMDModel> model;
	std::shared_ptr<PMXModel> pmxmodel;

	//�{�[���p
	DirectX::XMMATRIX* mappedBones;
	ComPtr<ID3D12Resource> boneBuffer;//�{�[���o�b�t�@ 
	ComPtr<ID3D12DescriptorHeap> boneHeap;//�{�[���p�q�[�v 
	std::vector<DirectX::XMMATRIX> boneMatrices;//�{�[���s��]���p
	std::map<std::string, BoneNode> boneMap;
	std::shared_ptr<LoadMotion> motion;

	ComPtr<ID3D12DescriptorHeap> _1stHeapRTV;
	ComPtr<ID3D12DescriptorHeap> _1stHeapSRV;
	ComPtr<ID3D12Resource> _1stPathBuffer;

	ComPtr<ID3D12DescriptorHeap> _2ndHeapRTV;
	ComPtr<ID3D12DescriptorHeap> _2ndHeapSRV;
	ComPtr<ID3D12Resource> _2ndPathBuffer;

	D3D12_VERTEX_BUFFER_VIEW _1stCanvasView;
	D3D12_VERTEX_BUFFER_VIEW _2ndCanvasView;
	ComPtr<ID3D12Resource> canvas1stBuffer;
	ComPtr<ID3D12Resource> canvas2ndBuffer;
	ComPtr<ID3D12RootSignature> _1stPathRootSignature;
	ComPtr<ID3D12PipelineState> _1stPathPipelineState;
	ComPtr<ID3D12RootSignature> _2ndPathRootSignature;
	ComPtr<ID3D12PipelineState> _2ndPathPipelineState;
	ComPtr<ID3D12DescriptorHeap> gauss1stDescHeap;
	ComPtr<ID3D12Resource> gauss1stBuffer;
	DirectX::XMFLOAT4* gauss1stMap;
	ComPtr<ID3D12DescriptorHeap> gauss2ndDescHeap;
	ComPtr<ID3D12Resource> gauss2ndBuffer;
	DirectX::XMFLOAT4* gauss2ndMap;
	HRESULT CreateDevice();
	HRESULT CreateCommand();
	HRESULT CreateFence();
	HRESULT CreateSwapChain();
	HRESULT CreateRenderTarget();
	HRESULT Create1stPathRTVSRV();
	HRESULT Create2ndPathRTVSRV();
	HRESULT CreateRootSgnature(); 
	HRESULT Create1stPathRootSgnature();
	HRESULT Create2ndPathRootSgnature();
	HRESULT CretaeToonTexture();
	HRESULT CreateModelTextures();
	void CreateWhiteTexBuffer();
	void CreateBlackTexBuffer();
	std::string GetToon(int index);
	HRESULT CreateShader();
	HRESULT CreateVertex();
	HRESULT CreateIndeis();
	HRESULT Create1stPathCanvasPorigonn();
	HRESULT Create2ndPathCanvasPorigonn();
	HRESULT CreateCanvasBuffer();
	HRESULT CreateConstantBuffer();
	HRESULT CreateMaterialBuffer();
	HRESULT CreateBone();
	HRESULT CreateBonesBuffer();
	HRESULT CreateDepth();
	HRESULT ResourceBarrier(std::vector<ID3D12Resource*> recource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	HRESULT Wait();
	void ClearRenderTarget(unsigned int bbindex);
	void RecursiveMatrixMultiply(BoneNode& node, DirectX::XMMATRIX& inMat);
	void BendBone(const char* name, DirectX::XMFLOAT4& q,DirectX::XMFLOAT3& loc, const DirectX::XMFLOAT4& q2, float t);
	void MotionUpdate(int frameNo);
	void CameraOperation(const char keyState);
	ComPtr<ID3D12CommandAllocator> bundleAllocator;
	ComPtr<ID3D12GraphicsCommandList> bundleList;
	void CreateModelDrawBundle();
public:
	DX12Init(HWND hwnd, ID3D12Device* device);
	~DX12Init();

	HRESULT Initialize();
	void Draw();
	void Delete();

	ID3D12Device* GetDevice();
	

};

