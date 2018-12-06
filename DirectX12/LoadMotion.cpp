#include "LoadMotion.h"
#include <iostream>
#include<minmax.h>

LoadMotion::LoadMotion(const char* filename)
{
	FILE* motionfp = nullptr;
	fopen_s(&motionfp, filename, "rb");

	VMDHeader header = {};
	fread(&header, sizeof(VMDHeader), 1, motionfp);

	unsigned int motionSize = 0;
	fread(&motionSize, sizeof(motionSize), 1, motionfp);
	std::vector<VMDMotion> motions;
	motions.resize(motionSize);
	fread(&motions[0], sizeof(VMDMotion), motionSize, motionfp);

	for (auto& f : motions) {
		DirectX::XMFLOAT2 a;
		a.x = f.interpolation[3]/127.0f;
		a.y = f.interpolation[7] / 127.0f;
		DirectX::XMFLOAT2 b;
		b.x = f.interpolation[11] / 127.0f;
		b.y = f.interpolation[15] / 127.0f;
		animations[f.boneName].emplace_back(KeyFrame(f.frameNo, f.quaternion, f.location, a, b));
		maxnum = max(f.frameNo, maxnum);
	}
	fclose(motionfp);
}


LoadMotion::~LoadMotion()
{
}


std::map<std::string, std::vector<KeyFrame>>
LoadMotion::GetAnimations(){
	return animations;
}

int 
LoadMotion::MaxNum() {
	return maxnum;
}