#pragma once
#include"NormalObject.h"

//ŠÛ‰e‚ð•\Ž¦‚·‚éƒNƒ‰ƒX
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

