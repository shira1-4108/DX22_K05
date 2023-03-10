#pragma once
#include"NormalObject.h"

//丸影を表示するクラス
class Shadow : public NormalObject 
{
private:
	GameObject* mOwner;
public:
	Shadow() = delete;
	Shadow(GameObject* pOwner);
	~Shadow();
	void Update() override;
	void Draw() override;
};

