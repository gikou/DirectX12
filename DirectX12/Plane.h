#pragma once
#include "PrimitiveMesh.h"
#include<d3d12.h>
#include<vector>
//class ID3D12Resource;
class ID3D12Device;
class ID3D12Resource;
class ID3D12GraphicsCommandList;
//struct D3D12_VERTEX_BUFFER_VIEW;

class Plane :
	public PrimitiveMesh
{
private:
	std::vector<PrimitiveVertex> vertices;
	ID3D12Device* device;
	ID3D12Resource* buffer;
	PrimitiveVertex* mapDate;
	D3D12_VERTEX_BUFFER_VIEW vbView;
public:
	Plane(ID3D12Device* dev, float width, float depth, float nx, float ny, float nz);
	~Plane();
	ID3D12Resource* VertexBuffer();
	void Draw(ID3D12GraphicsCommandList* cmdlist);
};

