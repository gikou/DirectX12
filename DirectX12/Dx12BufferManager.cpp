#include "Dx12BufferManager.h"
#include"DX12Init.h"
#include"Dx12TextureBufffer.h"

Dx12BufferManager::Dx12BufferManager(Dx12Init& dx):_dx(dx)
{
}

Dx12BufferManager::~Dx12BufferManager()
{
}

size_t 
Dx12BufferManager::GetConstantBufferAlignmentedSize(size_t size) {
	return size;
}

ID3D12Resource* 
Dx12BufferManager::CreateConstantBufferResource(size_t size) {
	return nullptr;
}

Dx12TextureBuffer* 
Dx12BufferManager::CreateTextureBuffer(const char* groupname, size_t width, size_t height) {
	return nullptr;
}

Dx12TextureBuffer* 
Dx12BufferManager::CreateTextureBufferFromFile(const char* groupname, const wchar_t* filepath) {
	return nullptr;
}
