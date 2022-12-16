#include "Shadow.h"
#include"Direct3D.h"

Shadow::Shadow(GameObject * pOwner)
{
	mOwner = pOwner;
}

Shadow::~Shadow()
{

}

void Shadow::Update()
{
	this->GetModel()->SetPos(mOwner->GetModel()->GetPos());

	this->GetModel()->SetPosY(0.0f);
}

void Shadow::Draw()
{
	//影がちらつくのを防ぐためZバッファを無効にする
	DIRECT3D* d3d = Direct3D_Get();
	UINT sampleMask = 0xffffffff;
	this->mModel->DisableZBuffer();	//Zバッファ無効

	//現在設定されているブレンドステートを取得する関数
	this->mModel->GetBlendState();

	// 減算合成を適用する
	this->mModel->SubBlendState();

	GameObject::Draw();
	
	//ブレンドステートをもとに戻す
	this->mModel->BackBlendState();

	//Zバッファを有効に戻す
	this->mModel->EnableZBuffer();	//Zバッファ無効
}


