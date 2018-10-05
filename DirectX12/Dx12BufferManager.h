#pragma once
#include<vector>
#include<memory>
struct ID3D12Resource;
struct ID3D12GraphicsCommandList;
class Dx12ConstantBuffer;
class Dx12TextureBufffer;
class DxViewSet;
class Dx12BufferManager
{
private:
	ID3D12Device* _device;
	ID3D12DescriptorHeap* heap;
	std::shared_ptr<Dx12ConstantBuffer> _cbuff;
	std::shared_ptr<Dx12TextureBufffer> _texbuff;
	std::shared_ptr<DxViewSet> view;
	std::vector<ID3D12Resource*> constantBuffers;
	std::vector<ID3D12Resource*> textureBuffers;
	
public:
	Dx12BufferManager(ID3D12Device* device);
	~Dx12BufferManager();

	void CreateDescriptorHeap();
	void CreateTextureBuffer();
	void Update(ID3D12GraphicsCommandList* list);

	ID3D12DescriptorHeap* GetDescriptorHeap();
};