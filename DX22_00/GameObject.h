#pragma once

#include "Model.h"

// ���̃N���X���炢�낢��ȃ��m��h���ō��
// �E���f���\���@�\
// �E�����Ă�������ɐi�ދ@�\
// �E�O�����x�N�g����Ԃ��@�\
class GameObject
{

public:
	GameObject();
	~GameObject();
	virtual void Draw();
	virtual void Update(); // �p����ŃI�[�o�[���C�h����O��

	// ���̃N���X�̕\���Ɏg����Model�I�u�W�F�N�g��Ԃ�
	Model* GetModel();

	// �O�����x�N�g����Ԃ��֐�
	DirectX::XMFLOAT3 GetForwardVector();

	//�n���ꂽ�ʒu�Ƃ̋����𑪂�֐�
	float GetDistaceVector(DirectX::XMFLOAT3 pOther);

	// �ړ����x
	float mSpeed;

protected:
	// ���̃N���X�ŕ\�������郂�f��
	Model* mModel;

private:
	// �ŏ��̑O�����x�N�g����ۑ�����ϐ�
	DirectX::XMFLOAT3 mDefaultForwardVector;

	// �O�����x�N�g����ۑ����Ă����ϐ�
	DirectX::XMFLOAT3 mForwardVector;

	//����GameObject�Ƃ̋�����ۑ�����ϐ�
	float mDistanceVector;

};

