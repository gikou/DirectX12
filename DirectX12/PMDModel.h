#pragma once
#include<vector>
#include<array>
#include"AbeMath.h"




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
	XMFLOAT3 pos;//���W(12�o�C�g) 
	XMFLOAT3 normal;//�@��(12�o�C�g) 
	XMFLOAT2 uv;//UV(8�o�C�g) 
	unsigned short bornNum[2];//�{�[���ԍ�(4�o�C�g) 
	unsigned char bornWeight;//�E�F�C�g(1�o�C�g) 
	unsigned char edgeFlag;//�֊s���t���O(1�o�C�g) 
};



struct PMDMaterial {
	XMFLOAT3 diffuse;//��{�F(�g�U���ːF) 
	float alpha;//;�A���t�@�F 
	float specularity;//�X�y�L�������� 
	XMFLOAT3 specular;//�X�y�L����(���ːF) 
	XMFLOAT3 ambient;//�A���r�G���g 
	unsigned char toonIdx;//�g�D�[��̃C���f�b�N�X 
	unsigned char edgeFlg;//�֊s���t���O 
	unsigned int indices;//vertexCount�����ǃC���f�b�N�X�� 
	char textureFilePath[20];//�e�N�X�`��������Ƃ��e�N�X�`���p�X 
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
	unsigned short boneIndex; //IK�{�[��ID
	unsigned short targetBoneIndex; //�Ή�����{�[��ID
	unsigned char length; //�q�m�[�h�̐�
	unsigned short iterationNum; //�����
	float restriction;	//�����p�x
	std::vector<unsigned short> childBoneIndex;	//�q�m�[�h��ID
};
#pragma pack()



struct PMDTexturePath {
	std::string normal;
	std::string sphir;
};

class PMDModel
{
private:
	const char* folderPath;
	std::vector<PMDVertex> vertices;
	std::vector<unsigned short> indices;
	std::vector<PMDMaterial> materials;
	std::vector<PMDTexturePath> texturePath;
	std::array<char[100], 10> toonTexNames;
	std::vector<PMDBone> bones;
	std::vector<IKData> ikData;
public:
	PMDModel(const char* file);
	~PMDModel();
	void ModelLoader();
	std::vector<PMDVertex> GetVertices();
	std::vector<unsigned short> GetIndices(); 
	std::vector<PMDMaterial> GetMaterials();
	std::vector<PMDTexturePath> GetTexturePath();
	std::array<char[100], 10> GetToonTexNames();
	std::vector<PMDBone> GetBones();
	std::vector<IKData> GetIKData();
	std::string GetModelFolder();
};



