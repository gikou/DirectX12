#include <d3d12.h>		//DirectX12���g���̂ɕK�v
#include "d3dx12.h"
#include <dxgi1_4.h>	//DXGI�������̂ɕK�v(DX12�ł�DXGI1.4���g���Ă�)
#include <D3Dcompiler.h>//�V�F�[�_�R���p�C���ŕK�v
#include <DirectXMath.h>//���w�n�֗̕��Ȃ̂������Ă�w�b�_
#include <tchar.h>
#include <vector>
#include <memory>
#include <wrl/client.h>
#include<DirectXTex.h>
#include"DX12Init.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

#define WIN_WIDTH	(640)	//�E�B���h�E�T�C�Y��
#define WIN_HEIGTH	(480)	//�E�B���h�E�T�C�Y��
using namespace DirectX;
HRESULT result = S_OK;
//���_�\����
struct Vertex {
	DirectX::XMFLOAT3 pos;//���W
	DirectX::XMFLOAT2 uv;//uv���W
};

DX12Init::DX12Init(HWND hwnd, ID3D12Device* device) :_hwnd(hwnd), device(device)
{
}


DX12Init::~DX12Init()
{
	Delete();
}

void
DX12Init::Delete() {
	
}

HRESULT
DX12Init::CreateDevice() {
	//std::vector <IDXGIAdapter*> adapters;
	//IDXGIAdapter* adapter = nullptr;
	//IDXGIFactory* _factory;
	//
	//for (int i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
	//	adapters.push_back(adapter);
	//	//���̒����� NVIDIA �̓z��T��
	//	for (auto adpt : adapters) {
	//		DXGI_ADAPTER_DESC adesc = {};
	//		adpt->GetDesc(&adesc);
	//		std::wstring strDesc = adesc.Description;
	//		if (strDesc.find(L"NVIDIA") != std::string::npos) {//NVIDIA�A�_�v�^������ 
	//			adapter = adpt;
	//			break;
	//		}
	//	}
	//}
	//
	//������
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = {};

	for (auto i : levels) {	//�}�V���ɂ������o�[�W������I��
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

	//�R�}���h���X�g�̍쐬
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(_commandList.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}

	_commandList->Close();
	//�R�}���h�L���[�̍쐬
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = 0;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(_commandQueue.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}
	return result;
}

HRESULT
DX12Init::CreateFence() {
	result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	fenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	return result;
}

HRESULT
DX12Init::CreateSwapChain() {

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
		_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(swapChain.GetAddressOf()));
	return result;

}

HRESULT
DX12Init::CreateRenderTarget() {
	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC descriptor = {};
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor.NumDescriptors = 2;
	descriptor.NodeMask = 0;

	result = device->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}
	//�f�B�X�N���v�^�n���h���̍쐬
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart());

	//�����_�[�^�[�Q�b�g

	//�����_�[�^�[�Q�b�g���Ԃ�m��
	renderTarget.resize(2);
	//�f�B�X�N���v�^1������̃T�C�Y���擾
	unsigned int descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (int i = 0; i < 2; ++i) {
		result = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i]));//�X���b�v�`�F�C������L�����o�X���擾
		device->CreateRenderTargetView(renderTarget[i], nullptr, descriptorHandle);//�L�����o�X�ƃr���[��R�Â�
		descriptorHandle.Offset(descriptorSize);//�L�����o�X�ƃr���[�̂Ԃ񎟂̂Ƃ���܂ŃI�t�Z�b�g
	}
	return result;
}

HRESULT
DX12Init::CreateRootSgnature() {
	//�T���v���̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc.RegisterSpace = 0;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	//�f�B�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER parameter = {};
	//"t0"�ɗ���
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTblRange[0].NumDescriptors = 1;
	descTblRange[0].BaseShaderRegister = 0;
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//"b0"�ɗ���
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRange[1].NumDescriptors = 1;
	descTblRange[1].BaseShaderRegister = 0;
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//�f�X�N���v�^�[�e�[�u���̐ݒ�
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter.DescriptorTable.NumDescriptorRanges = 2; //�����W�̐�
	parameter.DescriptorTable.pDescriptorRanges = descTblRange;

	//���[�g�V�O�l�`��
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 1;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pParameters = &parameter;
	rsDesc.pStaticSamplers = &samplerDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);
	if (FAILED(result)) {
		return result;
	}

	result = device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
	return result;
}

HRESULT
DX12Init::CreateVertex() {

	//���_���̍쐬
	//Vertex vertices[] = {
	//{ { -0.7f	, -0.7f , 0.0f }		,{ 0.0f, 0.0f } },
	//{ { -0.7f	,  0.7f	, 0.0f }		,{ 0.0f, 1.0f } },
	//{ { 0.7f	, -0.7f	, 0.0f }		,{ 1.0f, 0.0f } },
	//{ { 0.7f	, 0.7f	, 0.0f }		,{ 1.0f, 1.0f } },

	//};

	/*Vertex vertices[] = {
	{ XMFLOAT3(0, 0, 0),XMFLOAT2(0, 0) },
	{ XMFLOAT3(320,   0, 0),XMFLOAT2(1, 0) },
	{ XMFLOAT3(0, 240, 0),XMFLOAT2(0, 1) },
	{ XMFLOAT3(320, 240, 0),XMFLOAT2(1, 1) },

	{ XMFLOAT3(0, 240, 0),XMFLOAT2(0, 0) },
	{ XMFLOAT3(0, 240, 640),XMFLOAT2(1, 0) },
	{ XMFLOAT3(320, 240, 0),XMFLOAT2(0, 1) },
	{ XMFLOAT3(320, 480, 640),XMFLOAT2(1, 1) },
	};*/

	Vertex vertices[] = {
		{ XMFLOAT3(-0.5, -0.5, -0.5),XMFLOAT2(0, 0) },
		{ XMFLOAT3(-0.5, -0.5, 0.5),XMFLOAT2(1, 0) },
		{ XMFLOAT3(0.5, -0.5, -0.5),XMFLOAT2(0, 1) },
		{ XMFLOAT3(0.5, -0.5, 0.5),XMFLOAT2(1, 1) },

	{ XMFLOAT3(-0.5, 0.5, 0.5),XMFLOAT2(0, 0) },
	{ XMFLOAT3(-0.5, 0.5, -0.5),XMFLOAT2(1, 0) },
	{ XMFLOAT3(-0.5, -0.5, 0.5),XMFLOAT2(0, 1) },
	{ XMFLOAT3(-0.5, -0.5, -0.5),XMFLOAT2(1, 1) },

	{ XMFLOAT3(0.5, 0.5, 0.5),XMFLOAT2(0, 0) },
	{ XMFLOAT3(0.5, 0.5, -0.5),XMFLOAT2(1, 0) },
	{ XMFLOAT3(0.5, -0.5, 0.5),XMFLOAT2(0, 1) },
	{ XMFLOAT3(0.5, -0.5, -0.5),XMFLOAT2(1, 1) },

	{ XMFLOAT3(-0.5, 0.5, -0.5),XMFLOAT2(0, 0) },
	{ XMFLOAT3(0.5,   0.5, -0.5),XMFLOAT2(1, 0) },
	{ XMFLOAT3(-0.5, -0.5, -0.5),XMFLOAT2(0, 1) },
	{ XMFLOAT3(0.5, -0.5, -0.5),XMFLOAT2(1, 1) },
	};



	/*Vertex vertices[] = {
	{ { -0.7f,  0.7f, 0.0f },{ 0.0f, 0.0f } },
	{ { 0.7f,  0.7f, 0.0f },{ 1.0f, 0.0f } },
	{ { 0.7f, -0.7f, 0.0f },{ 1.0f, 1.0f } },

	{ { 0.7f, -0.7f, 0.0f },{ 1.0f, 1.0f } },
	{ { -0.7f, -0.7f, 0.0f },{ 0.0f, 1.0f } },
	{ { -0.7f,  0.7f, 0.0f },{ 0.0f, 0.0f } }
	};*/

	//���_�o�b�t�@�̍쐬
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPU����GPU�֓]������p
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),//�T�C�Y
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexBuffer.GetAddressOf())
	);
	if (FAILED(result)) {
		return result;
	}
	//�o�b�t�@�ɑ΂��ď�������
	result = vertexBuffer.Get()->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	memcpy(pData, vertices, sizeof(vertices));//���_�f�[�^���o�b�t�@�ɃR�s�[
	vertexBuffer.Get()->Unmap(0, nullptr);
	if (FAILED(result)) {
		return result;
	}

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();//���_�A�h���X��GPU�ɂ���A�h���X���L��
	vbView.StrideInBytes = sizeof(Vertex);//���_1������̃o�C�g�����w��
	vbView.SizeInBytes = sizeof(vertices);//�f�[�^�S�̂̃T�C�Y���w��

	return result;
}

HRESULT
DX12Init::CreateIndeis() {
	std::vector<unsigned short> indices = { 0,1,2, 1,3,2,   4,5,6, 5,7,6,   8,9,10, 9,11,10,  12,13,14, 13,15,14 };

	//���_�o�b�t�@�̍쐬

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPU����GPU�֓]������p
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0])),//�T�C�Y
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_indexBuffer.GetAddressOf())
	);
	if (FAILED(result)) {
		return result;
	}
	//�o�b�t�@�ɑ΂��ď�������
	unsigned short* indexData = nullptr;
	result = _indexBuffer.Get()->Map(0, 0, reinterpret_cast<void**>(&indexData));
	memcpy(indexData, &indices[0], indices.size() * sizeof(indices[0]));//���_�f�[�^���o�b�t�@�ɃR�s�[
	_indexBuffer.Get()->Unmap(0, nullptr);
	if (FAILED(result)) {
		return result;
	}
	//���_�o�b�t�@�r���[�̍쐬
	indexView = {};
	indexView.BufferLocation = _indexBuffer.Get()->GetGPUVirtualAddress();//���_�A�h���X��GPU�ɂ���A�h���X���L��
	indexView.Format = DXGI_FORMAT_R16_UINT;
	indexView.SizeInBytes = indices.size() * sizeof(indices[0]);//�f�[�^�S�̂̃T�C�Y���w��

	return result;
}

HRESULT
DX12Init::CretaeTexture() {

	TexMetadata metadata = {};
	ScratchImage image;

	//auto result = DirectX::LoadFromWICFile(L"image/aoba.bmp", 0, &metadata, image);
	result = DirectX::LoadFromWICFile(L"image/precure.jpg", 0, &metadata, image);

	//�e�N�X�`�����\�[�X�̍쐬
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texResourceDesc = {};
	texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Width = metadata.width;
	texResourceDesc.Height = metadata.height;
	texResourceDesc.DepthOrArraySize = metadata.arraySize;
	texResourceDesc.MipLevels = metadata.mipLevels;
	texResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.SampleDesc.Count = 1;
	texResourceDesc.SampleDesc.Quality = 0;
	texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.Type = D3D12_HEAP_TYPE_CUSTOM;
	hprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	hprop.CreationNodeMask = 1;
	hprop.VisibleNodeMask = 1;

	result = device->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(textureBuffer.GetAddressOf())
	);
	if (FAILED(result)) {
		return result;
	}
	
	/*_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), nullptr);*/
	//�e�N�X�`���̓ǂݍ���
	unsigned char *data = image.GetPixels();
	result = textureBuffer->WriteToSubresource(
		0,
		nullptr,
		image.GetPixels(),
		metadata.width * 4,
		image.GetPixelsSize());

	image.Release();

	//�V�F�[�_���\�[�X�r���[�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 2;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(registerDescHeap.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}
	unsigned int stride = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_SHADER_RESOURCE_VIEW_DESC sDesc = {};
	sDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	sDesc.Texture2D.MipLevels = 1;
	sDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	device->CreateShaderResourceView(textureBuffer.Get(), &sDesc, registerDescHeap.Get()->GetCPUDescriptorHandleForHeapStart());


	return result;
}


HRESULT
DX12Init::CreateShader() {
	ComPtr<ID3DBlob> vertexShader;//���_�V�F�[�_�[
	ComPtr<ID3DBlob> pixelShader;//�s�N�Z���V�F�[�_�[

								 //���_���C�A�E�g�̒�`
	D3D12_INPUT_ELEMENT_DESC input[] = {
		//���_
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 }
	};

	//�V�F�[�_�[�̃R���p�C�����s��
	result = D3DCompileFromFile(_T("Shader.hlsl"), nullptr, nullptr, "BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader, nullptr);
	if (FAILED(result)) {
		return result;
	}
	result = D3DCompileFromFile(_T("Shader.hlsl"), nullptr, nullptr, "BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader, nullptr);
	if (FAILED(result)) {
		return result;
	}

	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g(PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.DepthStencilState.DepthEnable = false;
	gpsDesc.DepthStencilState.StencilEnable = false;
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	gpsDesc.InputLayout.NumElements = _countof(input);
	gpsDesc.InputLayout.pInputElementDescs = input;
	gpsDesc.pRootSignature = rootSignature.Get();
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.SampleMask = UINT_MAX;

	result = device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&pipelineState));
	return result;
}

HRESULT
DX12Init::CreateConstantBuffer() {

	auto matrix = XMMatrixIdentity();
	auto eye = XMFLOAT3(0, 0, -1);
	auto target = XMFLOAT3(0, 0, 0);
	auto up = XMFLOAT3(0, 1, 0);
	matrix *= XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matrix *= XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(640) / static_cast<float>(480), 0.1f, 300.0f);
	

	/*matrix.r[0].m128_f32[0] = 2.f / 640.f;
	matrix.r[1].m128_f32[1] = -2.f / 480.f;
	matrix.r[3].m128_f32[0] = -1;
	matrix.r[3].m128_f32[1] = 1;*/

	size_t size = sizeof(matrix);
	size = (size + 0xff)&~0xff;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_constantBuffer.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}
	

	D3D12_RANGE range = { 0,0 };
	
	result = _constantBuffer.Get()->Map(0, &range, (void**)&matrixAddress);
	*matrixAddress = matrix;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constantBuffer.Get()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = size;
	auto handle = registerDescHeap->GetCPUDescriptorHandleForHeapStart();

	handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	device->CreateConstantBufferView(&cbvDesc, handle);
	return result;
}


HRESULT
DX12Init::Initialize() {
	if (FAILED(CreateDevice())) {
		MessageBox(nullptr, L"Error", L"�f�o�C�X�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateCommand())) {
		MessageBox(nullptr, L"Error", L"�R�}���h�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateFence())) {
		MessageBox(nullptr, L"Error", L"�t�F���X�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateSwapChain())) {
		MessageBox(nullptr, L"Error", L"�X���b�v�`�F�C���쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateRenderTarget())) {
		MessageBox(nullptr, L"Error", L"�����_�[�^�[�Q�b�g�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateRootSgnature())) {
		MessageBox(nullptr, L"Error", L"���[�g�V�O�l�`���[�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateVertex())) {
		MessageBox(nullptr, L"Error", L"Vertex�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateIndeis())) {
		MessageBox(nullptr, L"Error", L"Index�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CretaeTexture())) {
		MessageBox(nullptr, L"Error", L"�e�N�X�`���쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateShader())) {
		MessageBox(nullptr, L"Error", L"�V�F�[�_�[�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}
	if (FAILED(CreateConstantBuffer())) {
		MessageBox(nullptr, L"Error", L"�J�����쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}

	return result;
}

HRESULT
DX12Init::ResourceBarrier(std::vector<ID3D12Resource*> recource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(recource[swapChain->GetCurrentBackBufferIndex()], before, after));
	return S_OK;
}

HRESULT
DX12Init::Wait() {
	static UINT64 frames_ = 0;
	const UINT64 fenceValue = frames_;
	result = _commandQueue->Signal(fence.Get(), frames_);
	++frames_;

	while (fence->GetCompletedValue() < fenceValue) {
		result = fence->SetEventOnCompletion(fenceValue, fenceEvent);
	}


	return S_OK;
}


void
DX12Init::ClearRenderTarget(unsigned int bbindex) {
	static const float color[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	//�r���[�|�[�g
	D3D12_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = WIN_WIDTH;
	viewPort.Height = WIN_HEIGTH;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	//���[�g�V�O�l�`���̃Z�b�g
	_commandList->SetGraphicsRootSignature(rootSignature.Get());

	//�p�C�v���C���̃Z�b�g
	_commandList->SetPipelineState(pipelineState.Get());

	//�r���[�|�[�g���Z�b�g
	_commandList->RSSetViewports(1, &viewPort);
	//�V�U�[���Z�b�g
	const D3D12_RECT rect = { 0, 0, WIN_WIDTH, WIN_HEIGTH };
	_commandList->RSSetScissorRects(1, &rect);


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart(), bbindex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	_commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
	_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
}

void
DX12Init::Draw() {
	static unsigned int bbIndex = 0;
	_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), pipelineState.Get());

	ResourceBarrier(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	ClearRenderTarget(bbIndex);

	//�V�F�[�_���\�[�X�r���[�p�̃f�X�N���v�^���Z�b�g
	_commandList->SetDescriptorHeaps(1, (ID3D12DescriptorHeap* const*)registerDescHeap.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(0, registerDescHeap.Get()->GetGPUDescriptorHandleForHeapStart());

	static float angle = 0.0f;
	auto mat = XMMatrixRotationY(angle);
	auto eye = XMFLOAT3(0, 0, -1);
	auto target = XMFLOAT3(0, 0, 0);
	auto up = XMFLOAT3(0, 1, 0);
	mat *= XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	mat *= XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(640) / static_cast<float>(480), 0.1f, 300.0f);
	*matrixAddress = mat;
	angle += 0.01f;

	//�O�p�|���S���`��ɂ���
	_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//���_�o�b�t�@�̃Z�b�g
	_commandList->IASetVertexBuffers(0, 1, &vbView);
	_commandList->IASetIndexBuffer(&indexView);
	//���_�`��
	int cont = indexView.SizeInBytes/sizeof(unsigned short);
	_commandList->DrawIndexedInstanced(cont, cont/3, 0, 0, 0);

	ResourceBarrier(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	_commandList->Close();
	ID3D12CommandList* commandList[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(commandList), commandList);


	Wait();
	result = swapChain->Present(1, 0);
	if (FAILED(result))
	{
		result = device->GetDeviceRemovedReason();
	}
	bbIndex = swapChain->GetCurrentBackBufferIndex();
	result = device->GetDeviceRemovedReason();

}


ID3D12Device*
DX12Init::GetDevice() {
	return device.Get();
}

