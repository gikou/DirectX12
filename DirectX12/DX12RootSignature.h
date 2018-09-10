#pragma once
#include<d3d12.h>
#include<vector>
#include <wrl/client.h>

using namespace Microsoft::WRL;

class DX12RootSignature
{
private:
	ID3D12Device* _device;
	int rootCnt;
	ComPtr<ID3D12RootSignature> rootSignature;
	D3D12_STATIC_SAMPLER_DESC samplerDesc;
	std::vector<D3D12_DESCRIPTOR_RANGE> dRange;
	std::vector<D3D12_ROOT_PARAMETER> parameter;
public:
	DX12RootSignature(ID3D12Device* device);
	~DX12RootSignature();
	void SetRootSignature(int registerNum, D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY visibility);
	void CreateRootSignature();
	ID3D12RootSignature* GetRootSignature();
};

