#pragma once
#include <map>
#include <vector>
#include <string>
#include <wrl/client.h>
#include"../../d3dx12.h"
using namespace Microsoft::WRL;

class ID3D12Device;
class ID3D12RootSignature;
class D3D12_DESCRIPTOR_RANGE;
enum D3D12_DESCRIPTOR_RANGE_TYPE;

class RootSignature
{
private:

	std::map<int, std::vector<D3D12_DESCRIPTOR_RANGE>> ranges;
	ComPtr<ID3D12RootSignature>  rootSignature;
	std::vector<D3D12_ROOT_PARAMETER> parameter;
public:
	RootSignature();
	~RootSignature();
	void SetTableRange(int paramNum, D3D12_DESCRIPTOR_RANGE_TYPE type, int baseNum);
	void CreateRootSignature(ID3D12Device* dev);
	std::vector<D3D12_ROOT_PARAMETER>& GetParameter();
	ID3D12RootSignature* GetRootSignature();
};

