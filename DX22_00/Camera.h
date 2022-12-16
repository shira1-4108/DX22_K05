#pragma once

#include <DirectXMath.h>

// カメラを表すクラス
class Camera
{

public:
	// カメラの位置を設定する
	void SetEye(DirectX::XMFLOAT3 newEye);

	// カメラの見ている場所を設定する
	void SetFocus(DirectX::XMFLOAT3 newFocus);

	// カメラの上方向を設定する
	void SetUp(DirectX::XMFLOAT3 newUp);

	// 出来上がったビュー変換行列を返す
	DirectX::XMMATRIX GetViewMatrix();

	// カメラに必要な変数
	DirectX::XMFLOAT3 mEye, mFocus, mUp;

	// 更新処理（毎ループ１回呼び出す）
	virtual void Update();

private:
	// Update関数で計算したビュー変換行列を持つ
	DirectX::XMMATRIX mViewMatrix;

};

