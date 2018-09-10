#include "Camera.h"
#include<d3d12.h>



using namespace DirectX;


#define WIN_WIDTH	(640)	//�E�B���h�E�T�C�Y��
#define WIN_HEIGTH	(480)	//�E�B���h�E�T�C�Y��

Camera::Camera(ID3D12Device* device):_device(device)
{
}


Camera::~Camera()
{
}
BaseMatrixes* matrixAddress = nullptr;
void
Camera::CreateCamera() {
	XMMATRIX world = XMMatrixIdentity();
	XMVECTOR eye = { 0,10,-30 };
	XMVECTOR target = { 0,10,0 };
	XMVECTOR upper = { 0,1,0 };


	XMMATRIX camera = XMMatrixLookAtLH(eye, target, upper);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, static_cast<float>(WIN_WIDTH) / static_cast<float>(WIN_HEIGTH), 0.1f, 1000.0f);
	matrix.world = world;
	matrix.viewproj = camera * projection;



	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩���܂��悤�� 
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�R���X�^���g�o�b�t�@�ł� 
	_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(_cbvDescHeap.GetAddressOf()));//������ 

	D3D12_HEAP_PROPERTIES cbvHeapProperties = {};
	cbvHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	cbvHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	cbvHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	cbvHeapProperties.VisibleNodeMask = 1;
	cbvHeapProperties.CreationNodeMask = 1;

	D3D12_RESOURCE_DESC cbvResDesc = {};
	cbvResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;//�P�Ȃ�1�����o�b�t�@�Ȃ̂� 
	cbvResDesc.Width = (sizeof(BaseMatrixes) + 0xff)&~0xff;//256�A���C�������g 
	cbvResDesc.Height = 1;//1�����Ȃ�łP�ł��� 
	cbvResDesc.DepthOrArraySize = 1;//�[���Ƃ��Ȃ���� 
	cbvResDesc.MipLevels = 1;//�~�b�v�Ƃ��Ȃ���� 
	cbvResDesc.SampleDesc.Count = 1;//����1�ɈӖ��Ȃ��Ǝv���񂾂��ǖ����Ǝ��s 
	cbvResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;//SWIZZLE �Ƃ����ˁ[����B 

	_device->CreateCommittedResource(
		&cbvHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_constantBuffer.GetAddressOf()));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constantBuffer.Get()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (sizeof(BaseMatrixes) + 0xff)&~0xff;//256�A���C�������g 
	_device->CreateConstantBufferView(&cbvDesc, _cbvDescHeap.Get()->GetCPUDescriptorHandleForHeapStart());

	D3D12_RANGE range = { 0,0 };
	_constantBuffer.Get()->Map(0, &range, (void**)&matrixAddress);
	*matrixAddress = matrix;
}

void
Camera::Update() {
	static float angle = 0.0f;
	matrix.world = XMMatrixRotationY(angle);
	angle += 0.01;
	*matrixAddress = matrix;
}

ID3D12DescriptorHeap* 
Camera::GetCBVHeapDesc(){
	return _cbvDescHeap.Get();
}