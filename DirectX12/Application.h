#pragma once
#include <Windows.h>	//�E�B���h�E�\������̂ɕK�v

class Application
{
public:
	Application();//�����֎~
	Application(const Application&) {};//�R�s�[�֎~
	void operator=(const Application&) {};//����֎~
public:
	~Application();
	static Application& Instance() {
		static Application instance;
		return instance;
	}
	bool Run(HWND hwnd);
	void Delete();
};

