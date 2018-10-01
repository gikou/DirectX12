#pragma once
class Dx12Init;
class Dx12TextureBuffer;
class Dx12ConstantBuffer;
struct ID3D12Resource;

class Dx12BufferManager
{
private:
	//定数バッファアライメントサイズを返す() 
	size_t GetConstantBufferAlignmentedSize(size_t size);
	ID3D12Resource* CreateConstantBufferResource(size_t size);
	Dx12Init& _dx;
public:
	Dx12BufferManager(Dx12Init& dx);
	~Dx12BufferManager();

	Dx12TextureBuffer* CreateTextureBuffer(const char* groupname, size_t width, size_t height);

	Dx12TextureBuffer* CreateTextureBufferFromFile(const char* groupname, const wchar_t* filepath);

	///コンスタントバッファを作成します 
	///@param groupname グループ名(ワンセットにするための識別名) 
	/*template <typename T>
	inline Dx12ConstantBuffer<T>* CreateConstantBuffer(const char* groupname) {
		auto ret = new Dx12ConstantBuffer<T>();
		ret->_buffSize = GetConstantBufferAlignmentedSize(sizeof(T));
		ret->_resource.Attach(CreateConstantBufferResource(ret->_buffSize));
		ret->_mappedAddress = static_cast<T*>(ret->Map());
		return ret;
	}*/
};