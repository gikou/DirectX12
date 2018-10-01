#pragma once
#include<vector>
#include<memory>

class Dx12BufferManager;
class Dx12TextureBuffer;
class Dx12ConstantBuffer;

class DxViewSet
{
	friend Dx12BufferManager;

private:
	std::vector<std::shared_ptr<Dx12TextureBuffer>> _textures;
	std::vector<std::shared_ptr<Dx12ConstantBuffer>> _constants;
public:
	DxViewSet();
	~DxViewSet();
	void BuildHeapAndViews();
};
