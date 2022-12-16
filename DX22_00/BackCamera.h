#pragma once

#include "Camera.h"
#include "GameObject.h"

// 追従カメラを表すクラス
class BackCamera :
	public Camera
{

public:
	// 追従処理を実行
	void Update() override;

	void SetTarget(GameObject* pTarget);

private:
	// 追従対象
	GameObject* mTarget;

};

