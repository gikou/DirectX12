#include "DxViewSet.h"
#include <d3d12.h>
#include"Dx12ConstantBuffer.h"
#include"Dx12TextureBufffer.h"

DxViewSet::DxViewSet(ID3D12Device* device):_device(device)
{
}


DxViewSet::~DxViewSet()
{
}

void 
DxViewSet::BuildHeapAndViews(ID3D12DescriptorHeap* heap, size_t size, std::vector<ID3D12Resource*> constant, std::vector<ID3D12Resource*> texture) {
	
	D3D12_SHADER_RESOURCE_VIEW_DESC sDesc = {};
	sDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	sDesc.Texture2D.MipLevels = 1;
	sDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	_device->CreateShaderResourceView(texture[0], &sDesc, heap->GetCPUDescriptorHandleForHeapStart());


	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constant[0]->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = size;
	auto handle = heap->GetCPUDescriptorHandleForHeapStart();

	handle.ptr += _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	_device->CreateConstantBufferView(&cbvDesc, handle);


}
