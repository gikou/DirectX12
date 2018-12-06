#include "Dx12TextureBufffer.h"
#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXMath.h>
//#include<DirectXTex.h>
//#pragma comment(lib,"DirectXTex.lib")
Dx12TextureBufffer::Dx12TextureBufffer(ID3D12Device* device) :_device(device)
{
}


Dx12TextureBufffer::~Dx12TextureBufffer()
{
}

void
Dx12TextureBufffer::CreateTexture() {
	//DirectX::TexMetadata metadata = {};
	//DirectX::ScratchImage image;

	////auto result = DirectX::LoadFromWICFile(L"image/aoba.bmp", 0, &metadata, image);
	//DirectX::LoadFromWICFile(L"image/precure.jpg", 0, &metadata, image);

	////テクスチャリソースの作成
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

	//ID3D12Resource* resource;
	//_device->CreateCommittedResource(
	//	&hprop,
	//	D3D12_HEAP_FLAG_NONE,
	//	&texResourceDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&resource)
	//);

	//unsigned char *data = image.GetPixels();
	//resource->WriteToSubresource(
	//	0,
	//	nullptr,
	//	image.GetPixels(),
	//	metadata.width * 4,
	//	image.GetPixelsSize());

	//image.Release();

	//textureBuffers.push_back(resource);
}

std::vector<ID3D12Resource*>
Dx12TextureBufffer::GetTextureBuffers() {
	return textureBuffers;
}