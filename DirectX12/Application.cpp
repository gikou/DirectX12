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
#include <wrl/client.h>

#include"DX12Init.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//���_�\����
struct Vertex {
	DirectX::XMFLOAT3 pos;//���W
	DirectX::XMFLOAT2 uv;//uv���W
};

using namespace Microsoft::WRL;
using namespace DirectX;

#define WIN_WIDTH	(640)	//�E�B���h�E�T�C�Y��
#define WIN_HEIGTH	(480)	//�E�B���h�E�T�C�Y��

#define Release(X)	{if((X) != nullptr) (X)->Release();}

Application::Application()
{
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);//Windows��DLL�ɓ��������鏀��
}


Application::~Application()
{
	CoUninitialize();
}

bool
Application::Run(HWND hwnd) {


	ShowWindow(hwnd, SW_SHOW);

	HRESULT result = S_OK;
	//�G���[���o�͂ɕ\��������
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (FAILED(result))
		int i = 0;
	debug->EnableDebugLayer();
	//Release(debug);
	debug = nullptr;
#endif

	ID3D12Device* device = nullptr;
	std::shared_ptr<DX12Init> dx12;
	dx12.reset(new DX12Init(hwnd, device));
	dx12->Initialize();
	device = dx12->GetDevice();
	//���[�v
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {//OS���瓊�����Ă郁�b�Z�[�W��msg�Ɋi�[
			TranslateMessage(&msg);	//���z�L�[�֘A�̕ϊ�
			DispatchMessage(&msg);	//��������Ȃ��������b�Z�[�W��OS�ɕԂ�
		}
		dx12->Draw();
	}
	dx12.reset();

	ID3D12DebugDevice* debugInterface = nullptr;
	result = device->QueryInterface(&debugInterface);
	debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	Release(debugInterface);

	return true;
}
