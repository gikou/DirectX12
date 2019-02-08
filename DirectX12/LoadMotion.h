#pragma once
#include"AbeMath.h"
#include<vector>
#include<map>

#pragma pack(1)
struct VMDHeader
{
	char header[30];
	char name[20];
};

struct VMDMotion
{
	char boneName[15];
	unsigned int frameNo;
	DirectX::XMFLOAT3 location;
	DirectX::XMFLOAT4 quaternion;
	unsigned char interpolation[64];//ï‚äÆ
};
#pragma pack(0)

struct KeyFrame {
	KeyFrame(int f, DirectX::XMFLOAT4 q, DirectX::XMFLOAT3 l, DirectX::XMFLOAT2 b1,DirectX::XMFLOAT2 b2) {
		frameNo = f;
		quaternion = q;
		location = l;
		bz1 = b1;
		bz2 = b2;
	}
	int frameNo;
	DirectX::XMFLOAT3 location;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT2 bz1;//ÉxÉWÉFåWêîá@ 
	DirectX::XMFLOAT2 bz2;//ÉxÉWÉFåWêîáA 
};

class LoadMotion
{
private:
	std::vector<VMDMotion> motions;
	std::map<std::string, std::vector<KeyFrame>> animations;
	int maxnum;
public:
	LoadMotion(const char* filename);
	~LoadMotion();

	std::map<std::string, std::vector<KeyFrame>> GetAnimations();
	int MaxNum();
};

