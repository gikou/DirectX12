#include"RootSignature.h"
#include"../../d3dx12.h"

RootSignature::RootSignature()
{
}


RootSignature::~RootSignature()
{
}


D3D12_STATIC_SAMPLER_DESC SamplerDesc() {
	//サンプラの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
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
	return samplerDesc;
}

void
RootSignature::SetTableRange(int paramNum, D3D12_DESCRIPTOR_RANGE_TYPE type, int baseNum) {
	D3D12_DESCRIPTOR_RANGE descTblRange;

	descTblRange.RangeType = type;
	descTblRange.NumDescriptors = 1;
	descTblRange.BaseShaderRegister = baseNum;
	descTblRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ranges[paramNum].push_back(descTblRange);
}

void 
RootSignature::CreateRootSignature(ID3D12Device* dev) {
	
	int parameterSize = ranges.size();

	if (parameterSize <= 0) {
		return;
	}
	int a = ranges[0].size();
	parameter.resize(parameterSize);
	int i = 0;
	for (auto & param : parameter) {
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		param.DescriptorTable.NumDescriptorRanges = ranges[i].size(); //レンジの数
		param.DescriptorTable.pDescriptorRanges = ranges[i].data();
		i++;
	}

	//ルートシグネチャ
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = parameterSize;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pParameters = parameter.data();
	rsDesc.pStaticSamplers = &SamplerDesc();
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	HRESULT result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);


	result = dev->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.GetAddressOf())
	);
}
std::vector<D3D12_ROOT_PARAMETER>&
RootSignature::GetParameter() {
	return parameter;
}

ID3D12RootSignature*
RootSignature::GetRootSignature() {
	return rootSignature.Get();
}
