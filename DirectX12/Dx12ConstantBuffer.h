#pragma once
#include "Dx12ConstantBufferBase.h"
template <typename T>
class Dx12ConstantBuffer:public Dx12ConstantBufferBase
{
private:

	friend Dx12BufferManager;
private:
	T* _mappedAddress;
	Dx12ConstantBuffer() {};
	Dx12ConstantBuffer(const Dx12ConstantBuffer&);
	//void operator=(const Dx12ConstantBuffer&);
public:
	~Dx12ConstantBuffer();
	void UpdateValue(T& value) {
		*_mappedAddress = value;
	}
};

