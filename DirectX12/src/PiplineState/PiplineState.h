#pragma once
#include<string>
#include <wrl/client.h>
#include <vector>
#include <D3Dcompiler.h>//シェーダコンパイラで必要
using namespace Microsoft::WRL;

class ID3D12Device;
class ID3D12RootSignature;
class ID3D12PipelineState;
struct D3D12_INPUT_ELEMENT_DESC;

class PiplineState
{
private:
	ComPtr<ID3D12Device> device;
	ComPtr<ID3DBlob> vertexShader;//頂点シェーダー
	ComPtr<ID3DBlob> pixelShader;//ピクセルシェーダー
	std::vector<D3D12_INPUT_ELEMENT_DESC> input;
	ComPtr<ID3D12PipelineState> pipelineState;
public:
	PiplineState(ID3D12Device* dev);
	~PiplineState();
	void SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC> layout);
	void CompileShader(std::wstring filepath, std::string vertexpath, std::string pixelpath);
	HRESULT CreatePiplineState(ID3D12RootSignature* rootsignature);

	ID3D12PipelineState* GetPiplineState();

};

