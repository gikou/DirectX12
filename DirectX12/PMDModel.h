#pragma once
#include<vector>
#include<array>
#include<DirectXMath.h>

using namespace DirectX;
#pragma pack(1)
struct PMDHeader
{
	float version;
	char name[20];
	char comment[256];
	unsigned int vertexCount;
};

struct PMDVertex
{
	XMFLOAT3 pos;//座標(12バイト) 
	XMFLOAT3 normal;//法線(12バイト) 
	XMFLOAT2 uv;//UV(8バイト) 
	unsigned short bornNum[2];//ボーン番号(4バイト) 
	unsigned char bornWeight;//ウェイト(1バイト) 
	unsigned char edgeFlag;//輪郭線フラグ(1バイト) 
};



struct PMDMaterial {
	XMFLOAT3 diffuse;//基本色(拡散反射色) 
	float alpha;//;アルファ色 
	float specularity;//スペキュラ強さ 
	XMFLOAT3 specular;//スペキュラ(反射色) 
	XMFLOAT3 mirror;//アンビエント 
	unsigned char toonIdx;//トゥーんのインデックス 
	unsigned char edgeFlg;//輪郭線フラグ 
	unsigned int vertexCount;//vertexCountだけどインデックス数 
	char textureFilePath[20];//テクスチャがあるときテクスチャパス 
};

struct PMDBone {
	char boneName[20];
	unsigned short parentIndex;
	unsigned short childIndex;
	unsigned char type;
	unsigned short ikParent;
	XMFLOAT3 headPos;
};

struct IKData {
	unsigned short boneIndex;
	unsigned short targetBoneIndex;
	unsigned char length;
	unsigned short iterations;
	float weigth;
	unsigned short childBoneIndex[5];
};

#pragma pack()
class PMDModel
{
private:
	const char* folderPath;
	std::vector<PMDVertex> vertices;
	std::vector<unsigned short> indices;
	std::vector<PMDMaterial> materials;
	std::vector<std::string> texturePath;
	std::array<char[100], 10> toonTexNames;
public:
	PMDModel(const char* file);
	~PMDModel();
	void ModelLoader();
	std::vector<PMDVertex> GetVertices();
	std::vector<unsigned short> GetIndices(); 
	std::vector<PMDMaterial> GetMaterials();
	std::vector<std::string> GetTexturePath();
	std::array<char[100], 10> GetToonTexNames();
	std::string GetModelFolder();
};



