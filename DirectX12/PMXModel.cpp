#include "PMXModel.h"
#include <stdio.h>


PMXModel::PMXModel(const char* filename)
{
	struct PMXHeader{
		char type[4];
		float version;
		unsigned char bytesize;
		unsigned char byte[8];
	};

	FILE* modelfp = nullptr;
	fopen_s(&modelfp, filename, "rb");
	PMXHeader header;
	char type[4];
	fread(&header, sizeof(header), 1, modelfp);

	unsigned int size = 0;
	fread(&size, sizeof(unsigned int), 1, modelfp);
	
	fclose(modelfp);
	
}


PMXModel::~PMXModel()
{
}
