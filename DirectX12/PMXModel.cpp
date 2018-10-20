#include "PMXModel.h"
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>

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

	//texture.resize(texsize);
	//for (int i = 0; i < texsize; i++) {
	//	unsigned int num;
	//	fread(&num, sizeof(num), 1, modelfp);
	//	texture[i].resize(num);
	//	fread(&texture[0], sizeof(texture[0]), num, modelfp);
	//}
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