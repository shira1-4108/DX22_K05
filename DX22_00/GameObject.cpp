#include "GameObject.h"

extern DWORD gDeltaTime;

using namespace DirectX;

GameObject::GameObject()
{
	mModel = nullptr;
	mSpeed = 0.0f;
	mDefaultForwardVector = {0.0f, -90.0f, 0.0f};
}

GameObject::~GameObject()
{
	delete mModel;
}

void GameObject::Draw()
{
	mModel->Draw();
}

void GameObject::Update()
{
	// �O�����x�N�g�������
	XMFLOAT3 forwardVector;

	float radianY = XMConvertToRadians(mModel->GetRotY() + mDefaultForwardVector.y);
	forwardVector.x = sinf(radianY);
	forwardVector.z = cosf(radianY);
	forwardVector.y = 0.0f;
	// �O�����x�N�g���������o�[�ϐ��ɕۑ�
	mForwardVector = forwardVector;

	// �O�����x�N�g���Ƒ��x���g���đO�i����
	mModel->AddPos(forwardVector.x * mSpeed * gDeltaTime, 0.0f, forwardVector.z * mSpeed * gDeltaTime);
}

Model * GameObject::GetModel()
{
	return mModel;
}

DirectX::XMFLOAT3 GameObject::GetForwardVector()
{
	return mForwardVector;
}

float GameObject::GetDistaceVector(DirectX::XMFLOAT3 pOther)
{
	// (a)�n���ꂽ���f���̈ʒu���擾����
	XMFLOAT3 otherPos = pOther;

	// (b)�����̈ʒu���擾����
	XMFLOAT3 thisPos = this->mModel->GetPos();

	// (a)-(b)���v�Z����(b)��(a)�x�N�g�������
	XMVECTOR baVec = XMLoadFloat3(&otherPos) - XMLoadFloat3(&thisPos);

	// (b)��(a)�x�N�g���̒����𑪂�
	XMVECTOR baVecLength = XMVector3Length(baVec);

	// ���ۂ̋��������o��
	this->mDistanceVector = XMVectorGetX(baVecLength);

	return this->mDistanceVector;
}
