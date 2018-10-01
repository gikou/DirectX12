#pragma once
#include <wrl/client.h>
#include "Dx12Buffer.h"
class Dx12ConstantBufferBase : public Dx12Buffer
{
	friend Dx12BufferManager;
protected:
	size_t _buffSize;
	void* Map();
public:
	Dx12ConstantBufferBase();
	virtual ~Dx12ConstantBufferBase() = 0;
	size_t GetBufferSize() { return _buffSize; }
};

