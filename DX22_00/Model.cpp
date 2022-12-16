#include "Model.h"

// �萔�o�b�t�@�p�ϐ�
ID3D11Buffer* gpConstBuffer;

using namespace DirectX; // "DirectX"namespace������ȗ�����

Model::Model()
{
	d3d = Direct3D_Get();
	sampleMask = 0xffffffff;
}

void Model::Draw()
{
	static float ry; ry += 0.005f;
	// ��]�s��
	XMMATRIX mxRotY = XMMatrixRotationY(XMConvertToRadians(mRotate.y));  // Y����]
	XMMATRIX mxRotX = XMMatrixRotationX(XMConvertToRadians(mRotate.x));
	XMMATRIX mxRotZ = XMMatrixRotationZ(XMConvertToRadians(mRotate.z));

	// ���s�ړ��s��
	XMMATRIX mxTrans = XMMatrixTranslation(mPos.x, mPos.y, mPos.z);  // ���̂̌��݈ʒu��\���ϐ����g���āA���s�ړ��̍s������

	// �g��k���s��
	XMMATRIX mxScale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	// ���[���h�ϊ��s����쐬
	XMMATRIX mxWorld = mxScale * mxRotZ * mxRotX * mxRotY * mxTrans;

	// �v���W�F�N�V�����ϊ��s����쐬
	// 1: ����p
	// 2: �A�X�y�N�g��@���@��{�A�E�C���h�E�T�C�Y�̏c����������΂悢
	// 3: near ������͈͂̈�Ԏ�O�̎��_����̋���
	// 4: far  ������͈͂̈�ԉ��̎��_����̋���
	XMMATRIX mxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	// �`��Ŏg�p����e�N�X�`�����w�肷��֐��Ăяo��
	// �����Ŏw�肳�ꂽ�e�N�X�`�����A�s�N�Z���V�F�[�_�[�̃O���[�o���ϐ��ɃZ�b�g�����
	d3d->context->PSSetShaderResources(0, 1, &mModelData.mSRV);

	// �`��Ɏg�����_�o�b�t�@���w�肷��
	UINT strides = sizeof(VERTEX3D);
	UINT offsets = 0;
	// ��O�����F���_�o�b�t�@
	d3d->context->IASetVertexBuffers(0, 1, &mModelData.mVertexBuffer, &strides, &offsets);

	// �萔�o�b�t�@�X�V�@���@�b���ꂩ��V�F�[�_�[�Ƀf�[�^��]������
	D3D11_MAPPED_SUBRESOURCE pData;  // �萔�o�b�t�@�Ŋm�ۂ��ꂽ�������̈�̏�񂪊i�[�����\����
	// �萔�o�b�t�@�̃������̈�ɃA�N�Z�X���āA�f�[�^��]�����鏈��
	// �萔�o�b�t�@�̃������ɃA�N�Z�X���邽�߂ɁA���̃������̈�Ƀ��b�N��������

	HRESULT hr = d3d->context->Map(gpConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		ConstBufferData cbdata;

		// Camera����r���[�ϊ��s����擾
		XMMATRIX mxView = mCamera->GetViewMatrix();

		cbdata.world = XMMatrixTranspose(mxWorld);
		cbdata.view = XMMatrixTranspose(mxView);
		cbdata.projection = XMMatrixTranspose(mxProjection);

		// 1: �R�s�[��������̃A�h���X
		// 2: �R�s�[��̊m�ۂ���Ă���f�[�^�T�C�Y
		// 3: �R�s�[�������f�[�^�������Ă���擪�A�h���X
		// 4: �R�s�[�������f�[�^�̃T�C�Y
		memcpy_s(pData.pData, pData.RowPitch, &cbdata, sizeof(cbdata));

		d3d->context->Unmap(gpConstBuffer, 0);  // ���b�N��������������������
	}

	// �萔�o�b�t�@���V�F�[�_�[����A�N�Z�X�ł���悤�ɃZ�b�g����
	d3d->context->VSSetConstantBuffers(1, 1, &gpConstBuffer);

	// ����܂ł̐ݒ�Ŏ��ۂɕ`�悷��  �������F���ۂɕ`�悷�钸�_��
	d3d->context->Draw(mModelData.mNumVertex, 0);
}

void Model::SetModelData(ModelData model)
{
	mModelData = model;
}

void Model::AddBlendState()
{	//���Z����
	d3d->context->OMSetBlendState(d3d->blendAdd, NULL, sampleMask);
}

void Model::SubBlendState()
{	//���Z����
	d3d->context->OMSetBlendState(d3d->blendSub, NULL, sampleMask);
}

void Model::AlphaBlendState()
{	//�A���t�@�u�����f�B���O
	d3d->context->OMSetBlendState(d3d->blendAlpha, NULL, sampleMask);
}

void Model::GetBlendState()
{	//���ݐݒ肳��Ă���u�����h�X�e�[�g���擾����
	d3d->context->OMGetBlendState(&d3d->blendNow, NULL, &sampleMask);
}

void Model::BackBlendState()
{	//�ۑ����Ă����u�����h�X�e�[�g�ɖ߂�
	d3d->context->OMSetBlendState(d3d->blendNow, NULL, sampleMask);
}

void Model::DisableZBuffer()
{
	d3d->context->OMSetRenderTargets(1, &d3d->renderTarget, NULL);	//��3������NULL�ɂ����Z�o�b�t�@����
}

void Model::EnableZBuffer()
{
	d3d->context->OMSetRenderTargets(1, &d3d->renderTarget, d3d->depthStencilView);
}

void Model::SetPos(float PosX, float PosY, float PosZ)
{
	mPos.x = PosX;
	mPos.y = PosY;
	mPos.z = PosZ;
}

void Model::SetPos(DirectX::XMFLOAT3 newPos)
{
	mPos.x = newPos.x;
	mPos.y = newPos.y;
	mPos.z = newPos.z;
}

void Model::SetPosX(float PosX)
{
	this->mPos.x = PosX;
}

void Model::SetPosY(float PosY)
{
	this->mPos.y = PosY;
}

void Model::SetPosZ(float PosZ)
{
	this->mPos.z = PosZ;
}

void Model::SetRot(float RotX, float RotY, float RotZ)
{
	mRotate.x = RotX;
	mRotate.y = RotY;
	mRotate.z = RotZ;
}

void Model::SetRot(DirectX::XMFLOAT3 newRot)
{
	mRotate.x = newRot.x;
	mRotate.y = newRot.y;
	mRotate.z = newRot.z;
}

void Model::SetRotX(float RotX)
{
	this->mRotate.x = RotX;
}

void Model::SetRotY(float RotY)
{
	this->mRotate.y = RotY;
}

void Model::SetRotZ(float RotZ)
{
	this->mRotate.z = RotZ;
}


void Model::SetScale(float newScale)
{
	mScale.x = newScale;
	mScale.y = newScale;
	mScale.z = newScale;
}

void Model::SetScale(float ScaleX, float ScaleY, float ScaleZ)
{
	this->mScale.x = ScaleX;
	this->mScale.y = ScaleY;
	this->mScale.z = ScaleZ;
}

void Model::SetScaleX(float ScaleX)
{
	this->mScale.x = ScaleX;
}

void Model::SetScaleY(float ScaleY)
{
	this->mScale.y = ScaleY;
}

void Model::SetScaleZ(float ScaleZ)
{
	this->mScale.z = ScaleZ;
}


void Model::AddPos(float PosX, float PosY, float PosZ)
{
	this->mPos.x += PosX;
	this->mPos.y += PosY;
	this->mPos.z += PosZ;
}

void Model::AddRot(float RotX, float RotY, float RotZ)
{
	this->mRotate.x += RotX;
	this->mRotate.y += RotY;
	this->mRotate.z += RotZ;
}

void Model::AddScale(float addScale)
{
	mScale.x += addScale;
	mScale.y += addScale;
	mScale.z += addScale;
}

void Model::AddScale(float ScaleX, float ScaleY, float ScaleZ)
{
	mScale.x += ScaleX;
	mScale.y += ScaleY;
	mScale.z += ScaleZ;
}

DirectX::XMFLOAT3 Model::GetPos()
{
	return this->mPos;
}

float Model::GetPosX()
{
	return this->mPos.x;
}

float Model::GetPosY()
{
	return this->mPos.y;
}

float Model::GetPosZ()
{
	return this->mPos.z;
}

DirectX::XMFLOAT3 Model::GetRot()
{
	return this->mRotate;
}

float Model::GetRotX()
{
	return this->mRotate.x;
}

float Model::GetRotY()
{
	return this->mRotate.y;
}

float Model::GetRotZ()
{
	return this->mRotate.z;
}

DirectX::XMFLOAT3 Model::GetScale()
{
	return this->mScale;
}

float Model::GetScaleX()
{
	return this->mScale.x;
}

float Model::GetScaleY()
{
	return this->mScale.y;
}

float Model::GetScaleZ()
{
	return this->mScale.z;
}
