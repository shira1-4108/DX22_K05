#pragma once

#include "ModelData.h"
#include "Direct3D.h"
#include "Camera.h"
#include <DirectXMath.h>

// 3D���f����\��������N���X
class Model
{
protected:
	DIRECT3D *d3d;

	// ���z���E�i���[���h�j��̍��W
	DirectX::XMFLOAT3 mPos;
	// �\����]�p�x
	DirectX::XMFLOAT3 mRotate;
	// �\���g��k����
	DirectX::XMFLOAT3 mScale;

	ModelData mModelData; // 3D�f�[�^
	
	UINT sampleMask;

	// �萔�o�b�t�@�̓]���p
	struct ConstBufferData
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public:
	enum BlendState {
		Add,
		Sub,
		Alpha,
		Back,
	};

	Model();

	virtual void Draw(); // �\��

	// ���̃N���X�ŕ\������3D�f�[�^���Z�b�g����
	void SetModelData(ModelData model);

	// ���̃��f���ɓK�p����J�����I�u�W�F�N�g
	Camera* mCamera;

	// �u�����h�X�e�[�g�ݒ�p�֐�
	void AddBlendState();	// ���Z����
	void SubBlendState();	// ���Z����
	void AlphaBlendState();	// �A���t�@�u�����f�B���O

	void GetBlendState();	// ���݂̃u�����h�X�e�[�g��ۑ�����
	void BackBlendState();	// �ۑ����Ă����u�����h�X�e�[�g�ɖ߂�

	// Z�o�b�t�@�؂�ւ�
	void DisableZBuffer();	// Z�o�b�t�@�𖳌�������
	void EnableZBuffer();	// Z�o�b�t�@��L��������
	

	// ���W�ݒ�
	void SetPos(float PosX, float PosY, float PosZ);
	void SetPos(DirectX::XMFLOAT3 newPos);
	void SetPosX(float PosX);			//X
	void SetPosY(float PosY);			//Y
	void SetPosZ(float PosZ);			//Z
	// �p�x�ݒ�
	void SetRot(float RotX, float RotY, float RotZ);
	void SetRot(DirectX::XMFLOAT3 newRot);
	void SetRotX(float RotX);				//X
	void SetRotY(float RotY);				//Y
	void SetRotZ(float RotZ);				//Z
	// �g��k������xyz�𓯂��l�Őݒ肷��
	void SetScale(float newScale);
	void SetScale(float ScaleX, float ScaleY, float ScaleZ);
	void SetScaleX(float ScaleX);				//X
	void SetScaleY(float ScaleY);				//Y
	void SetScaleZ(float ScaleZ);				//Z

	// ���W�ړ�
	void AddPos(float PosX, float PosY, float PosZ);
	// ��]
	void AddRot(float RotX, float RotY, float RotZ);
	// �ψ�g��
	void AddScale(float addScale);
	// �w��g��
	void AddScale(float ScaleX, float ScaleY, float ScaleZ);

	// ���W�擾
	DirectX::XMFLOAT3 GetPos();	// �S�v�f�擾
	float GetPosX();			// X
	float GetPosY();			// Y
	float GetPosZ();			// Z
	// �p�x�擾
	DirectX::XMFLOAT3 GetRot();		// �S�v�f�擾
	float GetRotX();				// X
	float GetRotY();				// Y
	float GetRotZ();				// Z
	// �傫���擾
	DirectX::XMFLOAT3 GetScale();	// �S�v�f�擾
	float GetScaleX();				// X
	float GetScaleY();				// Y
	float GetScaleZ();				// Z
};

