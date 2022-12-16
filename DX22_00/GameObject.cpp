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
	// 前向きベクトルを作る
	XMFLOAT3 forwardVector;

	float radianY = XMConvertToRadians(mModel->GetRotY() + mDefaultForwardVector.y);
	forwardVector.x = sinf(radianY);
	forwardVector.z = cosf(radianY);
	forwardVector.y = 0.0f;
	// 前向きベクトルをメンバー変数に保存
	mForwardVector = forwardVector;

	// 前向きベクトルと速度を使って前進する
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
	// (a)渡されたモデルの位置を取得する
	XMFLOAT3 otherPos = pOther;

	// (b)自分の位置を取得する
	XMFLOAT3 thisPos = this->mModel->GetPos();

	// (a)-(b)を計算して(b)→(a)ベクトルを作る
	XMVECTOR baVec = XMLoadFloat3(&otherPos) - XMLoadFloat3(&thisPos);

	// (b)→(a)ベクトルの長さを測る
	XMVECTOR baVecLength = XMVector3Length(baVec);

	// 実際の距離を取り出す
	this->mDistanceVector = XMVectorGetX(baVecLength);

	return this->mDistanceVector;
}
