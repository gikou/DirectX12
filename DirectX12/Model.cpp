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
	fopen_s(&modelLoad, "model/�����~�N.pmd", "rb");

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

	//���_�o�b�t�@�̍쐬
	_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPU����GPU�֓]������p
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vsBufferSize),//�T�C�Y
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexBuffer.GetAddressOf())
	);
	//�o�b�t�@�ɑ΂��ď�������
	char* pData = nullptr;
	vertexBuffer->Map(0, nullptr, (void**)&pData);
	memcpy(pData, &vertices[0], vsBufferSize);//���_�f�[�^���o�b�t�@�ɃR�s�[
	vertexBuffer->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	vbView = {};
	vbView.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();//���_�A�h���X��GPU�ɂ���A�h���X���L��
	vbView.StrideInBytes = sizeof(PMDVertex);//���_1������̃o�C�g�����w��
	vbView.SizeInBytes = vsBufferSize;//�f�[�^�S�̂̃T�C�Y���w��


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
	memcpy(indexAdress, &indices[0], indices.size() * sizeof(indices[0]));//���_�f�[�^���o�b�t�@�ɃR�s�[
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
	//���_�o�b�t�@�̃Z�b�g
	_list->IASetVertexBuffers(0, 1, &vbView);
	_list->IASetIndexBuffer(&_indexBufferView);

	//���_�`��
	_list->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
}