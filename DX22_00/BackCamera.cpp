#include "BackCamera.h"

using namespace DirectX;

void BackCamera::Update()
{
	// カメラ追従処理
	// ①操作キャラの前向きベクトルを取ってくる
	XMFLOAT3 forwardVec = mTarget->GetForwardVector();
	// ②その前向きベクトルを反転して、後ろ向きベクトルを作る
	XMFLOAT3 backVec;
	backVec.x = -forwardVec.x;
	backVec.z = -forwardVec.z;
	backVec.y = -forwardVec.y;
	// ③後ろ向きベクトルを使って、キャラの後ろにカメラを置く
	XMFLOAT3 camEye;
	Model* pTargetModel = mTarget->GetModel();
	camEye.x = pTargetModel->GetPosX() + backVec.x * 2.0f;
	camEye.y = pTargetModel->GetPosY() + backVec.y * 2.0f + 1.0f;
	camEye.z = pTargetModel->GetPosZ() + backVec.z * 2.0f;
	// 緩やかカメラの処理
	// 1フレーム前のカメラ位置を保存する変数
	static XMFLOAT3 lastCamEye;
	float blendFactor = 0.997f;
	camEye.x = lastCamEye.x * blendFactor
		+ camEye.x * (1.0f - blendFactor);
	camEye.y = lastCamEye.y * blendFactor
		+ camEye.y * (1.0f - blendFactor);
	camEye.z = lastCamEye.z * blendFactor
		+ camEye.z * (1.0f - blendFactor);
	this->SetEye(camEye);
	lastCamEye = camEye; // 次のループのためにカメラ位置保存
	// ④カメラ注視点を設定
	// 操作キャラの少し前を注視点にする
	XMFLOAT3 camFocus;
	camFocus.x = pTargetModel->GetPosX() + forwardVec.x;
	camFocus.y = pTargetModel->GetPosY() + forwardVec.y;
	camFocus.z = pTargetModel->GetPosZ() + forwardVec.z;

	this->SetFocus(camFocus);

	// ビュー変換行列計算
	Camera::Update();
}

void BackCamera::SetTarget(GameObject * pTarget)
{
	mTarget = pTarget;
}
