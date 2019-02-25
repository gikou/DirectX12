#include "PMDModel.h"
#include <iostream>
#include <array>

std::string GetCombinedPathFrom2Path(const char* modelPath, const char* texPath) {
	std::string strTexPath = texPath;
	std::string strFilelPath = modelPath;

	int pathIndex1 = strFilelPath.rfind('/');
	int pathIndex2 = strFilelPath.rfind('\\');
	int pathIndex = (pathIndex1 > pathIndex2) ? pathIndex1 : pathIndex2;
	std::string folderPath = strFilelPath.substr(0, pathIndex1);
	folderPath += "/";//�Ō�̓Z�p���[�^�������邽��(���̍s�� pathIndex+1 �ɂ��Ă���) 
	folderPath += strTexPath;
	
	return folderPath;
}


std::string GetFolderPath(const char* modelPath) {
	std::string strFilelPath = modelPath;

	int pathIndex1 = strFilelPath.rfind('/');
	int pathIndex2 = strFilelPath.rfind('\\');
	int pathIndex = (pathIndex1 > pathIndex2) ? pathIndex1 : pathIndex2;
	std::string folderPath = strFilelPath.substr(0, pathIndex1);
	folderPath += "/";//�Ō�̓Z�p���[�^�������邽��(���̍s�� pathIndex+1 �ɂ��Ă���) 
	return folderPath;
}

PMDModel::PMDModel(const char* file):folderPath(file)
{
		
}


PMDModel::~PMDModel()
{
}

void 
PMDModel::ModelLoader() {
	
	FILE* modelfp=nullptr;
	fopen_s(&modelfp, folderPath, "rb");
	PMDHeader pmdHeader;
	char type[3];
	fread(&type, sizeof(type), 1, modelfp);
	fread(&pmdHeader, sizeof(pmdHeader), 1, modelfp);
	
	vertices.resize(pmdHeader.vertexCount);
	fread(&vertices[0], sizeof(PMDVertex), pmdHeader.vertexCount, modelfp);

	unsigned int indexnum = 0;
	fread(&indexnum, sizeof(indexnum), 1, modelfp);
	
	indices.resize(indexnum);
	fread(&indices[0], indexnum * sizeof(unsigned short), 1, modelfp);

	unsigned int materialNum = 0;
	fread(&materialNum, sizeof(unsigned int), 1, modelfp);

	materials.resize(materialNum);
	fread(&materials[0], sizeof(PMDMaterial), materialNum, modelfp);

	int i = 0;
	texturePath.resize(materialNum);
	for (auto mat : materials) {

		std::string a = mat.textureFilePath;
		int size = a.find(".");
		std::string exit = a.substr(size + 1, a.size());

		if (exit == "bmp" || exit == "jpg" || exit == "png"|| exit == "tga") {
			texturePath[i].normal = GetCombinedPathFrom2Path(folderPath, mat.textureFilePath);
		}
		if (exit == "spa"|| exit == "sph") {
			texturePath[i].sphir = GetCombinedPathFrom2Path(folderPath, mat.textureFilePath);
		}
		i++;
	}
	unsigned short bonenum = 0;
	fread(&bonenum, sizeof(unsigned short), 1, modelfp);

	bones.resize(bonenum);

	fread(&bones[0], sizeof(PMDBone), bonenum, modelfp);

	unsigned short iknum = 0;
	fread(&iknum, sizeof(unsigned short), 1, modelfp);

	ikData.resize(iknum);
	for (auto& index : ikData) {
		fread(&index.boneIndex, sizeof(unsigned short), 1, modelfp);
		fread(&index.targetBoneIndex, sizeof(unsigned short), 1, modelfp);
		fread(&index.length, sizeof(unsigned char), 1, modelfp);
		fread(&index.iterationNum, sizeof(unsigned short), 1, modelfp);
		fread(&index.restriction, sizeof(float), 1, modelfp);
		index.childBoneIndex.resize(index.length);
		fread(&index.childBoneIndex[0], sizeof(unsigned short), index.length, modelfp);
	}
	//�\�� 
	unsigned short skinNum = 0;
	fread(&skinNum, sizeof(skinNum), 1, modelfp);
	//���R�̌����̂悤�ɂ�����΂��� 
	for (int i = 0; i < skinNum; ++i) {
		fseek(modelfp, 20, SEEK_CUR);
		unsigned int vertNum = 0;
		fread(&vertNum, sizeof(vertNum), 1, modelfp);
		fseek(modelfp, 1, SEEK_CUR);
		fseek(modelfp, 16 * vertNum, SEEK_CUR);
	}

	//�\���p�\�� 
	unsigned char skinDispNum = 0;
	fread(&skinDispNum, sizeof(skinDispNum), 1, modelfp);
	fseek(modelfp, skinDispNum * sizeof(unsigned short), SEEK_CUR);

	//�\���p�{�[���� 
	unsigned char boneDispNum = 0;
	fread(&boneDispNum, sizeof(boneDispNum), 1, modelfp);
	fseek(modelfp, 50 * boneDispNum, SEEK_CUR);

	//�\���{�[�����X�g 
	unsigned int dispBoneNum = 0;
	fread(&dispBoneNum, sizeof(dispBoneNum), 1, modelfp);
	fseek(modelfp, 3 * dispBoneNum, SEEK_CUR);

	//�p�� 
	//�p���Ή��t���O 
	unsigned char englishFlg = 0;
	fread(&englishFlg, sizeof(englishFlg), 1, modelfp);
	if (englishFlg) {
		//���f����20�o�C�g+256�o�C�g�R�����g 
		fseek(modelfp, 20 + 256, SEEK_CUR);
		//�{�[����20�o�C�g*�{�[���� 
		fseek(modelfp, bonenum * 20, SEEK_CUR);
		//(�\�-1)*20�o�C�g�B-1�Ȃ̂̓x�[�X�����Ԃ� 
		fseek(modelfp, (skinNum - 1) * 20, SEEK_CUR);
		//�{�[����*50�o�C�g�B 
		fseek(modelfp, boneDispNum * 50, SEEK_CUR);
	}

	fread(toonTexNames.data(), sizeof(char) * 100, toonTexNames.size(), modelfp);
	//fread(&ikIndices[0], sizeof(IKData), iknum, modelmodelfp);

	fclose(modelfp);

}

std::vector<PMDVertex>
PMDModel::GetVertices() {
	return vertices;
}

std::vector<unsigned short>
PMDModel::GetIndices() {
	return indices;
}

std::vector<PMDMaterial> 
PMDModel::GetMaterials() {
	return materials;
}

std::vector<PMDTexturePath>
PMDModel::GetTexturePath() {
	return texturePath;
}

std::array<char[100], 10> 
PMDModel::GetToonTexNames() {
	return toonTexNames;
}

std::vector<PMDBone> 
PMDModel::GetBones() {
	return bones;
}

std::vector<IKData> PMDModel::GetIKData()
{
	return ikData;
}



std::string 
PMDModel::GetModelFolder() {
	return GetFolderPath(folderPath);
}