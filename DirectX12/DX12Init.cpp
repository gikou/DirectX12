#define _CRT_SECURE_NO_WARNINGS

#include <d3d12.h>		//DirectX12を使うのに必要
#include "d3dx12.h"
#include <dxgi1_4.h>	//DXGIを扱うのに必要(DX12ではDXGI1.4が使われてる)
#include <D3Dcompiler.h>//シェーダコンパイラで必要
#include <tchar.h>
#include <vector>
#include <memory>
#include <wrl/client.h>
#include<DirectXTex.h>
#include<assert.h>
#include <shlwapi.h>
#include<minmax.h>

#include"DX12Init.h"
#include"Dx12BufferManager.h"
#include"PMDModel.h"
#include"PMXModel.h"
#include"LoadMotion.h"
#include"src\RootSignature\RootSignature.h"
#include"src\PiplineState\PiplineState.h"
#include"PrimitiveCreator.h"
#include"Plane.h"
#include"Cube.h"
#include"Cylinder.h"
#include"SoundManager.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib, "shlwapi.lib")
#define WIN_WIDTH	(640)	//ウィンドウサイズ幅
#define WIN_HEIGTH	(480)	//ウィンドウサイズ高
using namespace DirectX;

const DirectX::XMFLOAT3 LIGHT_POSITION = { -50.0f, 30.0f, -50.0f };
const float VIEW_WIDTH = 40.0f;
const float VIEW_HEIGHT = 40.0f;
const float VIEW_NEAR = 1.0f;
const float VIEW_FAR = 100.0f;

//頂点構造体
struct Vertex {
	DirectX::XMFLOAT3 pos;//座標
	DirectX::XMFLOAT2 uv;//uv座標
};

void ConvertFormat() {

}

ID3D12Resource* 
CreateTexBuffer(ID3D12Device* device, ID3D12Resource* TexBuffer, int w, int h, int array, int mip, DXGI_FORMAT format, bool rtvFlag) {
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC whiteResourceDesc = {};
	whiteResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	whiteResourceDesc.Width = w;
	whiteResourceDesc.Height = h;
	whiteResourceDesc.DepthOrArraySize = array;
	whiteResourceDesc.MipLevels = mip;
	whiteResourceDesc.Format = format;
	whiteResourceDesc.SampleDesc.Count = 1;
	whiteResourceDesc.SampleDesc.Quality = 0;
	whiteResourceDesc.Flags = rtvFlag? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET: D3D12_RESOURCE_FLAG_NONE;
	whiteResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.Type = D3D12_HEAP_TYPE_CUSTOM;
	hprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	hprop.CreationNodeMask = 1;
	hprop.VisibleNodeMask = 1;

	float color[] = { 0.5f,0.5f,0.5f,1.0f };
	D3D12_CLEAR_VALUE clear = CD3DX12_CLEAR_VALUE(format, color);

	
	device->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAG_NONE,
		&whiteResourceDesc,
		rtvFlag ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_GENERIC_READ,
		rtvFlag ? &clear : nullptr,
		IID_PPV_ARGS(&TexBuffer)
	);


	return TexBuffer;
}

void 
DX12Init::RecursiveMatrixMultiply(BoneNode& node, XMMATRIX& inMat) {
	boneMatrices[node.boneIdx] *= inMat;
	for (auto& cnode : node.children) {
		RecursiveMatrixMultiply(*cnode, boneMatrices[node.boneIdx]);
	}
}


void 
DX12Init::BendBone(const char* name, DirectX::XMFLOAT4& q, DirectX::XMFLOAT3& loc, const DirectX::XMFLOAT4& q2 = DirectX::XMFLOAT4(), float t = 0.0f) {
	auto& elbow = boneMap[name];
	auto vec = XMLoadFloat3(&elbow.startPos);
	auto quaternion = XMLoadFloat4(&q);
	auto quaternion2 = XMLoadFloat4(&q2);
	auto completion = XMMatrixRotationQuaternion(XMQuaternionSlerp(quaternion, quaternion2, t));

	auto location = XMMatrixTranslation(loc.x, loc.y, loc.z);
	boneMatrices[elbow.boneIdx] = XMMatrixTranslationFromVector(XMVectorScale(vec, -1))
	 *completion*location 
	 * XMMatrixTranslationFromVector(vec);

	int a = 1;
}


float GetBezierYValueFromXWithNewton(float x, const XMFLOAT2& a, const XMFLOAT2& b, const unsigned int n = 16) {
	if (a.x == a.y&&b.x == b.y)return x;//直線になってるのでx=yである⇒計算不必要 
	
	float t = x;//tは最終的に求めたい媒介変数。初期値はxと同じでいい 
	float k0 = 1 + 3 * a.x - 3 * b.x;//t^3の係数 
	float k1 = 3 * b.x - 6 * a.x;//t^2の係数 
	float k2 = 3 * a.x;//tの係数 

	const float epsilon = 0.0005f;
	//ニュートン法ループ 
	for (int i = 0; i < n; ++i) {
		float r = (1 - t);//プログラムが長くなるので反転の意味でrとしている 
		float ft = (t*t*t)*k0 + t * t*k1 + t * k2 - x;//f(t)です 
		if (ft <= epsilon && ft >= -epsilon)break;//適当なところで計算打ち切り 
		float fdt = (3 * t*t*k0 + 2 * t*k1 + k2);//f'(t)つまりf(t)の微分結果式 
		if (fdt == 0)break;//0除算防止 
		t = t - ft / fdt;//ニュートン法で答に近づく 
	}
	//ループが終わった時点で、求めたいtにある程度近づいている(はず) 
	float r = (1 - t);
	//tが求まったのでyを求める。 
	return 3 * r*r*t*a.y + 3 * r*t*t*b.y + t * t*t;
}



DX12Init::DX12Init()
{
}


DX12Init::~DX12Init()
{
	Delete();
}

void
DX12Init::Delete() {
	
}

void
DX12Init::Dx12SetInit(HWND hwnd, ID3D12Device* dev) {
	_hwnd = hwnd;
	
	//model.reset(new PMDModel("Model/霊夢/reimu.pmd"));
	//model.reset(new PMDModel("Model/初音ミク/初音ミク.pmd"));
	model.reset(new PMDModel("Model/初音ミクメタル/初音ミクmetal.pmd"));
	//model.reset(new PMDModel("Model/hibiki/我那覇響v1.pmd"));
	//model.reset(new PMDModel("Model/巡音ルカ/巡音ルカ.pmd"));
	//pmxmodel.reset(new PMXModel("model/レム/Rem.pmx"));
	//pmxmodel.reset(new PMXModel("model/ラム/Ram.pmx"));
	motion.reset(new LoadMotion("motion/ヤゴコロダンス.vmd"));
	model->ModelLoader();

	mPlane = std::make_shared<Plane>(XMFLOAT3(0.0f, 0.0f, 0.0f), 100.0f, 100.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	mCylinder = std::make_shared<Cylinder>(15.0f, 20.0f, 20);
	mCube = std::make_shared<Cube>(XMFLOAT3(0.0f, 5.0f, 0.0f), XMFLOAT3(1.0f, 1.0f,1.0f), XMFLOAT3(0.5f, 0.0f, 0.0f));
	//SoundManager::GetInstance().Initialize("Sound/Sound.acf","Sound/GameMain.acb","");
	//SoundManager::GetInstance().PlayBGM("MainBGM", 0.0f, 0.0f);
}

std::wstring StringToWstring(const std::string& name) {
	auto bsize = MultiByteToWideChar(CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		name.data(), name.length(), nullptr, 0);

	std::wstring wstr;
	wstr.resize(bsize);

	bsize = MultiByteToWideChar(CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		name.data(), name.length(), &wstr[0], wstr.length());
	assert(bsize == wstr.length());

	return wstr;
}

void
DX12Init::CreateDevice() {
	
	
	//初期化
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = {};

	for (auto i : levels) {	//マシンにあったバージョンを選ぶ
		D3D12CreateDevice(nullptr, i, IID_PPV_ARGS(device.GetAddressOf()));
		/*if (result == S_OK) {
			level = i;
			break;
		}*/
	}

	//return result;
}

void
DX12Init::CreateCommand() {
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_commandAllocator.GetAddressOf()));
	

	//コマンドリストの作成
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(_commandList.GetAddressOf()));
	

	_commandList->Close();
	//コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = 0;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(_commandQueue.GetAddressOf()));
	
	//return result;
}

void
DX12Init::CreateFence() {
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	fenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	//return result;
}

void
DX12Init::CreateSwapChain() {

	//スワップチェイン周り
	CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf()));
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = WIN_WIDTH;
	swapChainDesc.Height = WIN_HEIGTH;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	factory->CreateSwapChainForHwnd(_commandQueue.Get(),
		_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(swapChain.GetAddressOf()));
	
	std::vector <IDXGIAdapter*> adapters;
	IDXGIAdapter* adapter = nullptr;

	for (int i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(adapter);
	}
	//この中から NVIDIA の奴を探す
	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		std::wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos) {//NVIDIAアダプタを強制 
			adapter = adpt;
			break;
		}
	}
	//return result;
}

void
DX12Init::CreateRenderTarget() {
	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC descriptor = {};
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor.NumDescriptors = 2;
	descriptor.NodeMask = 0;

	device->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
	
	//ディスクリプタハンドルの作成
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//レンダーターゲット

	//レンダーターゲット数ぶん確保
	renderTarget.resize(2);
	//ディスクリプタ1個あたりのサイズを取得
	unsigned int descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (int i = 0; i < 2; ++i) {
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i]));//スワップチェインからキャンバスを取得
		device->CreateRenderTargetView(renderTarget[i], nullptr, descriptorHandle);//キャンバスとビューを紐づけ
		descriptorHandle.Offset(descriptorSize);//キャンバスとビューのぶん次のところまでオフセット
	}
	//return result;
}

void 
DX12Init::Create1stPathRTVSRV() {
	D3D12_DESCRIPTOR_HEAP_DESC descriptor = {};
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor.NumDescriptors = 1;
	descriptor.NodeMask = 0;

	device->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(_1stHeapRTV.GetAddressOf()));
	descriptor.NumDescriptors = 2;
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(_1stHeapSRV.GetAddressOf()));

	descriptor.NumDescriptors = 1;
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	device->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(_1stHeapDSV.GetAddressOf()));
	
	auto desc = renderTarget[0]->GetDesc();
	_1stPathBuffer = CreateTexBuffer(device.Get(), _1stPathBuffer.Get(), 640, 480, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, true);
	device->CreateRenderTargetView(_1stPathBuffer.Get(), nullptr, _1stHeapRTV->GetCPUDescriptorHandleForHeapStart());//キャンバスとビューを紐づけ

	D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Format = desc.Format;
	srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(_1stPathBuffer.Get(), &srvdesc, _1stHeapSRV->GetCPUDescriptorHandleForHeapStart());

	//return S_OK;
}


void
DX12Init::Create2ndPathRTVSRV() {
	D3D12_DESCRIPTOR_HEAP_DESC descriptor = {};
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor.NumDescriptors = 1;
	descriptor.NodeMask = 0;

	device->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(_2ndHeapRTV.GetAddressOf()));

	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(_2ndHeapSRV.GetAddressOf()));

	auto desc = renderTarget[0]->GetDesc();

	_2ndPathBuffer = CreateTexBuffer(device.Get(), _2ndPathBuffer.Get(), 640, 480, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, true);
	device->CreateRenderTargetView(_2ndPathBuffer.Get(), nullptr, _2ndHeapRTV->GetCPUDescriptorHandleForHeapStart());//キャンバスとビューを紐づけ

	D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Format = desc.Format;
	srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(_2ndPathBuffer.Get(), &srvdesc, _2ndHeapSRV->GetCPUDescriptorHandleForHeapStart());

	//return S_OK;
}

void
DX12Init::CreateRootSgnature() {
	
	rootsignater["1Path"] = std::make_shared<RootSignature>();
	rootsignater["1Path"]->SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0);
	rootsignater["1Path"]->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);
	rootsignater["1Path"]->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	rootsignater["1Path"]->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
	rootsignater["1Path"]->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2);
	rootsignater["1Path"]->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3);
	rootsignater["1Path"]->SetTableRange(2, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2);

	rootsignater["1Path"]->CreateRootSignature(device.Get());

}

void 
DX12Init::Create1stPathRootSgnature() {

	rootsignater["2Path"] = std::make_shared<RootSignature>();
	rootsignater["2Path"]->SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	rootsignater["2Path"]->SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1);
	rootsignater["2Path"]->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0);
	rootsignater["2Path"]->SetTableRange(2, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2);

	rootsignater["2Path"]->CreateRootSignature(device.Get());

}

void
DX12Init::Create2ndPathRootSgnature() {
	
	rootsignater["3Path"] = std::make_shared<RootSignature>();
	rootsignater["3Path"]->SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	rootsignater["3Path"]->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	rootsignater["3Path"]->CreateRootSignature(device.Get());
}

void
DX12Init::CreateVertex() {
	auto vertices = model->GetVertices();
	//auto vertices = pmxmodel->GetVertices();

	size_t size = vertices.size() * sizeof(vertices[0]);

	//頂点バッファの作成
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexBuffer.GetAddressOf())
	);
	
	//バッファに対して書き込む
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	memcpy(pData, &vertices[0], size);//頂点データをバッファにコピー
	vertexBuffer->Unmap(0, nullptr);
	

	//頂点バッファビューの作成
	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();//頂点アドレスのGPUにあるアドレスを記憶
	vbView.StrideInBytes = sizeof(vertices[0]);//頂点1つあたりのバイト数を指定
	vbView.SizeInBytes = size;//データ全体のサイズを指定

	//return result;
}

void
DX12Init::CreateIndeis() {
	
	std::vector<unsigned short> indices = model->GetIndices();
	//std::vector<unsigned short> indices = pmxmodel->GetIndices();
	//頂点バッファの作成

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0])),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_indexBuffer.GetAddressOf())
	);
	
	//バッファに対して書き込む
	unsigned short* indexData = nullptr;
	_indexBuffer->Map(0, 0, reinterpret_cast<void**>(&indexData));
	memcpy(indexData, &indices[0], indices.size() * sizeof(indices[0]));//頂点データをバッファにコピー
	_indexBuffer->Unmap(0, nullptr);
	
	//頂点バッファビューの作成
	indexView = {};
	indexView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();//頂点アドレスのGPUにあるアドレスを記憶
	indexView.Format = DXGI_FORMAT_R16_UINT;
	indexView.SizeInBytes = indices.size() * sizeof(indices[0]);//データ全体のサイズを指定

	//return result;
}

void
DX12Init::Create1stPathCanvasPorigonn() {
	Vertex vertices[] = {
		XMFLOAT3(-1,-1,0),XMFLOAT2(0,1),//正面 
		XMFLOAT3(-1,1,0),XMFLOAT2(0,0),//正面 
		XMFLOAT3(1,-1,0),XMFLOAT2(1,1),//正面 
		XMFLOAT3(1,1,0),XMFLOAT2(1,0),//正面 
	};
	size_t size = sizeof(vertices);
	size = (size + 0xff)&~0xff;
	//頂点バッファの作成
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(canvas1stBuffer.GetAddressOf())
	);
	
	size_t sizea = sizeof(vertices);
	canvas1stBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_1stData));
	memcpy(_1stData, &vertices[0], sizeof(vertices));//頂点データをバッファにコピー
	canvas1stBuffer->Unmap(0, nullptr);
	
	_1stCanvasView.BufferLocation = canvas1stBuffer->GetGPUVirtualAddress();
	_1stCanvasView.SizeInBytes = size;
	_1stCanvasView.StrideInBytes = sizeof(Vertex);

	std::vector<float> weights(640);
	float total = 0.0f;
	float x = 0.0f;
	float s = 2.0f;
	for (auto& wgt : weights) {
		wgt = expf(-x * x / (2 * s*s));
		total += wgt;
		x += 1.0f;
	}
	//足して１になるようにする 
	total = total * 2.0f - 1;
	for (auto& wgt : weights) {
		wgt /= total;
	}

	size = sizeof(XMFLOAT4)*2;
	size = (size + 0xff)&~0xff;

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(gauss1stBuffer.GetAddressOf()));

	D3D12_DESCRIPTOR_HEAP_DESC canvasHeapDesc = {};
	canvasHeapDesc.NumDescriptors = 1;
	canvasHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	canvasHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	device->CreateDescriptorHeap(&canvasHeapDesc, IID_PPV_ARGS(gauss1stDescHeap.GetAddressOf()));
	

	D3D12_CONSTANT_BUFFER_VIEW_DESC gaussDesc = {};
	gaussDesc.BufferLocation = gauss1stBuffer->GetGPUVirtualAddress();
	gaussDesc.SizeInBytes = size;

	device->CreateConstantBufferView(&gaussDesc, gauss1stDescHeap->GetCPUDescriptorHandleForHeapStart());

	gauss1stBuffer->Map(0, nullptr, (void**)&gauss1stMap);
	memcpy(gauss1stMap, &weights[0], size);
	//-----------
	TexMetadata metadata = {};
	ScratchImage image = {};
	DirectX::LoadFromWICFile(L"image/NormalMap.png", 0, &metadata, image);
	normalTexBuffer = CreateTexBuffer(device.Get(), normalTexBuffer.Get(), metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels, metadata.format, false);
	normalTexBuffer->WriteToSubresource(
		0,
		nullptr,
		image.GetPixels(),
		metadata.width * 4,
		image.GetPixelsSize());

	D3D12_SHADER_RESOURCE_VIEW_DESC textureDesc = {};
	textureDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	textureDesc.Texture2D.MipLevels = 1;
	textureDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	auto handle = _1stHeapSRV->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	device->CreateShaderResourceView(normalTexBuffer.Get(), &textureDesc, handle);
}


void
DX12Init::Create2ndPathCanvasPorigonn() {
	Vertex vertices[] = {
		XMFLOAT3(-1,-1,0),XMFLOAT2(0,1),//正面 
		XMFLOAT3(-1,1,0),XMFLOAT2(0,0),//正面 
		XMFLOAT3(1,-1,0),XMFLOAT2(1,1),//正面 
		XMFLOAT3(1,1,0),XMFLOAT2(1,0),//正面 
	};
	size_t size = sizeof(vertices);
	size = (size + 0xff)&~0xff;
	//頂点バッファの作成
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(canvas2ndBuffer.GetAddressOf())
	);

	canvas2ndBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_2ndData));
	memcpy(_2ndData, &vertices[0], sizeof(vertices));//頂点データをバッファにコピー
	canvas2ndBuffer->Unmap(0, nullptr);
	
	_2ndCanvasView.BufferLocation = canvas2ndBuffer->GetGPUVirtualAddress();
	_2ndCanvasView.SizeInBytes = size;
	_2ndCanvasView.StrideInBytes = sizeof(Vertex);





	std::vector<float> weights(480);
	float total = 0.0f;
	float y = 0.0f;
	float s = 2.0f;
	for (auto& wgt : weights) {
		wgt = expf(-y * y / (2 * s*s));
		total += wgt;
		y += 1.0f;
	}
	//足して１になるようにする 
	total = total * 2.0f - 1;
	for (auto& wgt : weights) {
		wgt /= total;
	}

	size = sizeof(XMFLOAT4) * 2;
	size = (size + 0xff)&~0xff;

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(gauss2ndBuffer.GetAddressOf()));

	D3D12_DESCRIPTOR_HEAP_DESC canvasHeapDesc = {};
	canvasHeapDesc.NumDescriptors = 1;
	canvasHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	canvasHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	device->CreateDescriptorHeap(&canvasHeapDesc, IID_PPV_ARGS(gauss2ndDescHeap.GetAddressOf()));
	

	D3D12_CONSTANT_BUFFER_VIEW_DESC gaussDesc = {};
	gaussDesc.BufferLocation = gauss2ndBuffer->GetGPUVirtualAddress();
	gaussDesc.SizeInBytes = size;
	auto handle = gauss2ndDescHeap->GetCPUDescriptorHandleForHeapStart();

	device->CreateConstantBufferView(&gaussDesc, handle);

	gauss2ndBuffer->Map(0, nullptr, (void**)&gauss2ndMap);
	memcpy(gauss2ndMap, &weights[0], size);

}


void 
DX12Init::CreateModelTextures() {
	auto& texPath = model->GetTexturePath();
	//auto& texPath = pmxmodel->GetTexturePath();

	modelTextureBuffer.resize(texPath.size());
	sphirTextureBuffer.resize(texPath.size());
	int i = 0;
	for (auto& tex : texPath) {
		TexMetadata metadata = {};
		ScratchImage image;
		if (tex.normal != "") {
			auto a = tex.normal.find(".");
			std::string ext = tex.normal.substr(a + 1, tex.normal.size());

			if ((ext == "bmp") || (ext == "jpg") || (ext == "png")) {
				auto texture = StringToWstring(tex.normal.c_str());
				DirectX::LoadFromWICFile(texture.c_str(), 0, &metadata, image);
			}
			else if (ext == "tga") {
				auto texture = StringToWstring(tex.normal.c_str());
				DirectX::LoadFromTGAFile(texture.c_str(), &metadata, image);
			}
			modelTextureBuffer[i] = CreateTexBuffer(device.Get(), modelTextureBuffer[i].Get(), metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels, metadata.format, false);
			modelTextureBuffer[i]->WriteToSubresource(
				0,
				nullptr,
				image.GetPixels(),
				metadata.width * 4,
				image.GetPixelsSize());
		}

		if (tex.sphir != "") {
			auto a = tex.sphir.find(".");
			std::string ext = tex.sphir.substr(a + 1, tex.sphir.size());
			auto texture = StringToWstring(tex.sphir.c_str());
			DirectX::LoadFromWICFile(texture.c_str(), 0, &metadata, image);
			sphirTextureBuffer[i] = CreateTexBuffer(device.Get(), modelTextureBuffer[i].Get(), metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels, metadata.format, D3D12_RESOURCE_FLAG_NONE);
			sphirTextureBuffer[i]->WriteToSubresource(
				0,
				nullptr,
				image.GetPixels(),
				metadata.width * 4,
				image.GetPixelsSize());
		}
		/*_commandAllocator->Reset();
		_commandList->Reset(_commandAllocator, nullptr);*/
		//テクスチャの読み込み

		image.Release();
		i++;
	}

	/*auto a = modelTextureBuffer[7];
	modelTextureBuffer[7] = modelTextureBuffer[6];
	modelTextureBuffer[6] = a;*/

	//return S_OK;
	
}

std::string 
DX12Init::GetToon(int index) {
	auto& tpaths = model->GetToonTexNames();
	std::string retPath = "toon/";
	retPath += tpaths[index];
	if (PathFileExistsA(retPath.c_str())) {
		return retPath;
	}
	else {
		return model->GetModelFolder() + tpaths[index];
	}

}

void 
DX12Init::CretaeToonTexture() {
	auto material = model->GetMaterials();
	//auto material = pmxmodel->GetMaterials();
	int i = 0;
	toonTextureBuffer.resize(material.size());
	for (auto mat : material) {
		TexMetadata metadata = {};
		ScratchImage image = {};

		/*if (mat.toonFlag == 0) { 
			i++;
			continue;

		}
		auto toonPaht = pmxmodel->GetToonPath()[i];*/
		if (mat.toonIdx == 0xff) {
			continue;
		}
		auto toonPaht = GetToon(mat.toonIdx);
		
		auto texture = StringToWstring(toonPaht);
		DirectX::LoadFromWICFile(texture.c_str(), 0, &metadata, image);

		toonTextureBuffer[i] = CreateTexBuffer(device.Get(), toonTextureBuffer[i].Get(), metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels, metadata.format, false);

		toonTextureBuffer[i]->WriteToSubresource(
			0,
			nullptr,
			image.GetPixels(),
			metadata.width * 4,
			image.GetPixelsSize());

		image.Release();
		i++;
	}
	//return S_OK;
}

void
DX12Init::CreateBlackTexBuffer(){


	blackTexBuffer =  CreateTexBuffer(device.Get(), blackTexBuffer.Get(), 4, 4, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_NONE);
	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0x00);
	blackTexBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);
}

void
DX12Init::CreateWhiteTexBuffer() {
	whiteTexBuffer = CreateTexBuffer(device.Get(), whiteTexBuffer.Get(), 4, 4, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_NONE);

	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);

	whiteTexBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);
}

void
DX12Init::CreateShader() {
	ComPtr<ID3DBlob> vertexShader;//頂点シェーダー
	ComPtr<ID3DBlob> pixelShader;//ピクセルシェーダー

	ComPtr<ID3DBlob> effect1stPathVertexShader;//頂点シェーダー
	ComPtr<ID3DBlob> effect1stPathPixelShader;//ピクセルシェーダー

	ComPtr<ID3DBlob> effect2ndPathVertexShader;//頂点シェーダー
	ComPtr<ID3DBlob> effect2ndPathPixelShader;//ピクセルシェーダー

	ID3DBlob* error = nullptr;
	HRESULT result = S_OK;
	//頂点レイアウトの定義
	std::vector<D3D12_INPUT_ELEMENT_DESC> input = {
		//頂点
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "BONENO",0,DXGI_FORMAT_R16G16_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R8_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	

	piplineStates["1Path"] = std::make_shared<PiplineState>();
	piplineStates["1Path"]->CompileShader(_T("Shader.hlsl"), "BasicVS", "BasicPS");
	piplineStates["1Path"]->SetInputLayout(input);
	result = piplineStates["1Path"]->CreatePiplineState(device.Get(), rootsignater["1Path"]->GetRootSignature());

	//エフェクトの定義
	std::vector<D3D12_INPUT_ELEMENT_DESC> effectInput = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};

	piplineStates["2Path"] = std::make_shared<PiplineState>();
	piplineStates["2Path"]->CompileShader(_T("EffectCanvas.hlsl"), "EffectVS", "EffectPS");
	piplineStates["2Path"]->SetInputLayout(effectInput);
	result = piplineStates["2Path"]->CreatePiplineState(device.Get(), rootsignater["2Path"]->GetRootSignature());


	////シェーダーのコンパイルを行う
	//D3DCompileFromFile(_T("EffectCanvas.hlsl"), nullptr, nullptr, "EffectVS", "vs_5_0",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &effect1stPathVertexShader, &error);
	///*if (FAILED(result)) {
	//	std::string strErr;
	//	strErr.resize(error->GetBufferSize());
	//	memcpy(&strErr[0], &error[0], error->GetBufferSize());
	//	::OutputDebugStringA(strErr.c_str());
	//	return result;
	//}*/
	//D3DCompileFromFile(_T("EffectCanvas.hlsl"), nullptr, nullptr, "EffectPS", "ps_5_0",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &effect1stPathPixelShader, &error);
	///*if (FAILED(result)) {
	//	std::string strErr;
	//	strErr.resize(error->GetBufferSize());
	//	memcpy(&strErr[0], error->GetBufferPointer(), error->GetBufferSize());
	//	::OutputDebugStringA(strErr.c_str());
	//	return result;
	//}*/

	////パイプラインステートオブジェクト(PSO)
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC efect1stGPUDesc = {};
	//efect1stGPUDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	//efect1stGPUDesc.BlendState.AlphaToCoverageEnable = true;
	//efect1stGPUDesc.DepthStencilState.DepthEnable = false;
	//efect1stGPUDesc.DepthStencilState.StencilEnable = false;
	//efect1stGPUDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//efect1stGPUDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	//efect1stGPUDesc.VS = CD3DX12_SHADER_BYTECODE(effect1stPathVertexShader.Get());
	//efect1stGPUDesc.PS = CD3DX12_SHADER_BYTECODE(effect1stPathPixelShader.Get());
	//efect1stGPUDesc.InputLayout.NumElements = _countof(effect1stPathInput);
	//efect1stGPUDesc.InputLayout.pInputElementDescs = effect1stPathInput;
	//efect1stGPUDesc.pRootSignature = rootsignater["2Path"]->GetRootSignature();
	//efect1stGPUDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//efect1stGPUDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//efect1stGPUDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//efect1stGPUDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//efect1stGPUDesc.SampleDesc.Count = 1;
	//efect1stGPUDesc.NumRenderTargets = 1;
	//efect1stGPUDesc.SampleMask = UINT_MAX;

	//result = device->CreateGraphicsPipelineState(&efect1stGPUDesc, IID_PPV_ARGS(_1stPathPipelineState.GetAddressOf()));



	//シェーダーのコンパイルを行う
	D3DCompileFromFile(_T("Effect2ndPathCanvas.hlsl"), nullptr, nullptr, "EffectVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &effect2ndPathVertexShader, &error);
	/*if (FAILED(result)) {
		std::string strErr;
		strErr.resize(error->GetBufferSize());
		memcpy(&strErr[0], &error[0], error->GetBufferSize());
		::OutputDebugStringA(strErr.c_str());
		return result;
	}*/
	D3DCompileFromFile(_T("Effect2ndPathCanvas.hlsl"), nullptr, nullptr, "EffectPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &effect2ndPathPixelShader, &error);
	/*if (FAILED(result)) {
		std::string strErr;
		strErr.resize(error->GetBufferSize());
		memcpy(&strErr[0], error->GetBufferPointer(), error->GetBufferSize());
		::OutputDebugStringA(strErr.c_str());
		return result;
	}*/

	//パイプラインステートオブジェクト(PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC efect2ndGPUDesc = {};
	efect2ndGPUDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	efect2ndGPUDesc.BlendState.AlphaToCoverageEnable = true;
	efect2ndGPUDesc.DepthStencilState.DepthEnable = false;
	efect2ndGPUDesc.DepthStencilState.StencilEnable = false;
	efect2ndGPUDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	efect2ndGPUDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	efect2ndGPUDesc.VS = CD3DX12_SHADER_BYTECODE(effect2ndPathVertexShader.Get());
	efect2ndGPUDesc.PS = CD3DX12_SHADER_BYTECODE(effect2ndPathPixelShader.Get());
	efect2ndGPUDesc.InputLayout.NumElements = effectInput.size();
	efect2ndGPUDesc.InputLayout.pInputElementDescs = effectInput.data();
	efect2ndGPUDesc.pRootSignature = rootsignater["3Path"]->GetRootSignature();
	efect2ndGPUDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	efect2ndGPUDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	efect2ndGPUDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	efect2ndGPUDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	efect2ndGPUDesc.SampleDesc.Count = 1;
	efect2ndGPUDesc.NumRenderTargets = 1;
	efect2ndGPUDesc.SampleMask = UINT_MAX;

	device->CreateGraphicsPipelineState(&efect2ndGPUDesc, IID_PPV_ARGS(_2ndPathPipelineState.GetAddressOf()));
	
	//return result;
}

void
DX12Init::CreateConstantBuffer() {
	BaseMatrixes matrix = {};
	auto world = XMMatrixRotationY(0.2);
	eye = XMFLOAT3(0, 20, -13);
	target = XMFLOAT3(0, 10, 0);
	auto up = XMFLOAT3(0, 1, 0);
	camera = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(640) / static_cast<float>(480), 0.1f, 300.0f);


	XMFLOAT3 toLight = LIGHT_POSITION;
	XMVECTOR lightPos = XMLoadFloat3(&toLight);
	XMMATRIX lightview = XMMatrixLookAtLH(lightPos, XMLoadFloat3(&target), XMLoadFloat3(&up));
	XMMATRIX lightproj = XMMatrixOrthographicLH(VIEW_WIDTH, VIEW_HEIGHT, VIEW_NEAR, VIEW_FAR);
	matrix.ligth = lightview * lightproj;
	

	matrix.world = world;
	matrix.viewproj = camera * projection;
	
	matrix.eye = { eye.x, eye.y, eye.z, 1.0f};
	

	size_t size = sizeof(matrix);
	size = (size + 0xff)&~0xff;

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_constantBuffer.GetAddressOf()));
	/*if (FAILED(result)) {
		return result;
	}*/
	//リファクタリング最中
	/*buffer.reset(new Dx12BufferManager(device));
	buffer->CreateDescriptorHeap();*/

	D3D12_RANGE range = { 0,0 };
	
	_constantBuffer->Map(0, &range, (void**)&matrixAddress);
	*matrixAddress = matrix;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(registerDescHeap.GetAddressOf()));
	

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = size;
	auto handle = registerDescHeap->GetCPUDescriptorHandleForHeapStart();

	device->CreateConstantBufferView(&cbvDesc, handle);

	//return result;
}

void
DX12Init::CreateMaterialBuffer() {
	auto material = model->GetMaterials();
	//auto material = pmxmodel->GetMaterials();
	std::vector<Material> mats = {};
	mats.resize(material.size());
	for (int i = 0; i < material.size(); ++i) {
		mats[i].diffuse.x =material[i].diffuse.x;
		mats[i].diffuse.y =material[i].diffuse.y;
		mats[i].diffuse.z =material[i].diffuse.z;
		//mats[i].diffuse.w =material[i].diffuse.w;
		mats[i].diffuse.w = material[i].alpha;

		mats[i].specular.x =material[i].specular.x;
		mats[i].specular.y =material[i].specular.y;
		mats[i].specular.z =material[i].specular.z;
		mats[i].specular.w =material[i].specularity;

		mats[i].ambient.x = material[i].ambient.x;
		mats[i].ambient.y = material[i].ambient.y;
		mats[i].ambient.z = material[i].ambient.z;
	}

	size_t size = sizeof(Material);
	size = (size + 0xff)&~0xff;
	_materialsBuffer.resize(material.size());
	int midx = 0;
	for (auto& mbuff : _materialsBuffer) {
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mbuff));

		Material* material = nullptr;
		mbuff->Map(0, nullptr, (void**)&material);
		*material = mats[midx];
		mbuff->Unmap(0, nullptr);
		++midx;
	}

	D3D12_DESCRIPTOR_HEAP_DESC materialHeapDesc = {};
	materialHeapDesc.NumDescriptors =material.size() * 5;
	materialHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えますように 
	materialHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//コンスタントバッファです 
	device->CreateDescriptorHeap(&materialHeapDesc, IID_PPV_ARGS(materialDescHeap.GetAddressOf()));//いつもの 

	D3D12_SHADER_RESOURCE_VIEW_DESC textureDesc = {};
	textureDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	textureDesc.Texture2D.MipLevels = 1;
	textureDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	auto handle = materialDescHeap->GetCPUDescriptorHandleForHeapStart();
	//auto& texPath = pmxmodel->GetTexturePath();
	auto& texPath = model->GetTexturePath();
	for (int i = 0; i <material.size(); ++i) {
		//定数
		D3D12_CONSTANT_BUFFER_VIEW_DESC materialDesc = {};
		materialDesc.BufferLocation = _materialsBuffer[i]->GetGPUVirtualAddress();
		materialDesc.SizeInBytes = size;
		device->CreateConstantBufferView(&materialDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		auto a = texPath[i].normal.find(".");
		std::string ext = texPath[i].normal.substr(a + 1, texPath[i].normal.size());
		auto texbuffer = whiteTexBuffer;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (modelTextureBuffer[i] != nullptr) {
			textureDesc.Format = modelTextureBuffer[i]->GetDesc().Format;
			texbuffer = modelTextureBuffer[i];
		}
		device->CreateShaderResourceView(texbuffer.Get(), &textureDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//spa
		a = texPath[i].sphir.find(".");
		ext = texPath[i].sphir.substr(a + 1, texPath[i].sphir.size());
		texbuffer = blackTexBuffer;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (ext == "spa") {
			texbuffer = sphirTextureBuffer[i];
		}
		device->CreateShaderResourceView(texbuffer.Get(), &textureDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//sph
		a = texPath[i].sphir.find(".");
		ext = texPath[i].sphir.substr(a + 1, texPath[i].sphir.size());
		texbuffer = whiteTexBuffer;
		if (ext == "sph") {
			texbuffer = sphirTextureBuffer[i];
		}
		device->CreateShaderResourceView(texbuffer.Get(), &textureDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//toon
		texbuffer = whiteTexBuffer;
		if (toonTextureBuffer[i] != nullptr) {
			texbuffer = toonTextureBuffer[i];
		}
		device->CreateShaderResourceView(texbuffer.Get(), &textureDesc, handle);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	//return S_OK;
}

void 
DX12Init::CreateBone() {
	boneMatrices.resize(model->GetBones().size());
	std::fill(boneMatrices.begin(), boneMatrices.end(), XMMatrixIdentity());

	auto& mbones = model->GetBones();
	for (int idx = 0; idx < mbones.size(); ++idx) {
		auto b = model->GetBones()[idx];
		auto& boneNode = boneMap[b.boneName];

		boneNode.boneIdx = idx;
		boneNode.startPos = b.headPos;
		boneNode.endPos = mbones[b.childIndex].headPos;
	}

	for (auto& b : boneMap) {
		if (mbones[b.second.boneIdx].parentIndex >= mbones.size())continue;
		auto parentName = mbones[mbones[b.second.boneIdx].parentIndex].boneName;
		boneMap[parentName].children.push_back(&b.second);
	}

	//return S_OK;
}

void 
DX12Init::CreateBonesBuffer() {
	//バッファの作成 
	size_t size = sizeof(XMMATRIX)*boneMatrices.size();
	size = (size + 0xff)&~0xff;
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(boneBuffer.GetAddressOf()));

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(boneHeap.GetAddressOf()));

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = boneBuffer->GetGPUVirtualAddress();
	desc.SizeInBytes = size;
	auto handle = boneHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateConstantBufferView(&desc, handle);
	
	boneBuffer->Map(0, nullptr, (void**)&mappedBones);
	memcpy(mappedBones, &boneMatrices[0], size);


	//return S_OK;
}

void DX12Init::CCDIK()
{


	std::fill(boneMatrices.begin(), boneMatrices.end(), XMMatrixIdentity());//初期化 

	static XMFLOAT3 offsetpos = { 0.0f,0.0f,0.0f };
	unsigned char keystate[256];
	if (GetKeyboardState(keystate)) {
		if (keystate['W'] & 0x80) {
			offsetpos.y += 0.2f;
		}else if (keystate['S'] & 0x80) {
			offsetpos.y -= 0.2f;
		}
		if (keystate['A'] & 0x80) {
			offsetpos.x -= 0.2f;
		}
		else if (keystate['D'] & 0x80) {
			offsetpos.x += 0.2f;
		}
		if (keystate['E'] & 0x80) {
			offsetpos.z -= 0.2f;
		}
		else if (keystate['R'] & 0x80) {
			offsetpos.z += 0.2f;
		}
	}
	auto ikData = model->GetIKData();
	auto boneData = model->GetBones();
	int ikIdx = ikData[3].boneIndex;//IKボーンマトリクス番号を取得 
	std::vector<unsigned short>& itn = ikData[3].childBoneIndex;//IKに対応するボーンのインデックス 
	std::vector<unsigned short>::iterator it = itn.begin();

	DirectX::XMFLOAT3 offset = offsetpos;


	std::vector<XMFLOAT3> tmpLocation(itn.size());//IKから支点までのボーン座標(IKは含まない) 
	XMFLOAT3 ikpos = boneData[ikIdx].headPos;//IKの座標 
	XMFLOAT3 targetpos = ikpos + offset;//IKの座標(目的地やから変わらへん) 
	mCube->SetPosition({ targetpos.x,targetpos.y-5.0f,targetpos.z});
	int nodecount = itn.size();//チェーンノード数 

	for (int i = 0; i<nodecount; ++i) {
		tmpLocation[i] = boneData[itn[i]].headPos;
	}


	XMFLOAT3 ikOriginRootVec = ikpos - tmpLocation[ikData[3].length - 1];
	XMFLOAT3 ikTargetRootVec = targetpos - tmpLocation[ikData[3].length - 1];
	//XMMATRIX matIkRot = LookAtMatrix(Normalize(ikOriginRootVec), Normalize(ikTargetRootVec),DirectX::XMFLOAT3(0, 1, 0), XMFLOAT3(1, 0, 0));
	XMFLOAT3 rootAxis = Cross(Normalize(ikOriginRootVec), Normalize(ikTargetRootVec));
	float rootAngle = XMVector3AngleBetweenNormals(XMLoadFloat3(&Normalize(ikOriginRootVec)), XMLoadFloat3(&Normalize(ikTargetRootVec))).m128_f32[0];
	XMVECTOR matIkRot = XMQuaternionRotationAxis(XMLoadFloat3(&rootAxis), rootAngle);

	for (int c = 0; c<ikData[3].iterationNum; ++c) {
		for (int i = 0; i<nodecount; ++i) {



			if (ikpos == targetpos) {
				break;
			}
			XMFLOAT3 originVec = ikpos - tmpLocation[i];//もとの先っちょIKとさかのぼりノードでベクトル作成
			XMFLOAT3 transVec = targetpos - tmpLocation[i];//目標地点とさかのぼりノードでベクトルを作成
				//ベクトル長が小さすぎる場合は処理を打ち切る 
			if (abs(Length(transVec))<0.1 || abs(Length(originVec))<0.1) {
				return;
			}

			//軸作成 
			XMFLOAT3 axis =Cross( Normalize(originVec),Normalize(transVec));//ちなみに^は外積として扱っている
			const char* name = boneData[itn[i]].boneName;

			if (strcmp(boneData[itn[i]].boneName, " 右 ひ ざ ") == 0 ||
				strcmp(boneData[itn[i]].boneName, "左ひざ") == 0) {
				axis.x = -1;//norm; 
				axis.y = 0;
				axis.z = 0;
			}
			else {
				if (Length(axis) == 0) {
					return;
				}
			}

			if (strcmp(boneData[itn[i]].boneName, "左ひざ") == 0 && axis.x != -1) {
				return;
			}

			//角度計算 
			float rot = XMVector3AngleBetweenNormals(XMLoadFloat3(&Normalize(originVec)), XMLoadFloat3(&Normalize(transVec))).m128_f32[0];
			rot *= 0.5;
			//角度が小さすぎる場合は処理を打ち切る 
			if (abs(rot) == 0.000f) {
				return;
			}
			float strict = ikData[3].restriction*4.0f;
			if (strict <= rot || -strict >= rot) {
				int a = 0;
			}
			rot = min(rot, strict);
			rot = max(rot, -strict);
			XMVECTOR q = XMQuaternionRotationAxis(XMLoadFloat3(&axis), rot);

			//ボーンの変換行列を計算 
			XMFLOAT3 offset = boneData[itn[i]].headPos;
			XMMATRIX RotAt = XMMatrixTranslation(-offset.x, -offset.y, -offset.z)*
				XMMatrixRotationQuaternion(q)*
				XMMatrixTranslation(offset.x, offset.y, offset.z);


			boneMatrices[itn[i]] = boneMatrices[itn[i]] * RotAt;

			offset = tmpLocation[i];

			//理論上の変換行列を計算 
			RotAt = XMMatrixTranslation(-offset.x, -offset.y, -offset.z)*
				XMMatrixRotationQuaternion(q)*
				XMMatrixTranslation(offset.x, offset.y, offset.z);

			//理論値を更新 
			ikpos = ikpos*RotAt;
			for (int j = i - 1; j >= 0; --j) {
				tmpLocation[j] = tmpLocation[j] * RotAt;
			}
		}
	}
}

void 
DX12Init::MotionUpdate(int frameno) {
	frameno = frameno % motion->MaxNum();
	//行列の初期化
	std::fill(boneMatrices.begin(), boneMatrices.end(), XMMatrixIdentity());//初期化 

	//rootボーンから末端まで回転情報を渡していく
	for (auto& boneanim : motion->GetAnimations()) {
		std::sort(boneanim.second.begin(), boneanim.second.end(), [](const KeyFrame& a, const KeyFrame& b) {return a.frameNo < b.frameNo; });
		auto& keyframes = boneanim.second;
		auto rit = std::find_if(keyframes.rbegin(), keyframes.rend(), [frameno](const KeyFrame& k) {return k.frameNo <= frameno; });
		auto it = rit.base();
		if (it == keyframes.end()) {
			//キーフレームがあるとき
			BendBone(boneanim.first.c_str(), rit->quaternion,rit->location);
		}
		else {
			//補間処理をします
			float a = rit->frameNo;
			float b = it->frameNo;
			float t = (static_cast<float>(frameno) - a) / (b - a);
			//補間曲線
			t = GetBezierYValueFromXWithNewton(t, it->bz1, it->bz2);

			BendBone(boneanim.first.c_str(), rit->quaternion,  rit->location, it->quaternion, t);
		}
	}
	//回転情報をモデルに渡す
	RecursiveMatrixMultiply(boneMap["センター"], boneMatrices[0]);
	memcpy(mappedBones, &boneMatrices[0], boneMatrices.size() * sizeof(XMMATRIX));
}

void 
DX12Init::CreateDepth() {
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = 640;//画面に対して使うバッファなので画面幅 
	depthResDesc.Height = 480;//画面に対して使うバッファなので画面高さ 
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_R32_TYPELESS;//必須(大事)デプスですしおすし 
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//必須(大事) 

	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;//デフォルトでよい 
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;//別に知らなくてもOK 
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;//別に知らなくてもOK 

																   //このクリアバリューが重要な意味を持つので今回は作っておく 
	D3D12_CLEAR_VALUE _depthClearValue = {};
	_depthClearValue.DepthStencil.Depth = 1.0f;//深さ最大値は１ 
	_depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //デプス書き込みに使います 
		&_depthClearValue,
		IID_PPV_ARGS(&_depthBuffer));

	D3D12_DESCRIPTOR_HEAP_DESC _dsvHeapDesc = {};//ぶっちゃけ特に設定の必要はないっぽい 
	_dsvHeapDesc.NumDescriptors = 1;
	_dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	_dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&_dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(_depthBuffer.Get(), &dsvDesc, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(_depthBuffer.Get(), &srvdesc, _1stHeapDSV->GetCPUDescriptorHandleForHeapStart());

	//return S_OK;
}

size_t Roundup2Multiplier(size_t size) {
	size_t bit = 0x8000000;
	for (size_t i = 31; i >= 0; --i) {
		if (size&bit)break;
		bit >>= 1;
	}
	return bit << 1;
}

void 
DX12Init::CreateShadowMap() {
	D3D12_DESCRIPTOR_HEAP_DESC shadowmapHD = {};
	shadowmapHD.NumDescriptors = 1;
	shadowmapHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//深度ステンシル 
	device->CreateDescriptorHeap(&shadowmapHD, IID_PPV_ARGS(&shadowHeapDSV));

	shadowmapHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//深度ステンシル 
	shadowmapHD.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&shadowmapHD, IID_PPV_ARGS(&shadowHeapSRV));

	size_t ssize = max(640, 480);
	ssize = Roundup2Multiplier(ssize);
	D3D12_RESOURCE_DESC shadowmapResDesc = {};
	shadowmapResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	shadowmapResDesc.Width = shadowmapResDesc.Height = ssize;
	shadowmapResDesc.Format = DXGI_FORMAT_R32_TYPELESS;//D32_FLOATだと後で使いにくいので 
	shadowmapResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	shadowmapResDesc.DepthOrArraySize = 1;
	shadowmapResDesc.MipLevels = 1;
	shadowmapResDesc.SampleDesc.Count = 1;
	shadowmapResDesc.SampleDesc.Quality = 0;
	shadowmapResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES shadowHeapProperties = {};
	shadowHeapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	shadowHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	shadowHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	shadowHeapProperties.CreationNodeMask = 1;
	shadowHeapProperties.VisibleNodeMask = 1;

	//深度バッファを作成
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	device->CreateCommittedResource(&shadowHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&shadowmapResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(shadowBuffer.GetAddressOf()));

	//DepthStencilViewHeapの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(shadowBuffer.Get(), &dsvDesc, shadowHeapDSV->GetCPUDescriptorHandleForHeapStart());

	//ShaderResourceViewHeapの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowBuffer.Get(), &srvDesc, shadowHeapSRV->GetCPUDescriptorHandleForHeapStart());
}

void 
DX12Init::CreateShadowRootSgnature() {
	HRESULT result;
	rootsignater["Shadow"] = std::make_shared<RootSignature>();
	rootsignater["Shadow"]->SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0);
	rootsignater["Shadow"]->SetTableRange(1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1);

	rootsignater["Shadow"]->CreateRootSignature(device.Get());


	ComPtr<ID3DBlob> shadowVertexShader;//頂点シェーダー
	ComPtr<ID3DBlob> shadowPixelShader;//ピクセルシェーダー

	//頂点レイアウトの定義
	std::vector<D3D12_INPUT_ELEMENT_DESC> shadowInput = {
		//頂点
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "BONENO",0,DXGI_FORMAT_R16G16_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R8_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	piplineStates["Shadow"] = std::make_shared<PiplineState>();
	piplineStates["Shadow"]->CompileShader(_T("lightView.hlsl"), "BasicVS", "BasicPS");
	piplineStates["Shadow"]->SetInputLayout(shadowInput);
	piplineStates["Shadow"]->CreatePiplineState(device.Get(), rootsignater["Shadow"]->GetRootSignature());
	


}
//深度バッファを書き込みます
void DX12Init::DrawLightView() {
	
	_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), piplineStates["Shadow"]->GetPiplineState());
	_commandList->SetGraphicsRootSignature(rootsignater["Shadow"]->GetRootSignature());

	_commandList->OMSetRenderTargets(0, nullptr, false, &shadowHeapDSV->GetCPUDescriptorHandleForHeapStart());
	_commandList->ClearDepthStencilView(shadowHeapDSV->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0f, 0, nullptr);
	
	//ビューポートをセット
	D3D12_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = shadowBuffer->GetDesc().Width;
	viewPort.Height = shadowBuffer->GetDesc().Height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	_commandList->RSSetViewports(1, &viewPort);
	//シザーをセット
	const D3D12_RECT rect = { 0,0, shadowBuffer->GetDesc().Width,  shadowBuffer->GetDesc().Height };
	_commandList->RSSetScissorRects(1, &rect);

	_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//回転情報・カメラ情報をシェーダに渡す
	_commandList->SetDescriptorHeaps(1, registerDescHeap.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(0, registerDescHeap->GetGPUDescriptorHandleForHeapStart());
	_commandList->SetDescriptorHeaps(1, boneHeap.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(1, boneHeap->GetGPUDescriptorHandleForHeapStart());
	//モデルの情報を入れる
	_commandList->IASetVertexBuffers(0, 1, &vbView);
	_commandList->IASetIndexBuffer(&indexView);
	_commandList->DrawIndexedInstanced(model->GetIndices().size(), 1, 0, 0, 0);

}

void
DX12Init::CreateModelDrawBundle() {
	auto result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(bundleAllocator.GetAddressOf()));
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, bundleAllocator.Get(), nullptr, IID_PPV_ARGS(bundleList.GetAddressOf()));
	
	//ルートシグネチャのセット
	bundleList->SetGraphicsRootSignature(rootsignater["1Path"]->GetRootSignature());

	//パイプラインのセット
	bundleList->SetPipelineState(piplineStates["1Path"]->GetPiplineState());

	//三角ポリゴン描画にする
	bundleList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//頂点バッファのセット
	bundleList->IASetVertexBuffers(0, 1, &vbView);
	bundleList->IASetIndexBuffer(&indexView);

	//ボーンの回転情報
	bundleList->SetDescriptorHeaps(1, boneHeap.GetAddressOf());
	bundleList->SetGraphicsRootDescriptorTable(2, boneHeap->GetGPUDescriptorHandleForHeapStart());

	//シェーダリソースビュー用のデスクリプタをセット
	bundleList->SetDescriptorHeaps(1, registerDescHeap.GetAddressOf());
	bundleList->SetGraphicsRootDescriptorTable(0, registerDescHeap->GetGPUDescriptorHandleForHeapStart());

	//頂点描画
	unsigned int offset = 0;
	auto handle = materialDescHeap->GetGPUDescriptorHandleForHeapStart();
	const auto increment_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	bundleList->SetDescriptorHeaps(1, materialDescHeap.GetAddressOf());
	for (auto& mat : model->GetMaterials()) {
		bundleList->SetGraphicsRootDescriptorTable(1, handle);
		handle.ptr += increment_size * 5;
		bundleList->DrawIndexedInstanced(mat.indices, 1, offset, 0, 0);
		offset += mat.indices;
	}

	bundleList->Close();
}

void
DX12Init::Initialize() {
	CreateDevice();/*)) {
		MessageBox(nullptr, L"Error", L"デバイス作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	CreateCommand();/*)) {
		MessageBox(nullptr, L"Error", L"コマンド作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	CreateFence();/*)) {
		MessageBox(nullptr, L"Error", L"フェンス作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	CreateSwapChain();/*)) {
		MessageBox(nullptr, L"Error", L"スワップチェイン作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	CreateRenderTarget();/*)) {
		MessageBox(nullptr, L"Error", L"レンダーターゲット作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	Create1stPathRTVSRV();
	Create2ndPathRTVSRV();
	CreateRootSgnature();/*)) {
		MessageBox(nullptr, L"Error", L"ルートシグネチャー作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	Create1stPathRootSgnature();
	Create2ndPathRootSgnature();
	CreateVertex();/*)) {
		MessageBox(nullptr, L"Error", L"Vertex作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	CreateIndeis();/*)) {
		MessageBox(nullptr, L"Error", L"Index作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	Create1stPathCanvasPorigonn();
	Create2ndPathCanvasPorigonn();
	CreateModelTextures();
	CreateWhiteTexBuffer();
	CreateBlackTexBuffer();
	CretaeToonTexture();
	CreateMaterialBuffer();
	CreateBone();
	CreateBonesBuffer();
	CreateDepth();/*)) {
		MessageBox(nullptr, L"Error", L"深度設定作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	CreateShadowMap();
	CreateShadowRootSgnature();
	CreateShader();/*)) {
		MessageBox(nullptr, L"Error", L"シェーダー作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/
	CreateConstantBuffer();/*)) {
		MessageBox(nullptr, L"Error", L"カメラ作成に失敗しました", MB_OK | MB_ICONEXCLAMATION);
	}*/

	CreateModelDrawBundle();
	primitive.reset(new PrimitiveCreator(device.Get()));
	primitive->Init();
	mPlane->VertexBuffer(device.Get());
	mCylinder->VertexBuffer(device.Get());
	mCube->VertexBuffer(device.Get());
	//CCDIK();
	//return result;
}

void
DX12Init::ResourceBarrier(std::vector<ID3D12Resource*> recource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(recource[swapChain->GetCurrentBackBufferIndex()], before, after));
	//return S_OK;
}

void
DX12Init::Wait() {
	static UINT64 frames_ = 0;
	const UINT64 fenceValue = frames_;

	_commandQueue->Signal(fence.Get(), frames_);
	++frames_;

	while (fence->GetCompletedValue() < fenceValue) {
		fence->SetEventOnCompletion(fenceValue, fenceEvent);

	}


	//return S_OK;
}

void 
DX12Init::InputMove() {
	static XMFLOAT3 rotation(0.0f, 0.0f, 0.0f);
	unsigned char keystate[256];
	if (GetKeyboardState(keystate)) {
		if (keystate['Z'] & 0x80) {
			rotation.y += 0.01f;
		}
		if (keystate['X'] & 0x80) {
			rotation.y -= 0.01f;
		}
		/*if (keystate['W'] & 0x80) {
			rotation.x += 0.01f;
		}
		if (keystate['S'] & 0x80) {
			rotation.x -= 0.01f;
		}*/
		if (keystate[VK_UP] & 0x80) {
			eye.y -= 0.1f;
			target.y -= 0.1f;
		}
		if (keystate[VK_LEFT] & 0x80) {
			eye.x += 0.1f;
			target.x += 0.1f;
		}
		if (keystate[VK_RIGHT] & 0x80) {
			eye.x -= 0.1f;
			target.x -= 0.1f;
		}
		if (keystate[VK_DOWN] & 0x80) {
			eye.y += 0.1f;
			target.y += 0.1f;
		}
		if (keystate[VK_SHIFT] & 0x80) {
			eye.z += 0.1f;
		}
		if (keystate[VK_CONTROL] & 0x80) {
			eye.z -= 0.1f;
		}

		/*auto v = SoundManager::GetInstance().GetVolumeBGM();
		if (keystate['Q'] & 0x80) {
			SoundManager::GetInstance().SetVolumeBGM(v + 0.05f);
		}else if (keystate['A'] & 0x80) {
			SoundManager::GetInstance().SetVolumeBGM(v - 0.05f);
		}*/
		
	}

	XMMATRIX world = XMMatrixRotationY(rotation.y)*XMMatrixRotationX(rotation.x);
	auto up = XMFLOAT3(0, 1, 0);

	camera = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matrixAddress->world = world;
	matrixAddress->viewproj = camera * projection;
	matrixAddress->eye = { eye.x,eye.y,eye.z,1.0f };
}

void
DX12Init::SetViewAndScissor(unsigned int bbindex, ID3D12DescriptorHeap* heap) {
	//ビューポートをセット
	D3D12_VIEWPORT viewPort = {};
	viewPort.TopLeftX =0;
	viewPort.TopLeftY = 0;
	viewPort.Width = WIN_WIDTH;
	viewPort.Height = WIN_HEIGTH;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	_commandList->RSSetViewports(1, &viewPort);


	//シザーをセット
	const D3D12_RECT rect = {0,0, WIN_WIDTH, WIN_HEIGTH };
	_commandList->RSSetScissorRects(1, &rect);
}

void
DX12Init::Draw() {
	//CCDIK();
	//RecursiveMatrixMultiply(boneMap["センター"], boneMatrices[0]);
	//memcpy(mappedBones, &boneMatrices[0], boneMatrices.size() * sizeof(XMMATRIX));
	static unsigned int bbIndex = 0;
	//_commandAllocator->Reset();
	//_commandList->Reset(_commandAllocator.Get(), pipelineState.Get());
	InputMove();
	static int frameno = 0;
	
	MotionUpdate(frameno);

	frameno++/2.0f;

	DrawLightView();

	_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), pipelineState.Get());

	//_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(recource[swapChain->GetCurrentBackBufferIndex()], before, after));
	//_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_1stPathBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	SetViewAndScissor(bbIndex, _1stHeapRTV.Get());

	static const float color[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	_commandList->OMSetRenderTargets(1, &_1stHeapRTV->GetCPUDescriptorHandleForHeapStart(), false, &_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	_commandList->ClearDepthStencilView(_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0f, 0, nullptr);
	_commandList->ClearRenderTargetView(_1stHeapRTV->GetCPUDescriptorHandleForHeapStart(), color, 0, nullptr);
	//buffer->Update(_commandList);
	
	_commandList->SetDescriptorHeaps(1, materialDescHeap.GetAddressOf());
	_commandList->ExecuteBundle(bundleList.Get());

	//3Dオブジェクト(Primitive)の描画
	primitive->SetPrimitiveDrawMode(_commandList.Get());
	//カメラの情報
	_commandList->SetDescriptorHeaps(1, registerDescHeap.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(0, registerDescHeap->GetGPUDescriptorHandleForHeapStart());
	//正射影のテクスチャ
	_commandList->SetDescriptorHeaps(1, shadowHeapSRV.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(1, shadowHeapSRV->GetGPUDescriptorHandleForHeapStart());
	//描画
	mPlane->Draw(_commandList.Get());//床
	mCylinder->Draw(_commandList.Get());//円柱



	//mCube->Draw(_commandList.Get());

	//primitive->Draw(_commandList.Get());
	//_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_1stPathBuffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PRESENT));
	//HRESULT result = _commandList->Close();
	//ID3D12CommandList* commandList[] = { _commandList.Get() };
	//_commandQueue->ExecuteCommandLists(_countof(commandList), commandList);

	//Wait();
	//-----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), _1stPathPipelineState.Get());
	//_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_2ndPathBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	SetViewAndScissor(bbIndex, _1stHeapRTV.Get());

	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), bbIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	//_commandList->OMSetRenderTargets(1, &rtvHandle, false, &_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	//_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

	_commandList->OMSetRenderTargets(1, &_2ndHeapRTV->GetCPUDescriptorHandleForHeapStart(), false, &_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	_commandList->ClearRenderTargetView(_2ndHeapRTV->GetCPUDescriptorHandleForHeapStart(), color, 0, nullptr);

	_commandList->SetGraphicsRootSignature(rootsignater["2Path"]->GetRootSignature());
	_commandList->SetPipelineState(piplineStates["2Path"]->GetPiplineState());

	_commandList->SetDescriptorHeaps(1, _1stHeapSRV.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(0, _1stHeapSRV->GetGPUDescriptorHandleForHeapStart());

	_commandList->SetDescriptorHeaps(1, gauss1stDescHeap.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(1, gauss1stDescHeap->GetGPUDescriptorHandleForHeapStart());

	_commandList->SetDescriptorHeaps(1, _1stHeapDSV.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(2, _1stHeapDSV->GetGPUDescriptorHandleForHeapStart());

	//4角ポリゴン描画にする
	_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_commandList->IASetVertexBuffers(0, 1, &_1stCanvasView);
	_commandList->DrawInstanced(4, 1, 0, 0);

	//_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_2ndPathBuffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PRESENT));

	/*_commandList->Close();
	ID3D12CommandList* commandLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	Wait();*/
	//-----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	//2Path
	//-----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), _2ndPathPipelineState.Get());
	ResourceBarrier(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	SetViewAndScissor(bbIndex, _2ndHeapRTV.Get());
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), bbIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	_commandList->OMSetRenderTargets(1, &rtvHandle, false, &_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

	_commandList->SetGraphicsRootSignature(rootsignater["3Path"]->GetRootSignature());
	_commandList->SetPipelineState(_2ndPathPipelineState.Get());


	_commandList->SetDescriptorHeaps(1, _2ndHeapSRV.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(0, _2ndHeapSRV->GetGPUDescriptorHandleForHeapStart());

	_commandList->SetDescriptorHeaps(1, gauss2ndDescHeap.GetAddressOf());
	_commandList->SetGraphicsRootDescriptorTable(1, gauss2ndDescHeap->GetGPUDescriptorHandleForHeapStart());

	//三角ポリゴン描画にする
	_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_commandList->IASetVertexBuffers(0, 1, &_2ndCanvasView);
	_commandList->DrawInstanced(4, 1, 0, 0);

	ResourceBarrier(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	_commandList->Close();
	ID3D12CommandList* command2ndLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(command2ndLists), command2ndLists);

	Wait();

	//-----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------




	swapChain->Present(1, 0);
	///*if (FAILED(result))
	//{*/
	//	/*result = */device->GetDeviceRemovedReason();
	////}
	bbIndex = swapChain->GetCurrentBackBufferIndex();
	//device->GetDeviceRemovedReason();


	//SoundManager::GetInstance().ExecuteMain();
}

ID3D12Device*
DX12Init::GetDevice() {
	return device.Get();
}

ID3D12GraphicsCommandList*
DX12Init::GetList() {
	return _commandList.Get();
}

