#include "PMXModel.h"
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include<assert.h>

std::string GetCombinedPathFromPath(const char* modelPath, int index) {
	//std::string strTexPath = texPath;
	std::string strFilelPath = modelPath;

	int pathIndex1 = strFilelPath.rfind('/');
	int pathIndex2 = strFilelPath.rfind('\\');
	int pathIndex = (pathIndex1 > pathIndex2) ? pathIndex1 : pathIndex2;
	std::string folderPath = strFilelPath.substr(0, pathIndex1);
	folderPath += "/";//最後はセパレータが消えるため(↑の行を pathIndex+1 にしても可) 
	//folderPath += strTexPath;

	return folderPath;
}


std::string WstringToString(const std::wstring& name) {
	auto bsize = WideCharToMultiByte(CP_ACP,
		0,
		name.data(), -1, nullptr,0,nullptr, nullptr);

	std::string str;
	str.resize(bsize);

	bsize = WideCharToMultiByte(CP_ACP,
		0,
		name.data(), -1,&str[0], str.length(), nullptr, nullptr);

	//assert(bsize-1 == str.length());
	str = str.substr(0, str.length()-1);
	return str;
}

PMXModel::PMXModel(const char* filename)
{
	

	FILE* modelfp = nullptr;
	fopen_s(&modelfp, filename, "rb");
	PMXHeader header;
	fread(&header, sizeof(header), 1, modelfp);

	unsigned int namesize = 0;
	fread(&namesize, sizeof(unsigned int), 1, modelfp);
	
	std::wstring name;
	name.resize(namesize);
	fread(&name[0], name.size(), 1, modelfp);


	unsigned int size = 0;
	fread(&size, sizeof(unsigned int), 1, modelfp);

	unsigned int commentsize = 0;
	fread(&commentsize, sizeof(unsigned int), 1, modelfp);

	std::wstring comment;
	comment.resize(commentsize);
	fread(&comment[0], comment.size(), 1, modelfp);


	fread(&size, sizeof(unsigned int), 1, modelfp);
	
	unsigned int vertexsize = 0;
	fread(&vertexsize, sizeof(unsigned int), 1, modelfp);

	verteices.resize(vertexsize);
	for (int i = 0; i < vertexsize; ++i) {

		fread(&verteices[i].pos, sizeof(DirectX::XMFLOAT3), 1, modelfp);
		fread(&verteices[i].normal, sizeof(DirectX::XMFLOAT3), 1, modelfp);
		fread(&verteices[i].uv, sizeof(DirectX::XMFLOAT2), 1, modelfp);
		fread(&verteices[i].type, sizeof(unsigned char), 1, modelfp);

		switch (verteices[i].type)
		{
		case 0:
			fread(&verteices[i].bone, sizeof(short), 1, modelfp);
			break;
		case 1:
			fread(&verteices[i].bone, sizeof(short), 2, modelfp);
			fread(&verteices[i].weight, sizeof(float), 1, modelfp);
			break;
		case 2:
			fread(&verteices[i].bone, sizeof(short), 4, modelfp);
			fread(&verteices[i].weight, sizeof(float), 4, modelfp);
			break;
		case 3:
			fread(&verteices[i].bone, sizeof(short), 2, modelfp);
			fread(&verteices[i].weight, sizeof(float), 1, modelfp);
			fread(&verteices[i].vec, sizeof(DirectX::XMFLOAT3), 3, modelfp);
			break;
		default:
			break;
		}

		fread(&verteices[i].edge, sizeof(unsigned int), 1, modelfp);

	}


	unsigned int indexsize = 0;
	fread(&indexsize, sizeof(unsigned int), 1, modelfp);

	indices.resize(indexsize);
	fread(&indices[0], sizeof(unsigned short)* indexsize, 1, modelfp);

	unsigned int texsize = 0;
	fread(&texsize, sizeof(unsigned int), 1, modelfp);


	std::vector<std::wstring> textures(texsize);
	for (int i = 0; i < texsize; i++) {

		char textsize[4];
		fread(&textsize[0], sizeof(char), 4, modelfp);

		textures[i].resize(textsize[0]/2);
		fread(&textures[i][0], textures[i].size(), 2, modelfp);
	}

	unsigned int materialsize = 0;
	fread(&materialsize, sizeof(unsigned int), 1, modelfp);

	materials.resize(materialsize);
	toonTextures.resize(materialsize);
	int i = 0;
	for (auto& mat : materials) {
		char textsize[4];
		fread(&textsize[0], sizeof(char), 4, modelfp);
		mat.name.resize(textsize[0]/2);
		fread(&mat.name[0], mat.name.size(), 2, modelfp);

		fread(&textsize[0], sizeof(char), 4, modelfp);
		mat.engName.resize(textsize[0]/2);
		fread(&mat.engName, mat.engName.size(), 2, modelfp);

		fread(&mat.diffuse, sizeof(mat.diffuse), 1, modelfp);
		fread(&mat.specular, sizeof(mat.specular), 1, modelfp);
		fread(&mat.specularity, sizeof(mat.specularity), 1, modelfp);
		fread(&mat.ambient, sizeof(mat.ambient), 1, modelfp);

		char bitflag[1];
		fread(&bitflag, sizeof(char), 1, modelfp);

		fread(&mat.edgeColor, sizeof(mat.edgeColor), 1, modelfp);
		fread(&mat.edgeSize, sizeof(mat.edgeSize), 4, modelfp);
		
		fread(&mat.normalTexIndex, sizeof(mat.normalTexIndex), 1, modelfp);
		fread(&mat.sphirTexIndex, sizeof(mat.sphirTexIndex), 1, modelfp);
		fread(&mat.sphirMode, sizeof(mat.sphirMode), 1, modelfp);
		fread(&mat.toonFlag, sizeof(mat.toonFlag), 1, modelfp);
		if (mat.toonFlag) {
			fread(&mat.toonIndex, sizeof(mat.toonIndex), 1, modelfp);
		}
		else {
			fread(&mat.toonIndex, sizeof(mat.toonIndex), 1, modelfp);
		}
		fread(&textsize[0], sizeof(char), 4, modelfp);
		mat.comment.resize(textsize[0] /2);
		fread(&mat.comment[0], mat.comment.size(), 2, modelfp);
		fread(&mat.indices, sizeof(mat.indices), 1, modelfp);
		char nazo[2];
		fread(&nazo[0], sizeof(char), 2, modelfp);
		i++;
	}
	texturePath.resize(materialsize);
	for (int i = 0; i < materialsize; ++i) {
		if (materials[i].normalTexIndex != 0xff) {
			texturePath[i].normal = WstringToString(textures[materials[i].normalTexIndex]);
		}
		if (materials[i].sphirTexIndex != 0xff) {
			texturePath[i].sphir = WstringToString(textures[materials[i].sphirTexIndex]);
		}
	}
	fclose(modelfp);
	
}


PMXModel::~PMXModel()
{
}


std::vector<PMXVertex> 
PMXModel::GetVertices() {
	return verteices;
}
std::vector<unsigned short> 
PMXModel::GetIndices() {
	return indices;
}

std::vector<PMXMaterial>
PMXModel::GetMaterials() {
	return materials;
}

std::vector<TexturePath> 
PMXModel::GetTexturePath() {
	return texturePath;
}