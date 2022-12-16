#pragma once
#include"NormalObject.h"

//�ۉe��\������N���X
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

