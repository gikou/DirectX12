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

#include"DX12Init.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//���_�\����
struct Vertex {
	DirectX::XMFLOAT3 pos;//���W
	DirectX::XMFLOAT2 uv;//uv���W
};

using namespace DirectX;

#define WIN_WIDTH	(640)	//�E�B���h�E�T�C�Y��
#define WIN_HEIGTH	(480)	//�E�B���h�E�T�C�Y��

#define Release(X)	{if((X) != nullptr) (X)->Release();}

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
	Release(debug);
	debug = nullptr;
#endif


	std::shared_ptr<DX12Init> dx12;
	dx12.reset(new DX12Init(hwnd));

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

	//�T���v���̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
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
	std::vector<D3D12_DESCRIPTOR_RANGE> dRange(2);

	//���[�g�p�����[�^�̐ݒ�
	std::vector<D3D12_ROOT_PARAMETER> parameter(2);
	dRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	dRange[0].NumDescriptors = 1;
	dRange[0].BaseShaderRegister = 0;
	dRange[0].RegisterSpace = 0;
	dRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	parameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	parameter[0].DescriptorTable.NumDescriptorRanges = 1;
	parameter[0].DescriptorTable.pDescriptorRanges = &dRange[0];

	dRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dRange[1].NumDescriptors = 1;
	dRange[1].BaseShaderRegister = 0;
	dRange[1].RegisterSpace = 0;
	dRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	parameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter[1].DescriptorTable.NumDescriptorRanges = 1;
	parameter[1].DescriptorTable.pDescriptorRanges = &dRange[1];

	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature = nullptr;
	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = parameter.size();
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pParameters = parameter.data();
	rsDesc.pStaticSamplers = &samplerDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);
	result = dx12->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);

	//�t�F���X
	ID3D12Fence* fence = nullptr;
	UINT64 fenceValue = 0;
	result = dx12->GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//���_���̍쐬
	Vertex vertices[] = {
		{ { -0.7f,  0.7f, 0.0f },{ 0.0f, 0.0f } },
	{ { 0.7f,  0.7f, 0.0f },{ 1.0f, 0.0f } },
	{ { 0.7f, -0.7f, 0.0f },{ 1.0f, 1.0f } },

	{ { 0.7f, -0.7f, 0.0f },{ 1.0f, 1.0f } },
	{ { -0.7f, -0.7f, 0.0f },{ 0.0f, 1.0f } },
	{ { -0.7f,  0.7f, 0.0f },{ 0.0f, 0.0f } }
	};

	//���_���C�A�E�g�̒�`
	D3D12_INPUT_ELEMENT_DESC input[] = {
		//���_
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	//���_�o�b�t�@�̍쐬
	ComPtr<ID3D12Resource> vertexBuffer;
	result = dx12->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPU����GPU�֓]������p
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),//�T�C�Y
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexBuffer.GetAddressOf())
	);
	//�o�b�t�@�ɑ΂��ď�������
	unsigned char* pData = nullptr;
	result = vertexBuffer.Get()->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	memcpy(pData, vertices, sizeof(vertices));//���_�f�[�^���o�b�t�@�ɃR�s�[
	vertexBuffer.Get()->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();//���_�A�h���X��GPU�ɂ���A�h���X���L��
	vbView.StrideInBytes = sizeof(Vertex);//���_1������̃o�C�g�����w��
	vbView.SizeInBytes = sizeof(vertices);//�f�[�^�S�̂̃T�C�Y���w��


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
	dx12->GetList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	dx12->GetList()->Close();

	ID3D12CommandList* commandList[] = { dx12->GetList() };
	dx12->GetQueue()->ExecuteCommandLists(_countof(commandList), commandList);


	dx12->Wait();

	//�V�F�[�_�[�̓ǂݍ���
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
	gpsDesc.DepthStencilState.DepthEnable = false;
	gpsDesc.DepthStencilState.StencilEnable = false;
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);
	gpsDesc.InputLayout.NumElements = _countof(input);
	gpsDesc.InputLayout.pInputElementDescs = input;
	gpsDesc.pRootSignature = rootSignature;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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

	XMMATRIX world = XMMatrixIdentity();
	XMVECTOR eye = { 0,0,-5 };
	XMVECTOR target = { 0,0,0 };
	XMVECTOR upper = { 0,1,0 };


	XMMATRIX camera = XMMatrixLookAtLH(eye, target, upper);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, static_cast<float>(WIN_WIDTH) / static_cast<float>(WIN_HEIGTH), 1.0f, 100.0f);
	XMMATRIX matrix = world * camera * projection;

	ComPtr<ID3D12Resource> _constantBuffer;
	ComPtr<ID3D12DescriptorHeap> _cbvDescHeap;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩���܂��悤�� 
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�R���X�^���g�o�b�t�@�ł� 
	result = dx12->GetDevice()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(_cbvDescHeap.GetAddressOf()));//������ 

	D3D12_HEAP_PROPERTIES cbvHeapProperties = {};
	cbvHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	cbvHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	cbvHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	cbvHeapProperties.VisibleNodeMask = 1;
	cbvHeapProperties.CreationNodeMask = 1;

	D3D12_RESOURCE_DESC cbvResDesc = {};
	cbvResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;//�P�Ȃ�1�����o�b�t�@�Ȃ̂� 
	cbvResDesc.Width = (sizeof(XMMATRIX) + 0xff)&~0xff;//256�A���C�������g 
	cbvResDesc.Height = 1;//1�����Ȃ�łP�ł��� 
	cbvResDesc.DepthOrArraySize = 1;//�[���Ƃ��Ȃ���� 
	cbvResDesc.MipLevels = 1;//�~�b�v�Ƃ��Ȃ���� 
	cbvResDesc.SampleDesc.Count = 1;//����1�ɈӖ��Ȃ��Ǝv���񂾂��ǖ����Ǝ��s 
	cbvResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;//SWIZZLE �Ƃ����ˁ[����B 

	result = dx12->GetDevice()->CreateCommittedResource(
		&cbvHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMMATRIX)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_constantBuffer.GetAddressOf()));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constantBuffer.Get()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (sizeof(XMMATRIX) + 0xff)&~0xff;//256�A���C�������g 
	dx12->GetDevice()->CreateConstantBufferView(&cbvDesc, _cbvDescHeap.Get()->GetCPUDescriptorHandleForHeapStart());

	D3D12_RANGE range = { 0,0 };
	XMMATRIX* matrixAddress = nullptr;
	result = _constantBuffer.Get()->Map(0, &range, (void**)&matrixAddress);
	*matrixAddress = matrix;

	const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	unsigned int bbIndex = 0;
	//���[�v
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {//OS���瓊�����Ă郁�b�Z�[�W��msg�Ɋi�[
			TranslateMessage(&msg);	//���z�L�[�֘A�̕ϊ�
			DispatchMessage(&msg);	//��������Ȃ��������b�Z�[�W��OS�ɕԂ�
		}
		//���[�g�V�O�l�`���̃Z�b�g
		dx12->GetList()->SetGraphicsRootSignature(rootSignature);

		//�p�C�v���C���̃Z�b�g
		dx12->GetList()->SetPipelineState(piplineState);

		//�r���[�|�[�g���Z�b�g
		dx12->GetList()->RSSetViewports(1, &viewPort);
		//�V�U�[���Z�b�g
		const D3D12_RECT rect = { 0, 0, WIN_WIDTH, WIN_HEIGTH };
		dx12->GetList()->RSSetScissorRects(1, &rect);

		dx12->ResourceBarrier(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);


		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart(), bbIndex, descriptorSize);
		dx12->GetList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		dx12->GetList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		dx12->GetList()->SetDescriptorHeaps(1, _cbvDescHeap.GetAddressOf());
		dx12->GetList()->SetGraphicsRootDescriptorTable(1, _cbvDescHeap.Get()->GetGPUDescriptorHandleForHeapStart());

		//�V�F�[�_���\�[�X�r���[�p�̃f�X�N���v�^���Z�b�g
		dx12->GetList()->SetDescriptorHeaps(1, (ID3D12DescriptorHeap* const*)texHeap.GetAddressOf());
		dx12->GetList()->SetGraphicsRootDescriptorTable(0, texHeap.Get()->GetGPUDescriptorHandleForHeapStart());


		//�O�p�|���S���`��ɂ���
		dx12->GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//���_�o�b�t�@�̃Z�b�g
		dx12->GetList()->IASetVertexBuffers(0, 1, &vbView);


		//���_�`��
		dx12->GetList()->DrawInstanced(sizeof(vertices) / sizeof(Vertex), 1, 0, 0);

		dx12->ResourceBarrier(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);


		////���\�[�X�o���A
		//_commandList->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		static float angle = 0.0f;
		world = XMMatrixRotationY(angle);
		angle += 0.01;
		*matrixAddress = XMMatrixTranspose(world * camera * projection);

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
