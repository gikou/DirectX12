#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXMath.h>
#include "Dx12BufferManager.h"
#include"DX12Init.h"
#include"Dx12TextureBufffer.h"
#include"Dx12ConstantBuffer.h"
#include"DxViewSet.h"
Dx12BufferManager::Dx12BufferManager(ID3D12Device* device):_device(device)
{	
	
	_cbuff.reset(new Dx12ConstantBuffer(_device));
	_texbuff.reset(new Dx12TextureBufffer(_device));
	view.reset(new DxViewSet(_device));
	

}

Dx12BufferManager::~Dx12BufferManager()
{
}

void 
Dx12BufferManager::CreateDescriptorHeap() {
	_texbuff->CreateTexture();
	size_t size = sizeof(DirectX::XMMATRIX);
	size = (size + 0xff)&~0xff;
	_cbuff->CreateConstant(size);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 2;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	view->BuildHeapAndViews(heap,size, _cbuff->GetConstantBuffers(), _texbuff->GetTextureBuffers());
}
void 
Dx12BufferManager::CreateTextureBuffer() {

}

void
Dx12BufferManager::Update(ID3D12GraphicsCommandList* list) {
	list->SetDescriptorHeaps(1, (ID3D12DescriptorHeap* const*)&heap);
	list->SetGraphicsRootDescriptorTable(0, heap->GetGPUDescriptorHandleForHeapStart());
}

ID3D12DescriptorHeap* 
Dx12BufferManager::GetDescriptorHeap() {
	return heap;
}