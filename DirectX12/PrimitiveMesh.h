#pragma once
#include"AbeMath.h"
#include<string>
using namespace DirectX;
class ID3D12Device;
class ID3D12GraphicsCommandList;
class ID3D12Resource;

struct PrimitiveVertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT3 color;
	XMFLOAT2 uv;

	PrimitiveVertex() {
		pos = XMFLOAT3(0, 0, 0);
		normal = XMFLOAT3(0, 0, 0);
		color = XMFLOAT3(0, 0, 0);
		uv = XMFLOAT2(0, 0);
	}
	PrimitiveVertex(XMFLOAT3& p, XMFLOAT3& norm, XMFLOAT3& col, XMFLOAT2& coord) {
		//入力変数名は、自分のメンバと重ならないようにするためだけにこんな名前にしている。 
		pos = p;
		normal = norm;
		color = col;
		uv = coord;
	}
	PrimitiveVertex(float x, float y, float z, float nx, float ny, float nz, float cx, float cy, float cz, float u, float v) {
		pos.x = x;
		pos.y = y;
		pos.z = z;
		normal.x = nx;
		normal.y = ny;
		normal.z = nz;
		color.x = cx;
		color.y = cy;
		color.z = cz;
		uv.x = u;
		uv.y = v;
	}
};

class PrimitiveMesh
{
protected:
public:
	PrimitiveMesh();
	virtual ~PrimitiveMesh();
	virtual ID3D12Resource* VertexBuffer(ID3D12Device* dev) = 0;
	virtual void SetTexture(std::string texturePath) = 0;
	virtual void SetPosition(DirectX::XMFLOAT3 position) = 0;
	virtual void Update() = 0;
	virtual void Draw(ID3D12GraphicsCommandList* cmdlist) = 0;
};

