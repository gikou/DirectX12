#pragma once
#include<DirectXMath.h>
using namespace DirectX;
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
	PrimitiveVertex(XMFLOAT3& p, XMFLOAT3& norm, XMFLOAT2& coord) {
		//入力変数名は、自分のメンバと重ならないようにするためだけにこんな名前にしている。 
		pos = p;
		normal = norm;
		uv = coord;
	}
	PrimitiveVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) {
		pos.x = x;
		pos.y = y;
		pos.z = z;
		normal.x = nx;
		normal.y = ny;
		normal.z = nz;
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
	virtual ID3D12Resource* VertexBuffer() = 0;
	virtual void Draw(ID3D12GraphicsCommandList* cmdlist) = 0;
};

