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
	//�e��������̂�h������Z�o�b�t�@�𖳌��ɂ���
	DIRECT3D* d3d = Direct3D_Get();
	UINT sampleMask = 0xffffffff;
	this->mModel->DisableZBuffer();	//Z�o�b�t�@����

	//���ݐݒ肳��Ă���u�����h�X�e�[�g���擾����֐�
	this->mModel->GetBlendState();

	// ���Z������K�p����
	this->mModel->SubBlendState();

	GameObject::Draw();
	
	//�u�����h�X�e�[�g�����Ƃɖ߂�
	this->mModel->BackBlendState();

	//Z�o�b�t�@��L���ɖ߂�
	this->mModel->EnableZBuffer();	//Z�o�b�t�@����
}


