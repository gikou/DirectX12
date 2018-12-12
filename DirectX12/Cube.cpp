#include "Cube.h"
#include "d3dx12.h"
#include <DirectXMath.h>//���w�n�֗̕��Ȃ̂������Ă�w�b�_

#pragma comment(lib,"d3d12.lib")


Cube::Cube(ID3D12Device* dev, float width, float depth, float nx, float ny, float nz):device(dev)
{


	//����
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, -10), XMFLOAT3(0, 0, -1), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, -10), XMFLOAT3(0, 0, -1), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 0, -10), XMFLOAT3(0, 0, -1), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 0, -10), XMFLOAT3(0, 0, -1), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));
	//��
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, -10), XMFLOAT3(-1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, 0), XMFLOAT3(-1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 0, -10), XMFLOAT3(-1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 0, 0), XMFLOAT3(-1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));
	//�E
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, -10), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 0, -10), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));
		//���
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));


		//�V��
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, -10), XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, -10), XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(20, 10, 10), XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1)));
	vertices.push_back(PrimitiveVertex(XMFLOAT3(10, 10, 10), XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1)));
	//	//�n��
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
	//���_�o�b�t�@�̍쐬
	HRESULT result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPU����GPU�֓]������p
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),//�T�C�Y
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer)
	);

	result = buffer->Map(0, nullptr, reinterpret_cast<void**>(&mapDate));
	memcpy(mapDate, &vertices[0], sizeof(PrimitiveVertex)*vertices.size());//���_�f�[�^���o�b�t�@�ɃR�s�[
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
