#pragma once
#include "PrimitiveMesh.h"
#include<d3d12.h>
#include<vector>
class ID3D12Device;
class ID3D12Resource;
class ID3D12GraphicsCommandList;

class Cube :
	public PrimitiveMesh
{

public:

	Cube(XMFLOAT3 center, XMFLOAT3 size, XMFLOAT3 color);
	~Cube();
	ID3D12Resource* VertexBuffer(ID3D12Device* dev);
	void SetTexture(std::string texturePath);
	void SetPosition(DirectX::XMFLOAT3 positon);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdlist);
private:
	std::vector<PrimitiveVertex> vertices;
	ID3D12Device* device;
	ID3D12Resource* buffer;
	PrimitiveVertex* mapDate;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	std::vector<XMFLOAT3> mOriginalPos;
};

