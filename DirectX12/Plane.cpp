#include "Plane.h"
#include <d3d12.h>		//DirectX12を使うのに必要
#include "d3dx12.h"
#include <dxgi1_4.h>	//DXGIを扱うのに必要(DX12ではDXGI1.4が使われてる)
#include <D3Dcompiler.h>//シェーダコンパイラで必要
#include <DirectXMath.h>//数学系の便利なのが入ってるヘッダ
#include <tchar.h>
#include <vector>
#include <memory>
#include <wrl/client.h>
#include<assert.h>
#include <shlwapi.h>

#include"DX12Init.h"
#include"Dx12BufferManager.h"
#include"PMDModel.h"
#include"PMXModel.h"
#include"LoadMotion.h"

#include"Dx12ConstantBuffer.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "shlwapi.lib")
Plane::Plane(ID3D12Device* dev,float width, float depth, float nx, float ny, float nz):device(dev)
{	
	vertices.push_back(PrimitiveVertex(-50.f, -0.2f, 50.f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	vertices.push_back(PrimitiveVertex(50.f, -0.2f, 50.f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
	vertices.push_back(PrimitiveVertex(-50.f, -0.2f, -50.f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(PrimitiveVertex(50.f, -0.2f, -50.f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f));
}


Plane::~Plane()
{
}

ID3D12Resource* 
Plane::VertexBuffer() {
	
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
void 
Plane::Draw(ID3D12GraphicsCommandList* cmdlist) {
	
	cmdlist->IASetVertexBuffers(0, 1, &vbView);
	cmdlist->DrawInstanced(4, 1, 0, 0);
}
