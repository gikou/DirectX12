#pragma once
#include <wrl/client.h>
#include<memory>
using namespace Microsoft::WRL;
//�O���錾 
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
	//�v���~�e�B�u�p�̃p�C�v���C���X�e�[�g 
	ComPtr<ID3D12PipelineState> _pipelineState;
	std::shared_ptr<Plane> plane;
	ID3D12Device* device;
public:
	PrimitiveCreator(ID3D12Device* dev);
	~PrimitiveCreator();
	///���̊֐����Ńv���~�e�B�u�p�̃��C�A�E�g�A�V�F�[�_ 
	///�p�C�v���C���X�e�[�g������������܂� 
	void Init();

	///�v���~�e�B�u�p�̃p�C�v���C���X�e�[�g�������ŃZ�b�g����܂� 
	///Primitive�n��Draw���߂��o���O�ɕK���Ă�ł������� 
	void SetPrimitiveDrawMode(ID3D12GraphicsCommandList* cmdlist);

	///���ʃI�u�W�F�N�g����� 
	///@param width ���ʂ̕� 
	///@param depth ���ʂ̉��s�� 
	///@param y ���ʂ�Y���W 
	PrimitiveObject* CreatePlane(float width, float depth, float y);

	///�~���I�u�W�F�N�g����� 
	///@param radius ���a 
	///@param height ���� 
	///@param div ������ 
	PrimitiveObject* CreateCylinder(float radius, float height, unsigned int div);

	void Draw(ID3D12GraphicsCommandList* cmdlist);
};
