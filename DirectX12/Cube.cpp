#include "Cube.h"
#include "d3dx12.h"
#include <DirectXMath.h>//数学系の便利なのが入ってるヘッダ

#pragma comment(lib,"d3d12.lib")


Cube::Cube(ID3D12Device* dev, float width, float depth, float nx, float ny, float nz):device(dev)
{


	//正面
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, -10), XMFLOAT3(0, 0, -1), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, -10), XMFLOAT3(0, 0, -1), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 0, -10), XMFLOAT3(0, 0, -1), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 0, -10), XMFLOAT3(0, 0, -1), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));
	//左
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, -10), XMFLOAT3(-1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, 0), XMFLOAT3(-1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 0, -10), XMFLOAT3(-1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 0, 0), XMFLOAT3(-1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));
	//右
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, -10), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 0, -10), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));
		//後ろ
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));


		//天井
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, -10), XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, -10), XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, 10), XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, 10), XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));
	//	//地面
	//{ XMFLOAT3(-10, -10, -10),XMFLOAT2(0, 0) },
	//{ XMFLOAT3(10,  -10, -10),XMFLOAT2(1, 0) },
	//{ XMFLOAT3(-10, -10, 10),XMFLOAT2(0, 1) },
	//{ XMFLOAT3(10, -10, 10),XMFLOAT2(1, 1) },
}


Cube::~Cube()
{
}

ID3D12Resource * Cube::VertexBuffer()
{
	size_t size = sizeof(PrimitiveVertex)*vertices.size();
	size = (size + 0xff)&~0xff;
	//頂点バッファの作成
	HRESULT result = device->CreateCommittedResource(
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

void Cube::Draw(ID3D12GraphicsCommandList * cmdlist)
{
	cmdlist->IASetVertexBuffers(0, 1, &vbView);
	cmdlist->DrawInstanced(vertices.size(), 1, 0, 0);
}
