#pragma once
#include<vector>
#include<memory>

class Dx12BufferManager;
class Dx12TextureBufffer;
class Dx12ConstantBuffer;
struct ID3D12Device;
struct ID3D12Resource;
struct ID3D12DescriptorHeap;

class DxViewSet
{
private:
	ID3D12Device * _device;
	std::shared_ptr<Dx12TextureBufffer> _texbuff;
	std::shared_ptr<Dx12ConstantBuffer> _cbuff;
public:
	DxViewSet(ID3D12Device * device);
	~DxViewSet();
	void BuildHeapAndViews(ID3D12DescriptorHeap* heap, size_t size, std::vector<ID3D12Resource*> constant, std::vector<ID3D12Resource*> texture);
};
