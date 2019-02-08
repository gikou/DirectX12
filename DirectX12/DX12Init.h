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
class RootSignature;
class PrimitiveCreator;

class RootSignature;
class PiplineState;

//���[���h�r���[�v���W�F�N�g�̍\����
struct BaseMatrixes {
	DirectX::XMMATRIX world;//���[���h 
	DirectX::XMMATRIX viewproj;//�r���[�v���W�F 
	DirectX::XMMATRIX ligth;
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


	HWND _hwnd;
	

	std::shared_ptr<PrimitiveCreator> primitive;
	std::map<std::string, std::shared_ptr<RootSignature>> rootsignater;
	std::map<std::string, std::shared_ptr<PiplineState>> piplineStates;
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
	ComPtr<ID3D12DescriptorHeap> registerDescHeap;	
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
	ComPtr<ID3D12DescriptorHeap> _1stHeapDSV;
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

	//shadow
	ComPtr<ID3D12DescriptorHeap> shadowHeapDSV;
	ComPtr<ID3D12DescriptorHeap> shadowHeapSRV;
	ComPtr<ID3D12Resource> shadowBuffer;
	ComPtr<ID3D12RootSignature> shadowRootSignature;
	ComPtr<ID3D12PipelineState> shadowPipelineState;

	void CreateDevice();
	void CreateCommand();
	void CreateFence();
	void CreateSwapChain();
	void CreateRenderTarget();
	void Create1stPathRTVSRV();
	void Create2ndPathRTVSRV();
	void CreateRootSgnature(); 
	void Create1stPathRootSgnature();
	void Create2ndPathRootSgnature();
	void CreateShadowRootSgnature();
	void CretaeToonTexture();
	void CreateModelTextures();
	void CreateWhiteTexBuffer();
	void CreateBlackTexBuffer();
	std::string GetToon(int index);
	void CreateShader();
	void CreateVertex();
	void CreateIndeis();
	void Create1stPathCanvasPorigonn();
	void Create2ndPathCanvasPorigonn();
	void CreateConstantBuffer();
	void CreateMaterialBuffer();
	void CreateBone();
	void CreateBonesBuffer();
	void CreateDepth();
	void CreateShadowMap();
	void DrawLightView();//���C�g����̎B�e 
	void ResourceBarrier(std::vector<ID3D12Resource*> recource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	void Wait();
	void SetViewAndScissor(unsigned int bbindex, ID3D12DescriptorHeap* heap);
	void CCDIK();
	void RecursiveMatrixMultiply(BoneNode& node, DirectX::XMMATRIX& inMat);
	void BendBone(const char* name, DirectX::XMFLOAT4& q,DirectX::XMFLOAT3& loc, const DirectX::XMFLOAT4& q2, float t);
	void MotionUpdate(int frameNo);
	void CameraOperation(const char keyState);
	ComPtr<ID3D12CommandAllocator> bundleAllocator;
	ComPtr<ID3D12GraphicsCommandList> bundleList;
	void CreateModelDrawBundle();
	void InputMove();
	DX12Init(const DX12Init&);
public:
	DX12Init();
	~DX12Init();
	void Dx12SetInit(HWND hwnd, ID3D12Device* device);
	static DX12Init& Instance() {
		static DX12Init dx12;
		return dx12;
	}

	void Initialize();
	void Draw();
	void Delete();

	ID3D12Device* GetDevice();
	ID3D12GraphicsCommandList* GetList();

};

