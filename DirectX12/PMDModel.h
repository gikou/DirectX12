#pragma once
#include<vector>
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
	XMFLOAT3 mirror;//�A���r�G���g 
	unsigned char toonIdx;//�g�D�[��̃C���f�b�N�X 
	unsigned char edgeFlg;//�֊s���t���O 
	unsigned int vertexCount;//vertexCount�����ǃC���f�b�N�X�� 
	char textureFilePath[20];//�e�N�X�`��������Ƃ��e�N�X�`���p�X 
};
#pragma pack()
class PMDModel
{
private:
	std::vector<PMDVertex> vertices;
	std::vector<unsigned short> indices;
	std::vector<PMDMaterial> materials;
public:
	PMDModel();
	~PMDModel();
	void ModelLoader(const char* file);
	std::vector<PMDVertex> GetVertices();
	std::vector<unsigned short> GetIndices(); 
	std::vector<PMDMaterial> GetMaterials();
};



