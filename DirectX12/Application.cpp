#include <Windows.h>	//�E�B���h�E�\������̂ɕK�v
#include "Application.h"
#include <d3d12.h>		//DirectX12���g���̂ɕK�v
#include "d3dx12.h"
#include <dxgi1_4.h>	//DXGI�������̂ɕK�v(DX12�ł�DXGI1.4���g���Ă�)
#include <D3Dcompiler.h>//�V�F�[�_�R���p�C���ŕK�v
#include <DirectXMath.h>//���w�n�֗̕��Ȃ̂������Ă�w�b�_
#include <tchar.h>
#include <vector>
#include <memory>
#include <iostream>


#include"DX12Init.h"
#include"Model.h"
#include"DX12RootSignature.h"

#include"Camera.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;

#define WIN_WIDTH	(640)	//�E�B���h�E�T�C�Y��
#define WIN_HEIGTH	(480)	//�E�B���h�E�T�C�Y��
Application::Application()
{
}


Application::~Application()
{
}

bool
Application::Run(HWND hwnd) {

	ShowWindow(hwnd, SW_SHOW);

	HRESULT result = S_OK;
	//�G���[���o�͂ɕ\��������
#ifdef _DEBUG
	ID3D12Debug *debug = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (FAILED(result))
		int i = 0;
	debug->EnableDebugLayer();
	debug->Release();
#endif


	std::shared_ptr<DX12Init> dx12;
	dx12.reset(new DX12Init(hwnd));
	dx12->Initialize();

	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC descriptor = {};
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor.NumDescriptors = 2;
	descriptor.NodeMask = 0;
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	result = dx12->GetDevice()->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
	//�f�B�X�N���v�^�n���h���̍쐬
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart());

	//�����_�[�^�[�Q�b�g
	std::vector<ID3D12Resource*> renderTarget;
	//�����_�[�^�[�Q�b�g���Ԃ�m��
	renderTarget.resize(2);
	//�f�B�X�N���v�^1������̃T�C�Y���擾
	unsigned int descriptorSize = dx12->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (int i = 0; i < 2; ++i) {
		result = dx12->GetSwap()->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i]));//�X���b�v�`�F�C������L�����o�X���擾
		dx12->GetDevice()->CreateRenderTargetView(renderTarget[i], nullptr, descriptorHandle);//�L�����o�X�ƃr���[��R�Â�
		descriptorHandle.Offset(descriptorSize);//�L�����o�X�ƃr���[�̂Ԃ񎟂̂Ƃ���܂ŃI�t�Z�b�g
	}


	std::shared_ptr<DX12RootSignature> rootSignature;
	rootSignature.reset(new DX12RootSignature(dx12->GetDevice()));

	rootSignature->SetRootSignature(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature->SetRootSignature(0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, D3D12_SHADER_VISIBILITY_ALL);

	rootSignature->CreateRootSignature();

	std::shared_ptr<Model> model;
	model.reset(new Model(dx12->GetDevice(), dx12->GetList()));
	model->ModelLoader("model/�����~�N.pmd");


	//���_���̍쐬
	/*Vertex vertices[] = {
		{ { -0.7f,  0.7f, 0.0f },{ 0.0f, 0.0f } },
	{ { 0.7f,  0.7f, 0.0f },{ 1.0f, 0.0f } },
	{ { 0.7f, -0.7f, 0.0f },{ 1.0f, 1.0f } },

	{ { 0.7f, -0.7f, 0.0f },{ 1.0f, 1.0f } },
	{ { -0.7f, -0.7f, 0.0f },{ 0.0f, 1.0f } },
	{ { -0.7f,  0.7f, 0.0f },{ 0.0f, 0.0f } }
	};*/

	//���_���C�A�E�g�̒�`
	D3D12_INPUT_ELEMENT_DESC input[] = {
		//���_
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	/*	{"TEXCOORD",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},*/
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 }
	};
	
	
	model->SetVertexAndIndex();


	//�e�N�X�`�����\�[�X�̍쐬
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texResourceDesc = {};
	texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Width = 256;
	texResourceDesc.Height = 256;
	texResourceDesc.DepthOrArraySize = 1;
	texResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.SampleDesc.Count = 1;
	texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.Type = D3D12_HEAP_TYPE_CUSTOM;
	hprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	hprop.CreationNodeMask = 1;
	hprop.VisibleNodeMask = 1;

	ComPtr<ID3D12Resource> textureBuffer;
	result = dx12->GetDevice()->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(textureBuffer.GetAddressOf())
	);

	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = WIN_WIDTH;//��ʂɑ΂��Ďg���o�b�t�@�Ȃ̂ŉ�ʕ� 
	depthResDesc.Height = WIN_HEIGTH;//��ʂɑ΂��Ďg���o�b�t�@�Ȃ̂ŉ�ʍ��� 
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;//�K�{(�厖)�f�v�X�ł��������� 
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//�K�{(�厖) 

	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;//�f�t�H���g�ł悢 
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;//�ʂɒm��Ȃ��Ă�OK 
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;//�ʂɒm��Ȃ��Ă�OK 

																   //���̃N���A�o�����[���d�v�ȈӖ������̂ō���͍���Ă��� 
	D3D12_CLEAR_VALUE _depthClearValue = {};
	_depthClearValue.DepthStencil.Depth = 1.0f;//�[���ő�l�͂P 
	_depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	ComPtr<ID3D12Resource> _depthBuffer;
	result = dx12->GetDevice()->CreateCommittedResource(&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //�f�v�X�������݂Ɏg���܂� 
		&_depthClearValue,
		IID_PPV_ARGS(_depthBuffer.GetAddressOf()));

	D3D12_DESCRIPTOR_HEAP_DESC _dsvHeapDesc = {};//�Ԃ����Ⴏ���ɐݒ�̕K�v�͂Ȃ����ۂ� 
	ID3D12DescriptorHeap* _dsvHeap = nullptr;
	_dsvHeapDesc.NumDescriptors = 1;
	_dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	_dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = dx12->GetDevice()->CreateDescriptorHeap(&_dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap));

	D3D12_CPU_DESCRIPTOR_HANDLE depthHandle = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthHandle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	dx12->GetDevice()->CreateDepthStencilView(_depthBuffer.Get(), &dsvDesc, depthHandle);

	//�V�F�[�_���\�[�X�r���[�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	ComPtr<ID3D12DescriptorHeap> texHeap;
	result = dx12->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(texHeap.GetAddressOf()));

	unsigned int stride = dx12->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_SHADER_RESOURCE_VIEW_DESC sDesc = {};
	sDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	sDesc.Texture2D.MipLevels = 1;
	sDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	dx12->GetDevice()->CreateShaderResourceView(textureBuffer.Get(), &sDesc, texHeap.Get()->GetCPUDescriptorHandleForHeapStart());

	//�e�N�X�`���̓ǂݍ���
	BITMAPFILEHEADER bmpFileHeader = {};
	BITMAPINFOHEADER bmpInfoHeader = {};
	FILE* fp = nullptr;
	if (fopen_s(&fp, "image/aoba.bmp", "rb") != 0) {}
	//bmpFileHeader
	fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
	//bmpInfoHeader
	fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);

	unsigned long rgb[3];
	fread(&rgb[0], sizeof(rgb), 1, fp);

	std::vector<char> data;
	data.resize(bmpInfoHeader.biSizeImage);//�̈�m��
										   //bmp�f�[�^�����ׂēǂݍ���
	for (int line = bmpInfoHeader.biHeight - 1; line >= 0; --line) {//������1���C�����オ��
		for (int count = 0; count < bmpInfoHeader.biWidth * 4; count += 4) {//������E��
			unsigned int address = line * bmpInfoHeader.biWidth * 4;
			fread(&data[address + count], sizeof(unsigned char), 4, fp);
		}
	}
	fclose(fp);

	//�e�N�X�`���o�b�t�@�ւ̏�������
	D3D12_BOX box = {};
	box.left = 0;
	box.top = 0;
	box.front = 0;
	box.right = 256;
	box.bottom = 256;
	box.back = 1;
	result = textureBuffer.Get()->WriteToSubresource(
		0,
		&box,
		data.data(),
		box.right * 4,
		box.bottom * 4
	);

	//���\�[�X�o���A
	//dx12->GetList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	/*dx12->GetList()->Close();

	ID3D12CommandList* commandList[] = { dx12->GetList() };
	dx12->GetQueue()->ExecuteCommandLists(_countof(commandList), commandList);


	dx12->Wait();*/

	//�V�F�[�_�[�̓ǂݍ���
	ID3DBlob* error = nullptr;
	ID3DBlob* vertexShader = nullptr;//���_�V�F�[�_�[
	ID3DBlob* pixelShader = nullptr;//�s�N�Z���V�F�[�_�[
									//�V�F�[�_�[�̃R���p�C�����s��
	result = D3DCompileFromFile(_T("Shader.hlsl"), nullptr, nullptr, "BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader, &error);
	result = D3DCompileFromFile(_T("Shader.hlsl"), nullptr, nullptr, "BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader, &error);


	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g(PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.DepthStencilState.DepthEnable = true;
	gpsDesc.DepthStencilState.StencilEnable = false;
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);
	gpsDesc.InputLayout.NumElements = _countof(input);
	gpsDesc.InputLayout.pInputElementDescs = input;
	gpsDesc.pRootSignature = rootSignature->GetRootSignature();
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.SampleMask = UINT_MAX;
	ID3D12PipelineState* piplineState = nullptr;
	result = dx12->GetDevice()->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&piplineState));

	//�r���[�|�[�g
	D3D12_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = WIN_WIDTH;
	viewPort.Height = WIN_HEIGTH;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	std::shared_ptr<Camera> camera;
	camera.reset(new Camera(dx12->GetDevice()));
	camera->CreateCamera();

	const float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	unsigned int bbIndex = 0;
	dx12->GetAllocator()->Reset();
	dx12->GetList()->Reset(dx12->GetAllocator(), piplineState);

	//���[�v
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {//OS���瓊�����Ă郁�b�Z�[�W��msg�Ɋi�[
			TranslateMessage(&msg);	//���z�L�[�֘A�̕ϊ�
			DispatchMessage(&msg);	//��������Ȃ��������b�Z�[�W��OS�ɕԂ�
		}
		//���[�g�V�O�l�`���̃Z�b�g
		dx12->GetList()->SetGraphicsRootSignature(rootSignature->GetRootSignature());

		//�p�C�v���C���̃Z�b�g
		dx12->GetList()->SetPipelineState(piplineState);

		//�r���[�|�[�g���Z�b�g
		dx12->GetList()->RSSetViewports(1, &viewPort);
		//�V�U�[���Z�b�g
		const D3D12_RECT rect = { 0, 0, WIN_WIDTH, WIN_HEIGTH };
		dx12->GetList()->RSSetScissorRects(1, &rect);

		dx12->ResourceBarrier(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);


		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart(), bbIndex, descriptorSize);
		dx12->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		dx12->GetList()->ClearDepthStencilView(_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0f, 0, nullptr);
		dx12->GetList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
		ID3D12DescriptorHeap* cbv = camera->GetCBVHeapDesc();
		dx12->GetList()->SetDescriptorHeaps(1, &cbv);
		dx12->GetList()->SetGraphicsRootDescriptorTable(1, camera->GetCBVHeapDesc()->GetGPUDescriptorHandleForHeapStart());

		//�V�F�[�_���\�[�X�r���[�p�̃f�X�N���v�^���Z�b�g
		dx12->GetList()->SetDescriptorHeaps(1, (ID3D12DescriptorHeap* const*)texHeap.GetAddressOf());
		dx12->GetList()->SetGraphicsRootDescriptorTable(0, texHeap.Get()->GetGPUDescriptorHandleForHeapStart());

		model->ModelDraw();

		camera->Update();

		dx12->GetList()->Close();

		ID3D12CommandList* commandList[] = { dx12->GetList() };
		dx12->GetQueue()->ExecuteCommandLists(_countof(commandList), commandList);


		dx12->Wait();
		dx12->GetAllocator()->Reset();
		dx12->GetList()->Reset(dx12->GetAllocator(), piplineState);

		result = dx12->GetSwap()->Present(1, 0);
		if (FAILED(result))
		{
			result = dx12->GetDevice()->GetDeviceRemovedReason();
		}
		bbIndex = dx12->GetSwap()->GetCurrentBackBufferIndex();
	}
	return true;
}

void 
Application::Delete() {
	
}
