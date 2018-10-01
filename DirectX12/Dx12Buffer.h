#pragma once
#include <d3d12.h>		//DirectX12‚ðŽg‚¤‚Ì‚É•K—v
#include <wrl/client.h>
using namespace Microsoft::WRL;
class Dx12BufferManager;
//struct ID3D12Resource;

class Dx12Buffer
{
	friend Dx12BufferManager;

protected:
	ComPtr<ID3D12Resource> _resource;
	int regsterNum;
public:
	ComPtr<ID3D12Resource> GetResource()const;
	Dx12Buffer();
	virtual ~Dx12Buffer() = 0;
};

