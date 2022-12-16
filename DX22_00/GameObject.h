#pragma once

#include "Model.h"

// このクラスからいろいろなモノを派生で作る
// ・モデル表示機能
// ・向いている方向に進む機能
// ・前向きベクトルを返す機能
class GameObject
{

public:
	GameObject();
	~GameObject();
	virtual void Draw();
	virtual void Update(); // 継承先でオーバーライドする前提

	// このクラスの表示に使われるModelオブジェクトを返す
	Model* GetModel();

	// 前向きベクトルを返す関数
	DirectX::XMFLOAT3 GetForwardVector();

	//渡された位置との距離を測る関数
	float GetDistaceVector(DirectX::XMFLOAT3 pOther);

	// 移動速度
	float mSpeed;

protected:
	// このクラスで表示させるモデル
	Model* mModel;

private:
	// 最初の前向きベクトルを保存する変数
	DirectX::XMFLOAT3 mDefaultForwardVector;

	// 前向きベクトルを保存しておく変数
	DirectX::XMFLOAT3 mForwardVector;

	//他のGameObjectとの距離を保存する変数
	float mDistanceVector;

};

