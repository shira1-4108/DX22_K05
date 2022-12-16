#pragma once

#include <DirectXMath.h>

// �J������\���N���X
class Camera
{

public:
	// �J�����̈ʒu��ݒ肷��
	void SetEye(DirectX::XMFLOAT3 newEye);

	// �J�����̌��Ă���ꏊ��ݒ肷��
	void SetFocus(DirectX::XMFLOAT3 newFocus);

	// �J�����̏������ݒ肷��
	void SetUp(DirectX::XMFLOAT3 newUp);

	// �o���オ�����r���[�ϊ��s���Ԃ�
	DirectX::XMMATRIX GetViewMatrix();

	// �J�����ɕK�v�ȕϐ�
	DirectX::XMFLOAT3 mEye, mFocus, mUp;

	// �X�V�����i�����[�v�P��Ăяo���j
	virtual void Update();

private:
	// Update�֐��Ōv�Z�����r���[�ϊ��s�������
	DirectX::XMMATRIX mViewMatrix;

};

