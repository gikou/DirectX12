#include "Cylinder.h"
#include <d3d12.h>
#include "d3dx12.h"


using namespace DirectX;

Cylinder::Cylinder(float r, float height, unsigned short div)
{
	float offset = 30.0f;

	vertices.resize(div * 2 + 2);
	for (int i = 0; i <= div; ++i) {
		vertices[i * 2].pos.x = r*cos((XM_2PI / float(div))*(float)i) + offset;
		vertices[i * 2].pos.z = r*sin((XM_2PI / float(div))*(float)i)+ offset;
		vertices[i * 2].pos.y = 0;

		XMFLOAT3 norm = vertices[i * 2].pos;
		XMStoreFloat3(&vertices[i * 2].normal, XMVector3Normalize(XMLoadFloat3(&norm)));

		vertices[i * 2].uv.x = (1.f / float(div))*float(i);
		vertices[i * 2].uv.y = 1.0f;

		vertices[i * 2].color = XMFLOAT3(0.0f, 1.0f, 0.0f);


		vertices[i * 2 + 1].pos.x = r*cos((XM_2PI / float(div))*(float)i) + offset;
		vertices[i * 2 + 1].pos.z = r*sin((XM_2PI / float(div))*(float)i)+ offset;
		vertices[i * 2 + 1].pos.y = height;

		vertices[i * 2 + 1].normal = vertices[i * 2].normal;

		vertices[i * 2 + 1].uv.x = (1.f / float(div))*float(i);
		vertices[i * 2 + 1].uv.y = 0.0f;

		vertices[i * 2+1].color = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}
	
	int a = 1;

}

Cylinder::~Cylinder()
{
}

ID3D12Resource * Cylinder::VertexBuffer(ID3D12Device* dev)
{
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
void Cylinder::SetTexture(std::string texturePath)
{
}
void Cylinder::SetPosition(DirectX::XMFLOAT3 position)
{
}
void Cylinder::Update()
{
}
void
Cylinder::Draw(ID3D12GraphicsCommandList* cmdlist) {
	
	cmdlist->IASetVertexBuffers(0, 1, &vbView);
	cmdlist->DrawInstanced(vertices.size(), 1, 0, 0);
}
