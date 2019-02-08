#include "PrimitiveCreator.h"
#include"DX12Init.h"
#include"PrimitiveMesh.h"
#include <tchar.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <D3Dcompiler.h>

#include"Plane.h"
#include"Cube.h"
#include"Cylinder.h"
#pragma comment(lib, "d3dcompiler.lib")


PrimitiveCreator::PrimitiveCreator(ID3D12Device* dev):device(dev)
{
	plane.reset(new Plane(device,XMFLOAT3(0.0f,0.0f,0.0f),100.0f,100.0f,XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT3(1.0f,1.0f,1.0f)));
	cube.reset(new Cube(device, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	cylinder.reset(new Cylinder(device,15.0f,20.0f,20));
}


PrimitiveCreator::~PrimitiveCreator()
{
}

void
PrimitiveCreator::Init() {
	

	HRESULT result = S_OK;
	//�T���v���̐ݒ�
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
	//�f�B�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE range[2] = {};

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER parameter[2] = {};
	//"b0"�ɗ���
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range[0].NumDescriptors = 1;
	range[0].BaseShaderRegister = 0;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//"t1"�ɗ���
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[1].NumDescriptors = 1;
	range[1].BaseShaderRegister = 0;
	range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	//�f�X�N���v�^�[�e�[�u���̐ݒ�
	parameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter[0].DescriptorTable.NumDescriptorRanges = 1; //�����W�̐�
	parameter[0].DescriptorTable.pDescriptorRanges = &range[0];

	parameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter[1].DescriptorTable.NumDescriptorRanges = 1;
	parameter[1].DescriptorTable.pDescriptorRanges = &range[1];

	//���[�g�V�O�l�`��
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = _countof(parameter);;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pParameters = parameter;
	rsDesc.pStaticSamplers = &samplerDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);
	result = device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.GetAddressOf())
	);

	plane->VertexBuffer();
	cube->VertexBuffer();
	cylinder->VertexBuffer();
	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescs[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};

	ComPtr<ID3DBlob> vertexShader;//���_�V�F�[�_�[
	ComPtr<ID3DBlob> pixelShader;//�s�N�Z���V�F�[�_�[


	result = D3DCompileFromFile(_T("PrimitiveShader.hlsl"), nullptr, nullptr, "PrimitiveVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader, nullptr);
	
	result = D3DCompileFromFile(_T("PrimitiveShader.hlsl"), nullptr, nullptr, "PrimitivePS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader, nullptr);
	
	

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.DepthStencilState.DepthEnable = true;//�[�x�o�b�t�@�̎g�p
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//DSV�K�{
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//�������ق���ʂ�
	gpsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//DSV�K�{
	gpsDesc.DepthStencilState.StencilEnable = false;
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	gpsDesc.InputLayout.NumElements = sizeof(inputLayoutDescs) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	gpsDesc.InputLayout.pInputElementDescs = inputLayoutDescs;
	gpsDesc.pRootSignature = rootSignature.Get();
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.SampleMask = 0xffffffff;

	result = device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
}

void 
PrimitiveCreator::SetPrimitiveDrawMode(ID3D12GraphicsCommandList* cmdlist) {
	
	cmdlist->SetPipelineState(_pipelineState.Get());
	cmdlist->SetGraphicsRootSignature(rootSignature.Get());
	//
	cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

//PrimitiveObject*
//PrimitiveCreator::CreatePlane(float width, float depth, float y) {
//	
//}
void
PrimitiveCreator::Draw(ID3D12GraphicsCommandList* cmdlist) {
	
	plane->Draw(cmdlist);
	//cube->Draw(cmdlist);
	cylinder->Draw(cmdlist);
}