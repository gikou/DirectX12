#include <Windows.h>	//�E�B���h�E�\������̂ɕK�v
#include "Application.h"
#include <tchar.h>


//�v���g�^�C�v�錾
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DirectX12");//�A�v���P�[�V�����N���X��
	w.hInstance = GetModuleHandle(0);//�n���h���̎擾
	RegisterClassEx(&w);//�A�v���P�[�V�����N���X

						//�E�B���h�E�T�C�Y�ݒ�
	RECT wrc = { 0, 0, 640, 480 };//�E�B���h�E�T�C�Y��ݒ�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�␳

													   //�E�B���h�E����
	HWND hwnd = CreateWindow(w.lpszClassName,//�N���X���w��
		_T("DirectX12"),			//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,		//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,			//�\��X���W��OS�ɂ��C��
		CW_USEDEFAULT,			//�\��Y���W��OS�ɂ��C��
		wrc.right - wrc.left,		//�E�B���h�E��
		wrc.bottom - wrc.top,		//�E�B���h�E��
		nullptr,					//�e�E�B���h�E�̃n���h��
		nullptr,					//���j���[�n���h��
		w.hInstance,				//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);				//�ǉ��p�����[�^

								//�E�B���h�E�\��
	if (!Application::Instance().Run(hwnd)) {
		return 0;
	}
	Application::Instance().Delete();
}

//�E�B���h�E�v���V�[�W��
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {	//�E�B���h�E���j�����ꂽ��Ă΂��
		PostQuitMessage(0);	//OS�ɑ΂��ăA�v���̏I����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�K��̏������s��
}
