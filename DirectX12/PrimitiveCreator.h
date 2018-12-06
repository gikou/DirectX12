#pragma once
#include <wrl/client.h>
#include<memory>
using namespace Microsoft::WRL;
//前方宣言 
class PrimitiveObject;
class ID3D12Device;
class ID3D12GraphicsCommandList;
struct ID3D12PipelineState;
struct ID3D12RootSignature;
class Plane;
class PrimitiveCreator
{
private:
	ComPtr<ID3D12RootSignature> rootSignature;
	//プリミティブ用のパイプラインステート 
	ComPtr<ID3D12PipelineState> _pipelineState;
	std::shared_ptr<Plane> plane;
	ID3D12Device* device;
public:
	PrimitiveCreator(ID3D12Device* dev);
	~PrimitiveCreator();
	///この関数内でプリミティブ用のレイアウト、シェーダ 
	///パイプラインステートが初期化されます 
	void Init();

	///プリミティブ用のパイプラインステートが内部でセットされます 
	///Primitive系のDraw命令を出す前に必ず呼んでください 
	void SetPrimitiveDrawMode(ID3D12GraphicsCommandList* cmdlist);

	///平面オブジェクトを作る 
	///@param width 平面の幅 
	///@param depth 平面の奥行き 
	///@param y 平面のY座標 
	PrimitiveObject* CreatePlane(float width, float depth, float y);

	///円柱オブジェクトを作る 
	///@param radius 半径 
	///@param height 高さ 
	///@param div 分割数 
	PrimitiveObject* CreateCylinder(float radius, float height, unsigned int div);

	void Draw(ID3D12GraphicsCommandList* cmdlist);
};
