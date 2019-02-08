#include "PiplineState.h"
#include <d3d12.h>		//DirectX12���g���̂ɕK�v
#include "d3dx12.h"
#include <D3Dcompiler.h>//�V�F�[�_�R���p�C���ŕK�v
#include <tchar.h>

PiplineState::PiplineState(ID3D12Device* dev):device(dev)
{
}


PiplineState::~PiplineState()
{
}

void PiplineState::SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC> layout)
{
	input = layout;
}

void PiplineState::CompileShader(std::wstring filepath, std::string vertexpath, std::string pixelpath)
{
	HRESULT result = S_OK;
	ID3DBlob* error = nullptr;
	//�V�F�[�_�[�̃R���p�C�����s��
	result = D3DCompileFromFile(filepath.c_str(), nullptr, nullptr, vertexpath.c_str(), "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader, &error);
	if (FAILED(result)) {
		std::string strErr;
		strErr.resize(error->GetBufferSize());
		memcpy(&strErr[0], &error[0], error->GetBufferSize());
		::OutputDebugStringA(strErr.c_str());
		return;
	}

	result = D3DCompileFromFile(filepath.c_str(), nullptr, nullptr, pixelpath.c_str(), "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader, &error);
	if (FAILED(result)) {
		std::string strErr;
		strErr.resize(error->GetBufferSize());
		memcpy(&strErr[0], &error[0], error->GetBufferSize());
		::OutputDebugStringA(strErr.c_str());
		return;
	}
}

HRESULT PiplineState::CreatePiplineState(ID3D12RootSignature * rootsignature)
{
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g(PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.BlendState.AlphaToCoverageEnable = true;
	gpsDesc.DepthStencilState.DepthEnable = true;
	gpsDesc.DepthStencilState.StencilEnable = false;
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	gpsDesc.InputLayout.NumElements = input.size();
	gpsDesc.InputLayout.pInputElementDescs = input.data();
	gpsDesc.pRootSignature = rootsignature;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	gpsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.SampleMask = UINT_MAX;

	HRESULT r = device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
	return r;
}

ID3D12PipelineState*
PiplineState::GetPiplineState()
{
	return pipelineState.Get();
}
