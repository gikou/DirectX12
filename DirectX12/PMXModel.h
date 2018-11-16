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

struct PMXMaterial {
	std::wstring name;
	std::wstring engName;
	DirectX::XMFLOAT4 diffuse;
	//float alpha;
	DirectX::XMFLOAT3 specular;
	float specularity;
	DirectX::XMFLOAT3 ambient;
	unsigned char edgeSize;
	DirectX::XMFLOAT4 edgeColor;
	unsigned char normalTexIndex;
	unsigned char sphirTexIndex;
	unsigned char sphirMode;
	unsigned char toonFlag;
	unsigned char toonIndex;
	std::wstring comment;
	unsigned int indices;
};

struct TexturePath {
	std::string normal;
	std::string sphir;
};



#pragma pack(0)

class PMXModel
{
private:
	std::vector<PMXVertex> verteices;
	std::vector<unsigned short> indices;
	std::vector<std::wstring> texture;
	std::vector<PMXMaterial> materials;
	std::vector<TexturePath> texturePath;
	std::vector<std::string> toonPath;
public:
	PMXModel(const char* filename);
	~PMXModel();

	std::vector<PMXVertex> GetVertices();
	std::vector<unsigned short> GetIndices();
	std::vector<PMXMaterial> GetMaterials();
	std::vector<TexturePath> GetTexturePath();
	std::vector<std::string> GetToonPath();
};

