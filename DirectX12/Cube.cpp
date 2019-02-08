#include "Cube.h"
#include "d3dx12.h"

#pragma comment(lib,"d3d12.lib")


Cube::Cube(XMFLOAT3 center, XMFLOAT3 size, XMFLOAT3 color)
{


	//正面
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x/2.0f, center.y + size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, 0, -1), color, XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y + size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, 0, -1), color, XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y - size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, 0, -1), color, XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y - size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, 0, -1), color, XMFLOAT2(1, 1)));
	//左
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y + size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(-1, 0, 0), color, XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y + size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(-1, 0, 0), color, XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y - size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(-1, 0, 0), color, XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y - size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(-1, 0, 0), color, XMFLOAT2(1, 1)));
	//右
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y + size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(1, 0, 0), color, XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y + size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(1, 0, 0), color, XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y - size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(1, 0, 0), color, XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y - size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(1, 0, 0), color, XMFLOAT2(1, 1)));
		//後ろ
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y + size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(0, 0, 1), color, XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y + size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, 0, 1), color, XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y - size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(0, 0, 1), color, XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y - size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, 0, 1), color, XMFLOAT2(1, 1)));


		//天井
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y + size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, 1, 0), color, XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y + size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, 1, 0), color, XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y + size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(0, 1, 0), color, XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y + size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(0, 1, 0), color, XMFLOAT2(1, 1)));
	//床
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y - size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, -1, 0), color, XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y - size.y / 2.0f, center.z - size.z / 2.0f), XMFLOAT3(0, -1, 0), color, XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x + size.x / 2.0f, center.y - size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(0, -1, 0), color, XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(center.x - size.x / 2.0f, center.y - size.y / 2.0f, center.z + size.z / 2.0f), XMFLOAT3(0, -1, 0), color, XMFLOAT2(1, 1)));
	//	//地面
	//{ XMFLOAT3(-10, -10, -10),XMFLOAT2(0, 0) },
	//{ XMFLOAT3(10,  -10, -10),XMFLOAT2(1, 0) },
	//{ XMFLOAT3(-10, -10, 10),XMFLOAT2(0, 1) },
	//{ XMFLOAT3(10, -10, 10),XMFLOAT2(1, 1) },

	mOriginalPos.resize(vertices.size());
	int i = 0;
	for (auto &pos : mOriginalPos) {
		pos = vertices[i++].pos;
	}

}


Cube::~Cube()
{
}

ID3D12Resource * Cube::VertexBuffer(ID3D12Device* dev)
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

void Cube::SetTexture(std::string texturePath)
{
}

void Cube::SetPosition(DirectX::XMFLOAT3 positon)
{
	int i = 0;
	for (auto &vertex : vertices) {
		vertex.pos = mOriginalPos[i++] + positon;
	}
	buffer->Map(0, nullptr, reinterpret_cast<void**>(&mapDate));
	memcpy(mapDate, &vertices[0], sizeof(PrimitiveVertex)*vertices.size());//頂点データをバッファにコピー
	buffer->Unmap(0, nullptr);
}

void Cube::Update()
{
}

void Cube::Draw(ID3D12GraphicsCommandList * cmdlist)
{
	cmdlist->IASetVertexBuffers(0, 1, &vbView);
	cmdlist->DrawInstanced(vertices.size(), 1, 0, 0);
}
