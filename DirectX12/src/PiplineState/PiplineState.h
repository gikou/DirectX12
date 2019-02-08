#pragma once
#include<string>
#include <wrl/client.h>
#include <vector>
#include <D3Dcompiler.h>//�V�F�[�_�R���p�C���ŕK�v
using namespace Microsoft::WRL;

class ID3D12Device;
class ID3D12RootSignature;
class ID3D12PipelineState;
struct D3D12_INPUT_ELEMENT_DESC;

class PiplineState
{
private:
	ComPtr<ID3D12Device> device;
	ComPtr<ID3DBlob> vertexShader;//���_�V�F�[�_�[
	ComPtr<ID3DBlob> pixelShader;//�s�N�Z���V�F�[�_�[
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

