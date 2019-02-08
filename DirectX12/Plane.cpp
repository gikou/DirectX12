#include "Plane.h"
#include "d3dx12.h"

#pragma comment(lib,"d3d12.lib")

Plane::Plane(XMFLOAT3& pos, float width, float depth, XMFLOAT3& normal, XMFLOAT3& color)
{	
	vertices.push_back(PrimitiveVertex(XMFLOAT3(pos.x - (width / 2), pos.y, pos.z + (depth/2)), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(color.x,color.y,color.z), XMFLOAT2(0.0f, 0.0f)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(pos.x + (width / 2), pos.y, pos.z + (depth / 2)), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(color.x, color.y, color.z), XMFLOAT2(1.0f, 0.0f)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(pos.x - (width / 2), pos.y, pos.z - (depth / 2)), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(color.x, color.y, color.z), XMFLOAT2(0.0f, 1.0f)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(pos.x + (width / 2), pos.y, pos.z - (depth / 2)), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(color.x, color.y, color.z), XMFLOAT2(1.0f, 1.0f)));
	
}


Plane::~Plane()
{
}

ID3D12Resource* 
Plane::VertexBuffer(ID3D12Device* dev) {
	
	size_t size = sizeof(PrimitiveVertex)*vertices.size();
	size = (size + 0xff)&~0xff;
	//頂点バッファの作成
	HRESULT result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer)
	);

	result = buffer->Map(0, nullptr, reinterpret_cast<void**>(&mapDate));
	memcpy(mapDate, &vertices[0], sizeof(PrimitiveVertex)*vertices.size());//頂点データをバッファにコピー
	buffer->Unmap(0, nullptr);

	vbView.BufferLocation = buffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = size;
	vbView.StrideInBytes = sizeof(PrimitiveVertex);

	return buffer;
}
void Plane::SetTexture(std::string texturePath)
{

}
void Plane::Update()
{
}
void Plane::SetPosition(DirectX::XMFLOAT3 position)
{
	for (auto &v : vertices) {
		v.pos.x += position.x;
		v.pos.y += position.y;
		v.pos.z += position.z;
	}
	buffer->Map(0, nullptr, reinterpret_cast<void**>(&mapDate));
	memcpy(mapDate, &vertices[0], sizeof(PrimitiveVertex)*vertices.size());//頂点データをバッファにコピー
	buffer->Unmap(0, nullptr);
}
void 
Plane::Draw(ID3D12GraphicsCommandList* cmdlist) {
	
	cmdlist->IASetVertexBuffers(0, 1, &vbView);
	cmdlist->DrawInstanced(4, 1, 0, 0);
}
