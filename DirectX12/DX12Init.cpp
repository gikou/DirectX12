#define _CRT_SECURE_NO_WARNINGS

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
#include<assert.h>
#include <shlwapi.h>

#include"DX12Init.h"
#include"Dx12BufferManager.h"
#include"PMDModel.h"
#include"PMXModel.h"


#include"Dx12ConstantBuffer.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib, "shlwapi.lib")
#define WIN_WIDTH	(640)	//�E�B���h�E�T�C�Y��
#define WIN_HEIGTH	(480)	//�E�B���h�E�T�C�Y��
using namespace DirectX;
HRESULT result = S_OK;
//���_�\����
struct Vertex {
	DirectX::XMFLOAT3 pos;//���W
	DirectX::XMFLOAT2 uv;//uv���W
};

ID3D12Resource* 
CreateTexBuffer(ID3D12Device* device, ID3D12Resource* TexBuffer, int w, int h, int array, int mip) {
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC whiteResourceDesc = {};
	whiteResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	whiteResourceDesc.Width = w;
	whiteResourceDesc.Height = h;
	whiteResourceDesc.DepthOrArraySize = array;
	whiteResourceDesc.MipLevels = mip;
	whiteResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	whiteResourceDesc.SampleDesc.Count = 1;
	whiteResourceDesc.SampleDesc.Quality = 0;
	whiteResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	whiteResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.Type = D3D12_HEAP_TYPE_CUSTOM;
	hprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	hprop.CreationNodeMask = 1;
	hprop.VisibleNodeMask = 1;

	result = device->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAG_NONE,
		&whiteResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&TexBuffer)
	);


	return TexBuffer;
}


DX12Init::DX12Init(HWND hwnd, ID3D12Device* device) :_hwnd(hwnd), device(device)
{
	model.reset(new PMDModel("Model/�얲/reimu.pmd"));
	//model.reset(new PMDModel("Model/�����~�N/�����~�N.pmd"));
	//model.reset(new PMDModel("Model/�����~�N���^��/�����~�Nmetal.pmd"));
	//model.reset(new PMDModel("Model/hibiki/��ߔe��v1.pmd"));
	//pmxmodel.reset(new PMXModel("model/����/Rem.pmx"));

	model->ModelLoader();
	
}


DX12Init::~DX12Init()
{
	Delete();
}

void
DX12Init::Delete() {
	
}

std::wstring StringToWstring(const std::string& name) {
	auto bsize = MultiByteToWideChar(CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		name.data(), name.length(), nullptr, 0);

	std::wstring wstr;
	wstr.resize(bsize);

	bsize = MultiByteToWideChar(CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		name.data(), name.length(), &wstr[0], wstr.length());
	assert(bsize == wstr.length());

	return wstr;
}

HRESULT
DX12Init::CreateDevice() {
	
	
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
	
	std::vector <IDXGIAdapter*> adapters;
	IDXGIAdapter* adapter = nullptr;

	for (int i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(adapter);
	}
	//���̒����� NVIDIA �̓z��T��
	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		std::wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos) {//NVIDIA�A�_�v�^������ 
			adapter = adpt;
			break;
		}
	}
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
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

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
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
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
	D3D12_DESCRIPTOR_RANGE descTblRange[6] = {};

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER parameter[2] = {};
	////"t0"�ɗ���
	//descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//descTblRange[0].NumDescriptors = 1;
	//descTblRange[0].BaseShaderRegister = 0;
	//descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//"b0"�ɗ���
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRange[0].NumDescriptors = 1;
	descTblRange[0].BaseShaderRegister = 0;
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	////D3D12_DESCRIPTOR_RANGE descTblRange[1 = {};
	//"b1"�ɗ���
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRange[1].NumDescriptors = 1;
	descTblRange[1].BaseShaderRegister = 1;//���W�X�^�[�w��
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//"t0"�ɗ��� 
	descTblRange[2].NumDescriptors = 1;//�}�e���A���� 
	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTblRange[2].BaseShaderRegister = 0;
	descTblRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//"t1"�ɗ��� 
	descTblRange[3].NumDescriptors = 1;//�}�e���A���� 
	descTblRange[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTblRange[3].BaseShaderRegister = 1;
	descTblRange[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//"t2"�ɗ��� 
	descTblRange[4].NumDescriptors = 1;//�}�e���A���� 
	descTblRange[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTblRange[4].BaseShaderRegister = 2;
	descTblRange[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//"t3"�ɗ��� 
	descTblRange[5].NumDescriptors = 1;//�}�e���A���� 
	descTblRange[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTblRange[5].BaseShaderRegister = 3;
	descTblRange[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//�f�X�N���v�^�[�e�[�u���̐ݒ�
	parameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter[0].DescriptorTable.NumDescriptorRanges = 1; //�����W�̐�
	parameter[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];

	parameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter[1].DescriptorTable.NumDescriptorRanges = 5; //�����W�̐�
	parameter[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];

	//���[�g�V�O�l�`��
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 2;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pParameters = parameter;
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
		IID_PPV_ARGS(rootSignature.GetAddressOf())
	);
	return result;
}

HRESULT
DX12Init::CreateVertex() {
	auto vertices = model->GetVertices();
	//auto vertices = pmxmodel->GetVertices();

	size_t size = vertices.size() * sizeof(vertices[0]);

	//���_�o�b�t�@�̍쐬
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPU����GPU�֓]������p
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),//�T�C�Y
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexBuffer.GetAddressOf())
	);
	if (FAILED(result)) {
		return result;
	}
	//�o�b�t�@�ɑ΂��ď�������
	result = vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	memcpy(pData, &vertices[0], size);//���_�f�[�^���o�b�t�@�ɃR�s�[
	vertexBuffer->Unmap(0, nullptr);
	if (FAILED(result)) {
		return result;
	}

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();//���_�A�h���X��GPU�ɂ���A�h���X���L��
	vbView.StrideInBytes = sizeof(vertices[0]);//���_1������̃o�C�g�����w��
	vbView.SizeInBytes = size;//�f�[�^�S�̂̃T�C�Y���w��

	return result;
}

HRESULT
DX12Init::CreateIndeis() {
	
	std::vector<unsigned short> indices = model->GetIndices();
	//std::vector<unsigned short> indices = pmxmodel->GetIndices();
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
	result = _indexBuffer->Map(0, 0, reinterpret_cast<void**>(&indexData));
	memcpy(indexData, &indices[0], indices.size() * sizeof(indices[0]));//���_�f�[�^���o�b�t�@�ɃR�s�[
	_indexBuffer->Unmap(0, nullptr);
	if (FAILED(result)) {
		return result;
	}
	//���_�o�b�t�@�r���[�̍쐬
	indexView = {};
	indexView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();//���_�A�h���X��GPU�ɂ���A�h���X���L��
	indexView.Format = DXGI_FORMAT_R16_UINT;
	indexView.SizeInBytes = indices.size() * sizeof(indices[0]);//�f�[�^�S�̂̃T�C�Y���w��

	return result;
}

HRESULT
DX12Init::CretaeTexture() {

	//TexMetadata metadata = {};
	//ScratchImage image;
	////auto result = DirectX::LoadFromWICFile(L"image/aoba.bmp", 0, &metadata, image);
	//result = DirectX::LoadFromWICFile(L"image/eye2.bmp", 0, &metadata, image);

	////result = DirectX::LoadFromWICFile(L"image/eye2.bmp", 0, &metadata, image);

	////�e�N�X�`�����\�[�X�̍쐬
	//CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	//D3D12_RESOURCE_DESC texResourceDesc = {};
	//texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//texResourceDesc.Width = metadata.width;
	//texResourceDesc.Height = metadata.height;
	//texResourceDesc.DepthOrArraySize = metadata.arraySize;
	//texResourceDesc.MipLevels = metadata.mipLevels;
	//texResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//texResourceDesc.SampleDesc.Count = 1;
	//texResourceDesc.SampleDesc.Quality = 0;
	//texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	//texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	//D3D12_HEAP_PROPERTIES hprop = {};
	//hprop.Type = D3D12_HEAP_TYPE_CUSTOM;
	//hprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//hprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//hprop.CreationNodeMask = 1;
	//hprop.VisibleNodeMask = 1;

	//result = device->CreateCommittedResource(
	//	&hprop,
	//	D3D12_HEAP_FLAG_NONE,
	//	&texResourceDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(textureBuffer.GetAddressOf())
	//);
	//if (FAILED(result)) {
	//	return result;
	//}
	//
	///*_commandAllocator->Reset();
	//_commandList->Reset(_commandAllocator, nullptr);*/
	////�e�N�X�`���̓ǂݍ���
	//unsigned char *data = image.GetPixels();
	//result = textureBuffer->WriteToSubresource(
	//	0,
	//	nullptr,
	//	image.GetPixels(),
	//	metadata.width * 4,
	//	image.GetPixelsSize());

	//image.Release();


	//D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	//heapDesc.NumDescriptors = 1;
	//heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	//result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(registerDescHeap.GetAddressOf()));
	//if (FAILED(result)) {
	//	return result;
	//}
	//D3D12_SHADER_RESOURCE_VIEW_DESC sDesc = {};
	//sDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//sDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//sDesc.Texture2D.MipLevels = 1;
	//sDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//device->CreateShaderResourceView(textureBuffer.Get(), &sDesc, registerDescHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

HRESULT 
DX12Init::CreateModelTextures() {
	auto& texPath = model->GetTexturePath();
	modelTextureBuffer.resize(texPath.size());
	int i = 0;
	for (auto& tex : texPath) {
		if (tex != "") {
			TexMetadata metadata = {};
			ScratchImage image;


			auto a = tex.find(".");
			std::string ext = tex.substr(a+1,tex.size());

			if ((ext == "bmp") || (ext == "jpg") || (ext == "png")) {
				auto texture = StringToWstring(tex.c_str());
				result = DirectX::LoadFromWICFile(texture.c_str(), 0, &metadata, image);
			}
			else if (ext == "tga") {
				auto texture = StringToWstring(tex.c_str());
				result = DirectX::LoadFromTGAFile(texture.c_str(),&metadata, image);
			}
			else if (ext == "sph") {
				auto texture = StringToWstring(tex.c_str());
				result = DirectX::LoadFromWICFile(texture.c_str(),0,&metadata, image);
			}
			else if (ext == "spa") {
				auto texture = StringToWstring(tex.c_str());
				result = DirectX::LoadFromWICFile(texture.c_str(), 0, &metadata, image);
			}
			modelTextureBuffer[i] = CreateTexBuffer(device.Get(), modelTextureBuffer[i].Get(), metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);

			/*_commandAllocator->Reset();
			_commandList->Reset(_commandAllocator, nullptr);*/
			//�e�N�X�`���̓ǂݍ���
			result = modelTextureBuffer[i]->WriteToSubresource(
				0,
				nullptr,
				image.GetPixels(),
				metadata.width * 4,
				image.GetPixelsSize());

			image.Release();
			
		}
		i++;
	}
	return S_OK;
	
}

std::string 
DX12Init::GetToon(int index) {
	auto& tpaths = model->GetToonTexNames();
	std::string retPath = "toon/";
	retPath += tpaths[index];
	if (PathFileExistsA(retPath.c_str())) {
		return retPath;
	}
	else {
		return model->GetModelFolder() + tpaths[index];
	}

}

HRESULT 
DX12Init::CretaeToonTexture() {
	auto material = model->GetMaterials();
	int i = 0;
	toonTextureBuffer.resize(material.size());
	for (auto mat : material) {
		TexMetadata metadata = {};
		ScratchImage image = {};

		if (mat.toonIdx == 0xff)continue;
		auto toonPaht = GetToon(mat.toonIdx);

		auto texture = StringToWstring(toonPaht);
		result = DirectX::LoadFromWICFile(texture.c_str(), 0, &metadata, image);

		toonTextureBuffer[i] = CreateTexBuffer(device.Get(), toonTextureBuffer[i].Get(), metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);

		result = toonTextureBuffer[i]->WriteToSubresource(
			0,
			nullptr,
			image.GetPixels(),
			metadata.width * 4,
			image.GetPixelsSize());

		image.Release();
		i++;
	}
	return S_OK;
}

void
DX12Init::CreateBlackTexBuffer(){


	blackTexBuffer =  CreateTexBuffer(device.Get(), blackTexBuffer.Get(), 4, 4, 1, 1);
	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0x00);
	auto result = blackTexBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);
}

void
DX12Init::CreateWhiteTexBuffer() {
	whiteTexBuffer = CreateTexBuffer(device.Get(), whiteTexBuffer.Get(), 4, 4, 1, 1);

	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);

	auto result = whiteTexBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);
}

HRESULT
DX12Init::CreateShader() {
	ComPtr<ID3DBlob> vertexShader;//���_�V�F�[�_�[
	ComPtr<ID3DBlob> pixelShader;//�s�N�Z���V�F�[�_�[

	ID3DBlob* error = nullptr;

	//���_���C�A�E�g�̒�`
	D3D12_INPUT_ELEMENT_DESC input[] = {
		//���_
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 }
	};

	//�V�F�[�_�[�̃R���p�C�����s��
	result = D3DCompileFromFile(_T("Shader.hlsl"), nullptr, nullptr, "BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader, &error);
	if (FAILED(result)) {
		std::string strErr;
		strErr.resize(error->GetBufferSize());
		memcpy(&strErr[0], &error[0], error->GetBufferSize());
		::OutputDebugStringA(strErr.c_str());
		return result;
	}
	result = D3DCompileFromFile(_T("Shader.hlsl"), nullptr, nullptr, "BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader, &error);
	if (FAILED(result)) {
		std::string strErr;
		strErr.resize(error->GetBufferSize());
		memcpy(&strErr[0], error->GetBufferPointer(), error->GetBufferSize());
		::OutputDebugStringA(strErr.c_str());
		return result;
	}

	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g(PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.DepthStencilState.DepthEnable = true;
	gpsDesc.DepthStencilState.StencilEnable = false;
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	gpsDesc.InputLayout.NumElements = _countof(input);
	gpsDesc.InputLayout.pInputElementDescs = input;
	gpsDesc.pRootSignature = rootSignature.Get();
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.SampleMask = UINT_MAX;

	result = device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
	return result;
}

HRESULT
DX12Init::CreateConstantBuffer() {
	BaseMatrixes matrix = {};
	auto world = XMMatrixRotationY(0.2);
	eye = XMFLOAT3(0, 10, -15);
	auto target = XMFLOAT3(0, 10, 0);
	auto up = XMFLOAT3(0, 1, 0);
	camera = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(640) / static_cast<float>(480), 0.1f, 300.0f);
	
	matrix.world = world;
	matrix.viewproj = camera * projection;
	matrix.eye = { eye.x, eye.y, eye.z, 1.0f};
	
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
	/*if (FAILED(result)) {
		return result;
	}*/
	//���t�@�N�^�����O�Œ�
	/*buffer.reset(new Dx12BufferManager(device));
	buffer->CreateDescriptorHeap();*/

	D3D12_RANGE range = { 0,0 };
	
	result = _constantBuffer->Map(0, &range, (void**)&matrixAddress);
	*matrixAddress = matrix;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(registerDescHeap.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = size;
	auto handle = registerDescHeap->GetCPUDescriptorHandleForHeapStart();

	device->CreateConstantBufferView(&cbvDesc, handle);

	return result;
}

HRESULT 
DX12Init::CreateMaterialBuffer() {

	//auto material = model->GetMaterials();
	std::vector<Material> mats = {};
	mats.resize(model->GetMaterials().size());
	for (int i = 0; i < model->GetMaterials().size(); ++i) {
		mats[i].diffuse.x = model->GetMaterials()[i].diffuse.x;
		mats[i].diffuse.y = model->GetMaterials()[i].diffuse.y;
		mats[i].diffuse.z = model->GetMaterials()[i].diffuse.z;
		mats[i].diffuse.w = model->GetMaterials()[i].alpha;

		mats[i].specular.x = model->GetMaterials()[i].specular.x;
		mats[i].specular.y = model->GetMaterials()[i].specular.y;
		mats[i].specular.z = model->GetMaterials()[i].specular.z;
		mats[i].specular.w = model->GetMaterials()[i].specularity;

		mats[i].ambient.x = model->GetMaterials()[i].mirror.x;
		mats[i].ambient.y = model->GetMaterials()[i].mirror.y;
		mats[i].ambient.z = model->GetMaterials()[i].mirror.z;
	} 

	size_t size = sizeof(Material);
	size = (size + 0xff)&~0xff;
	_materialsBuffer.resize(model->GetMaterials().size());
	int midx = 0;
	for (auto& mbuff : _materialsBuffer) {
		auto result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mbuff));

		Material* material = nullptr;
		result = mbuff->Map(0, nullptr, (void**)&material);
		*material = mats[midx];
		mbuff->Unmap(0, nullptr);
		++midx;
	}
	

	D3D12_DESCRIPTOR_HEAP_DESC materialHeapDesc = {};
	materialHeapDesc.NumDescriptors =model->GetMaterials().size()*5;
	materialHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩���܂��悤�� 
	materialHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�R���X�^���g�o�b�t�@�ł� 
	result = device->CreateDescriptorHeap(&materialHeapDesc, IID_PPV_ARGS(materialDescHeap.GetAddressOf()));//������ 
	
	D3D12_SHADER_RESOURCE_VIEW_DESC textureDesc = {};
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	textureDesc.Texture2D.MipLevels = 1;
	textureDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	auto handle = materialDescHeap->GetCPUDescriptorHandleForHeapStart();
	auto& texPath = model->GetTexturePath();
	for (int i = 0; i < model->GetMaterials().size(); ++i) {
		//�萔
		D3D12_CONSTANT_BUFFER_VIEW_DESC materialDesc = {};
		materialDesc.BufferLocation = _materialsBuffer[i]->GetGPUVirtualAddress();
		materialDesc.SizeInBytes = size;
		device->CreateConstantBufferView(&materialDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//�e�N�X�`��
		auto a = texPath[i].find(".");
		std::string ext = texPath[i].substr(a + 1, texPath[i].size());
		auto texbuffer = whiteTexBuffer;
		if ((ext == "bmp") || (ext == "jpg") || (ext == "png")) {
			texbuffer = modelTextureBuffer[i];
		}
		device->CreateShaderResourceView(texbuffer.Get(), &textureDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//spa
		a = texPath[i].find(".");
		ext = texPath[i].substr(a + 1, texPath[i].size());
		texbuffer = blackTexBuffer;
		if (ext == "spa") {
			texbuffer = modelTextureBuffer[i];
		}
		device->CreateShaderResourceView(texbuffer.Get(), &textureDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//sph
		a = texPath[i].find(".");
		ext = texPath[i].substr(a + 1, texPath[i].size());
		texbuffer = whiteTexBuffer;
		if (ext == "sph") {
			texbuffer = modelTextureBuffer[i];
		}
		device->CreateShaderResourceView(texbuffer.Get(), &textureDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//toon
		
		texbuffer = toonTextureBuffer[i];
		device->CreateShaderResourceView(texbuffer.Get(), &textureDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
	}

	return S_OK;
}

HRESULT 
DX12Init::CreateDepth() {
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = 640;//��ʂɑ΂��Ďg���o�b�t�@�Ȃ̂ŉ�ʕ� 
	depthResDesc.Height = 480;//��ʂɑ΂��Ďg���o�b�t�@�Ȃ̂ŉ�ʍ��� 
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;//�K�{(�厖)�f�v�X�ł��������� 
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//�K�{(�厖) 

	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;//�f�t�H���g�ł悢 
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;//�ʂɒm��Ȃ��Ă�OK 
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;//�ʂɒm��Ȃ��Ă�OK 

																   //���̃N���A�o�����[���d�v�ȈӖ������̂ō���͍���Ă��� 
	D3D12_CLEAR_VALUE _depthClearValue = {};
	_depthClearValue.DepthStencil.Depth = 1.0f;//�[���ő�l�͂P 
	_depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	result = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //�f�v�X�������݂Ɏg���܂� 
		&_depthClearValue,
		IID_PPV_ARGS(&_depthBuffer));

	D3D12_DESCRIPTOR_HEAP_DESC _dsvHeapDesc = {};//�Ԃ����Ⴏ���ɐݒ�̕K�v�͂Ȃ����ۂ� 
	_dsvHeapDesc.NumDescriptors = 1;
	_dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	_dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = device->CreateDescriptorHeap(&_dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(_depthBuffer.Get(), &dsvDesc, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
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
	if (FAILED(CreateDepth())) {
		MessageBox(nullptr, L"Error", L"�[�x�ݒ�쐬�Ɏ��s���܂���", MB_OK | MB_ICONEXCLAMATION);
	}

	
	CreateModelTextures();
	CreateWhiteTexBuffer();
	CreateBlackTexBuffer();
	CretaeToonTexture();
	CreateMaterialBuffer();

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
	static const float color[] = {0.5f, 0.5f, 0.5f, 1.0f };

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


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), bbindex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	_commandList->OMSetRenderTargets(1, &rtvHandle, false, &_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	_commandList->ClearDepthStencilView(_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0f, 0, nullptr);
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
	_commandList->SetDescriptorHeaps(1, registerDescHeap.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(0, registerDescHeap->GetGPUDescriptorHandleForHeapStart());
	//buffer->Update(_commandList);

	static XMFLOAT3 rotation(0.0f, 0.0f,0.0f);
	unsigned char keystate[256];
	if (GetKeyboardState(keystate)) {
		if (keystate[VK_UP] & 0x80) {
			rotation.x += 0.01f;
		}
		if (keystate[VK_LEFT] & 0x80) {
			rotation.y += 0.01f;
		}
		if (keystate[VK_RIGHT] & 0x80) {
			rotation.y -= 0.01f;
		}
		if (keystate[VK_DOWN] & 0x80) {
			rotation.x -= 0.01f;
		}
		if (keystate['W'] & 0x80) {
			eye.y += 0.1f;
		}
		if (keystate['A'] & 0x80) {
			eye.x -= 0.1f;
		}
		if (keystate['S'] & 0x80) {
			eye.y -= 0.1f;
		}
		if (keystate['D'] & 0x80) {
			eye.x += 0.1f;
		}
		if (keystate[VK_SHIFT] & 0x80) {
			eye.z -= 0.1f;
		}
		if (keystate[VK_CONTROL] & 0x80) {
			eye.z += 0.1f;
		}
	}
	
	XMMATRIX world = XMMatrixRotationY(rotation.y)*XMMatrixRotationX(rotation.x);

	auto target = XMFLOAT3(0, 10, 0);
	auto up = XMFLOAT3(0, 1, 0);

	camera = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matrixAddress->world = world;
	matrixAddress->viewproj = camera * projection;
	matrixAddress->eye = { eye.x,eye.y,eye.z,1.0f };

	//�O�p�|���S���`��ɂ���
	_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//���_�o�b�t�@�̃Z�b�g
	_commandList->IASetVertexBuffers(0, 1, &vbView);
	_commandList->IASetIndexBuffer(&indexView);

	//_commandList->DrawIndexedInstanced(indexView.SizeInBytes, 1, 0, 0, 0);

	//���_�`��
	unsigned int offset = 0;
	auto handle = materialDescHeap->GetGPUDescriptorHandleForHeapStart();
	const auto increment_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_commandList->SetDescriptorHeaps(1, materialDescHeap.GetAddressOf());
	for (auto& mat : model->GetMaterials()) {
		_commandList->SetGraphicsRootDescriptorTable(1,handle);
		handle.ptr += increment_size * 5;
		_commandList->DrawIndexedInstanced(mat.vertexCount, 1, offset, 0, 0);
		offset += mat.vertexCount;
	}



	/*for (auto& mat : model->GetMaterials()) {
		_commandList->SetGraphicsRootDescriptorTable(1, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}*/

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

