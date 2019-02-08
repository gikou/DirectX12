#include "PrimitiveCreator.h"
#include"DX12Init.h"
#include"PrimitiveMesh.h"
#include"src\RootSignature\RootSignature.h"
#include"src\PiplineState\PiplineState.h"
#include <tchar.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <D3Dcompiler.h>

#include"Plane.h"
#include"Cube.h"
#include"Cylinder.h"
#pragma comment(lib, "d3dcompiler.lib")


PrimitiveCreator::PrimitiveCreator(ID3D12Device* dev)
	: mDevice(dev)
	, mRootSignature(std::make_shared<RootSignature>())
	, mPipelineState(std::make_shared<PiplineState>())
	//, mPlaneMesh(std::make_shared<Plane>(XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0f, 100.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)))
	//, mPlaneMesh(std::make_shared<Plane>(XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0f, 100.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)))
	//, mCubeMesh(std::make_shared<Cube>(mDevice, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f))
	//, mCylinderMesh(std::make_shared<Cylinder>(15.0f, 20.0f, 20))
{
}


PrimitiveCreator::~PrimitiveCreator()
{
}

void
PrimitiveCreator::Init() {
	
	HRESULT result = S_OK;


	mRootSignature->SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0);
	mRootSignature->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	mRootSignature->CreateRootSignature(mDevice);

	//mPlaneMesh->VertexBuffer(mDevice);
	//mCubeMesh->VertexBuffer();
	//mCylinderMesh->VertexBuffer(mDevice);

	//頂点レイアウト
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDescs = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};

	
	mPipelineState->SetInputLayout(inputLayoutDescs);
	mPipelineState->CompileShader(_T("PrimitiveShader.hlsl"), "PrimitiveVS", "PrimitivePS");
	mPipelineState->CreatePiplineState(mDevice, mRootSignature->GetRootSignature());

}

void 
PrimitiveCreator::SetPrimitiveDrawMode(ID3D12GraphicsCommandList* cmdlist) {
	
	cmdlist->SetPipelineState(mPipelineState->GetPiplineState());
	cmdlist->SetGraphicsRootSignature(mRootSignature->GetRootSignature());
	cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void
PrimitiveCreator::Draw(ID3D12GraphicsCommandList* cmdlist) {
	
	//mPlaneMesh->Draw(cmdlist);
	//cube->Draw(cmdlist);
	//mCylinderMesh->Draw(cmdlist);
}