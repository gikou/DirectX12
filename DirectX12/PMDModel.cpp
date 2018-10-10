#include "PMDModel.h"



PMDModel::PMDModel()
{
		
}


PMDModel::~PMDModel()
{
}

void 
PMDModel::ModelLoader(const char* file) {
	
	FILE* modelfp=nullptr;
	fopen_s(&modelfp, file, "rb");
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