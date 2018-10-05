#pragma once
#include<vector>
struct ID3D12Device;
struct ID3D12Resource;
class Dx12TextureBufffer
{
private:
	ID3D12Device * _device;
	std::vector<ID3D12Resource*> textureBuffers;
	Dx12TextureBufffer(const Dx12TextureBufffer&);
public:
	Dx12TextureBufffer(ID3D12Device* device);
	~Dx12TextureBufffer();
	void CreateTexture();
	std::vector<ID3D12Resource*> GetTextureBuffers();
};

