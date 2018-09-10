#include "DX12RootSignature.h"
#include<d3d12.h>




DX12RootSignature::DX12RootSignature(ID3D12Device* device):_device(device)
{
	//サンプラの設定
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc.RegisterSpace = 0;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	rootCnt = 0;
}


DX12RootSignature::~DX12RootSignature()
{
}

void 
DX12RootSignature::SetRootSignature(int registerNum, D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY visibility) {
	dRange.resize(rootCnt);
	D3D12_DESCRIPTOR_RANGE _range = {};
	_range.RangeType = type;
	_range.NumDescriptors = 1;
	_range.BaseShaderRegister = registerNum;
	_range.RegisterSpace = 0;
	_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	dRange.push_back(_range);

	parameter.resize(rootCnt);
	D3D12_ROOT_PARAMETER _param = {};
	_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	_param.ShaderVisibility = visibility;
	_param.DescriptorTable.NumDescriptorRanges = 1;
	_param.DescriptorTable.pDescriptorRanges = &_range;
	parameter.push_back(_param);

	rootCnt += 1;
}


void
DX12RootSignature::CreateRootSignature() {

	//ルートシグネチャ
	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = parameter.size();
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pParameters = parameter.data();
	rsDesc.pStaticSamplers = &samplerDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	for (int i = 0; i < parameter.size(); ++i) {
		parameter[i].DescriptorTable.pDescriptorRanges = &dRange[i];
	}

	D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);
	_device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
}

ID3D12RootSignature* 
DX12RootSignature::GetRootSignature() {
	return rootSignature.Get();
}