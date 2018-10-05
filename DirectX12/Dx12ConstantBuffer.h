#pragma once
#include<vector>
#include<DirectXMath.h>
struct ID3D12Device;
struct ID3D12Resource;

//struct BaseMatrixes {
//	DirectX::XMMATRIX world;//ワールド 
//	DirectX::XMMATRIX viewproj;//ビュープロジェ 
//};

class Dx12ConstantBuffer
{
private:
	ID3D12Device * _device;
	/*BaseMatrixes* matrixAddress;
	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX projection;*/

	std::vector<ID3D12Resource*> constantBuffers;
	Dx12ConstantBuffer(const Dx12ConstantBuffer&);
	//void operator=(const Dx12ConstantBuffer&);
public:
	Dx12ConstantBuffer(ID3D12Device* device);
	~Dx12ConstantBuffer();
	void CreateConstant(size_t size);
	std::vector<ID3D12Resource*> GetConstantBuffers();
};

