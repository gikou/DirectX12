#pragma once
#include<DirectXMath.h>
#include<vector>

#pragma pack(1)
struct PMXHeader {
	char type[4];
	float version;
	unsigned char bytesize;
	unsigned char encode;
	unsigned char uvnum;
	unsigned char vertexsize;
	unsigned char texsize;
	unsigned char matsize;
	unsigned char bonesize;
	unsigned char morphsize;
	unsigned char Rigidsize;
};

struct PMXVertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	unsigned char type;
	unsigned short bone[4];
	unsigned int weight[4];
	DirectX::XMFLOAT3 vec[3];
	unsigned int edge;
};


struct BDEF1 {
	short bone1;
};
struct BDEF2 {
	short bone1;
	short bone2;
	float whigt;
};
struct BDEF3 {
	short bone1;
	short bone2;
	short bone3;
	short bone4;
	float whigt1;
	float whigt2;
	float whigt3;
	float whigt4;
};
struct SDEF {
	BDEF2 bdef2;
	DirectX::XMFLOAT3 vec1;
	DirectX::XMFLOAT3 vec2;
	DirectX::XMFLOAT3 vec3;
};
#pragma pack(0)

class PMXModel
{
private:
	std::vector<PMXVertex> verteices;
	std::vector<unsigned short> indices;
	std::vector<std::wstring> texture;
public:
	PMXModel(const char* filename);
	~PMXModel();

	std::vector<PMXVertex> GetVertices();
	std::vector<unsigned short> GetIndices();
};

