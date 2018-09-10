#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include <wrl/client.h>

using namespace DirectX;

struct BaseMatrixes {
	XMMATRIX world;//���[���h 
	XMMATRIX viewproj;//�r���[�v���W�F 
};

class Camera
{
private:
	ID3D12Device * _device;
	BaseMatrixes matrix;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _cbvDescHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> _constantBuffer;

public:
	Camera(ID3D12Device* device);
	~Camera();
	void CreateCamera();
	void Update();
	ID3D12DescriptorHeap* GetCBVHeapDesc();
};

