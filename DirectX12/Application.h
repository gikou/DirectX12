#pragma once
#include <Windows.h>	//ウィンドウ表示するのに必要

class Application
{
public:
	Application();//生成禁止
	Application(const Application&) {};//コピー禁止
	void operator=(const Application&) {};//代入禁止
public:
	~Application();
	static Application& Instance() {
		static Application instance;
		return instance;
	}
	bool Run(HWND hwnd);
	void Delete();
};

