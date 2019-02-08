#pragma once
#include "PrimitiveMesh.h"
#include <d3d12.h>
#include<vector>
//class ID3D12Resource;
class ID3D12Device;
class ID3D12Resource;
class ID3D12GraphicsCommandList;
//struct D3D12_VERTEX_BUFFER_VIEW;

class Cylinder :
	public PrimitiveMesh
{

public:
	Cylinder(float radius, float h, unsigned short div);
	~Cylinder();
	ID3D12Resource* VertexBuffer(ID3D12Device* dev);
	void SetTexture(std::string texturePath);
	void SetPosition(DirectX::XMFLOAT3 position);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdlist);
private:
	std::vector<PrimitiveVertex> vertices;
	ID3D12Resource* buffer;
	PrimitiveVertex* mapDate;
	D3D12_VERTEX_BUFFER_VIEW vbView;
};

