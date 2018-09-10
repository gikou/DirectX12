#include "Model.h"
#include<Windows.h>

#include<vector>

#include "d3dx12.h"


Model::Model(ID3D12Device* device, ID3D12GraphicsCommandList* list):_device(device), _list(list)
{
}


Model::~Model()
{
}

void
Model::ModelLoader(char* filename) {
	PMDHeader header = {};
	FILE* modelLoad = nullptr;
	fopen_s(&modelLoad, "model/初音ミク.pmd", "rb");

	fread(&header, sizeof(header), 1, modelLoad);

	const unsigned int vertex_size = sizeof(PMDVertex);

	vertices.resize(header.verCount);
	
	fread(&vertices[0], sizeof(PMDVertex), header.verCount, modelLoad);

	unsigned int indexCount = 0;
	fread(&indexCount, sizeof(indexCount), 1, modelLoad);

	indices.resize(indexCount);
	fread(&indices[0], indices.size() * sizeof(indices[0]), 1, modelLoad);
	fclose(modelLoad);
}

void 
Model::CreateVertex() {


	unsigned int vsBufferSize = vertices.size() * sizeof(PMDVertex);

	//頂点バッファの作成
	_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vsBufferSize),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexBuffer.GetAddressOf())
	);
	//バッファに対して書き込む
	char* pData = nullptr;
	vertexBuffer->Map(0, nullptr, (void**)&pData);
	memcpy(pData, &vertices[0], vsBufferSize);//頂点データをバッファにコピー
	vertexBuffer->Unmap(0, nullptr);

	//頂点バッファビューの作成
	vbView = {};
	vbView.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();//頂点アドレスのGPUにあるアドレスを記憶
	vbView.StrideInBytes = sizeof(PMDVertex);//頂点1つあたりのバイト数を指定
	vbView.SizeInBytes = vsBufferSize;//データ全体のサイズを指定


}

void 
Model::CreateIndex() {
	_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0])),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer)
	);

	D3D12_RANGE indexRange = { 0,0 };
	unsigned short* indexAdress = nullptr;
	indexBuffer->Map(0, 0, (void**)&indexAdress);
	memcpy(indexAdress, &indices[0], indices.size() * sizeof(indices[0]));//頂点データをバッファにコピー
	indexBuffer->Unmap(0, nullptr);

	_indexBufferView = {};
	_indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	_indexBufferView.SizeInBytes = indices.size() * sizeof(indices[0]);
}

void
Model::SetVertexAndIndex() {
	CreateVertex();
	CreateIndex();
}

void
Model::ModelDraw() {

	_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//頂点バッファのセット
	_list->IASetVertexBuffers(0, 1, &vbView);
	_list->IASetIndexBuffer(&_indexBufferView);

	//頂点描画
	_list->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
}