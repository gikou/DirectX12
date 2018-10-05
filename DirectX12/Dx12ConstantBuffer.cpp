#include"Dx12ConstantBuffer.h"
#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXMath.h>

using namespace DirectX;

Dx12ConstantBuffer::Dx12ConstantBuffer(ID3D12Device* device):_device(device)
{

}

Dx12ConstantBuffer::~Dx12ConstantBuffer()
{
}

void
Dx12ConstantBuffer::CreateConstant(size_t size) {
	/*BaseMatrixes matrix = {};
	auto world = XMMatrixRotationY(0.2f);
	auto eye = XMFLOAT3(0, 10, 15);
	auto target = XMFLOAT3(0, 10, 0);
	auto up = XMFLOAT3(0, 1, 0);
	camera = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(640) / static_cast<float>(480), 0.1f, 300.0f);
	matrix.world = world;
	matrix.viewproj = camera * projection;



	ID3D12Resource* resource = nullptr;
	auto result = _device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));

	result = resource->Map(0, nullptr, (void**)&matrixAddress);
	*matrixAddress = matrix;

	constantBuffers.push_back(resource);*/
}

std::vector<ID3D12Resource*> 
Dx12ConstantBuffer::GetConstantBuffers() {
	return constantBuffers;
}