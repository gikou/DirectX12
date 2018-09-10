#pragma once
#include <d3d12.h>
#include<DirectXMath.h>
#include<vector>
#include <wrl/client.h>

using namespace DirectX;
#pragma pack(1)
struct PMDHeader {
	char magic[3];
	float version;
	char name[20];
	char comment[256];
	unsigned int verCount;
};

struct PMDVertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	unsigned short boneNum[2];
	unsigned char boneWeight;
	unsigned char edge;
};
#pragma pack()

using namespace Microsoft::WRL;
class Model
{
private:
	ID3D12Device* _device;
	ID3D12GraphicsCommandList* _list;

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;
	std::vector<PMDVertex> vertices;
	std::vector<unsigned short> indices;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;

	void CreateVertex();
	void CreateIndex();
public:
	Model(ID3D12Device* device, ID3D12GraphicsCommandList* list);
	~Model();

	void ModelLoader(char* filename);
	void SetVertexAndIndex();
	void ModelDraw();
};

