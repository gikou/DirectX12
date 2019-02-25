#pragma once
#include <wrl/client.h>
#include<memory>
using namespace Microsoft::WRL;
//�O���錾 
class PrimitiveObject;
class ID3D12Device;
class ID3D12GraphicsCommandList;
struct ID3D12PipelineState;
class RootSignature;
class PiplineState;
class Plane;
class Cube;
class Cylinder;
class PrimitiveCreator
{
private:
	std::shared_ptr<RootSignature> mRootSignature;
	//�v���~�e�B�u�p�̃p�C�v���C���X�e�[�g 
	std::shared_ptr<PiplineState> mPipelineState;
	std::shared_ptr<Plane> mPlaneMesh;
	std::shared_ptr<Cube> mCubeMesh;
	std::shared_ptr<Cylinder> mCylinderMesh;
	ID3D12Device* mDevice;
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
