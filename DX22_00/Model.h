#pragma once

#include "ModelData.h"
#include "Direct3D.h"
#include "Camera.h"
#include <DirectXMath.h>

// 3Dモデルを表示させるクラス
class Model
{
protected:
	DIRECT3D *d3d;

	// 仮想世界（ワールド）上の座標
	DirectX::XMFLOAT3 mPos;
	// 表示回転角度
	DirectX::XMFLOAT3 mRotate;
	// 表示拡大縮小率
	DirectX::XMFLOAT3 mScale;

	ModelData mModelData; // 3Dデータ
	
	UINT sampleMask;

	// 定数バッファの転送用
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

	virtual void Draw(); // 表示

	// このクラスで表示する3Dデータをセットする
	void SetModelData(ModelData model);

	// このモデルに適用するカメラオブジェクト
	Camera* mCamera;

	// ブレンドステート設定用関数
	void AddBlendState();	// 加算合成
	void SubBlendState();	// 減算合成
	void AlphaBlendState();	// アルファブレンディング

	void GetBlendState();	// 現在のブレンドステートを保存する
	void BackBlendState();	// 保存していたブレンドステートに戻す

	// Zバッファ切り替え
	void DisableZBuffer();	// Zバッファを無効化する
	void EnableZBuffer();	// Zバッファを有効化する
	

	// 座標設定
	void SetPos(float PosX, float PosY, float PosZ);
	void SetPos(DirectX::XMFLOAT3 newPos);
	void SetPosX(float PosX);			//X
	void SetPosY(float PosY);			//Y
	void SetPosZ(float PosZ);			//Z
	// 角度設定
	void SetRot(float RotX, float RotY, float RotZ);
	void SetRot(DirectX::XMFLOAT3 newRot);
	void SetRotX(float RotX);				//X
	void SetRotY(float RotY);				//Y
	void SetRotZ(float RotZ);				//Z
	// 拡大縮小率のxyzを同じ値で設定する
	void SetScale(float newScale);
	void SetScale(float ScaleX, float ScaleY, float ScaleZ);
	void SetScaleX(float ScaleX);				//X
	void SetScaleY(float ScaleY);				//Y
	void SetScaleZ(float ScaleZ);				//Z

	// 座標移動
	void AddPos(float PosX, float PosY, float PosZ);
	// 回転
	void AddRot(float RotX, float RotY, float RotZ);
	// 均一拡大
	void AddScale(float addScale);
	// 指定拡大
	void AddScale(float ScaleX, float ScaleY, float ScaleZ);

	// 座標取得
	DirectX::XMFLOAT3 GetPos();	// 全要素取得
	float GetPosX();			// X
	float GetPosY();			// Y
	float GetPosZ();			// Z
	// 角度取得
	DirectX::XMFLOAT3 GetRot();		// 全要素取得
	float GetRotX();				// X
	float GetRotY();				// Y
	float GetRotZ();				// Z
	// 大きさ取得
	DirectX::XMFLOAT3 GetScale();	// 全要素取得
	float GetScaleX();				// X
	float GetScaleY();				// Y
	float GetScaleZ();				// Z
};

