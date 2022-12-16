#include "Model.h"

// 定数バッファ用変数
ID3D11Buffer* gpConstBuffer;

using namespace DirectX; // "DirectX"namespaceを今後省略する

Model::Model()
{
	d3d = Direct3D_Get();
	sampleMask = 0xffffffff;
}

void Model::Draw()
{
	static float ry; ry += 0.005f;
	// 回転行列
	XMMATRIX mxRotY = XMMatrixRotationY(XMConvertToRadians(mRotate.y));  // Y軸回転
	XMMATRIX mxRotX = XMMatrixRotationX(XMConvertToRadians(mRotate.x));
	XMMATRIX mxRotZ = XMMatrixRotationZ(XMConvertToRadians(mRotate.z));

	// 平行移動行列
	XMMATRIX mxTrans = XMMatrixTranslation(mPos.x, mPos.y, mPos.z);  // 立体の現在位置を表す変数を使って、平行移動の行列を作る

	// 拡大縮小行列
	XMMATRIX mxScale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	// ワールド変換行列を作成
	XMMATRIX mxWorld = mxScale * mxRotZ * mxRotX * mxRotY * mxTrans;

	// プロジェクション変換行列を作成
	// 1: 視野角
	// 2: アスペクト比　→　基本、ウインドウサイズの縦横比を書けばよい
	// 3: near 見える範囲の一番手前の視点からの距離
	// 4: far  見える範囲の一番奥の視点からの距離
	XMMATRIX mxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	// 描画で使用するテクスチャを指定する関数呼び出し
	// ここで指定されたテクスチャが、ピクセルシェーダーのグローバル変数にセットされる
	d3d->context->PSSetShaderResources(0, 1, &mModelData.mSRV);

	// 描画に使う頂点バッファを指定する
	UINT strides = sizeof(VERTEX3D);
	UINT offsets = 0;
	// 第三引数：頂点バッファ
	d3d->context->IASetVertexBuffers(0, 1, &mModelData.mVertexBuffer, &strides, &offsets);

	// 定数バッファ更新　→　Ｃ言語からシェーダーにデータを転送する
	D3D11_MAPPED_SUBRESOURCE pData;  // 定数バッファで確保されたメモリ領域の情報が格納される構造体
	// 定数バッファのメモリ領域にアクセスして、データを転送する処理
	// 定数バッファのメモリにアクセスするために、そのメモリ領域にロックをかける

	HRESULT hr = d3d->context->Map(gpConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		ConstBufferData cbdata;

		// Cameraからビュー変換行列を取得
		XMMATRIX mxView = mCamera->GetViewMatrix();

		cbdata.world = XMMatrixTranspose(mxWorld);
		cbdata.view = XMMatrixTranspose(mxView);
		cbdata.projection = XMMatrixTranspose(mxProjection);

		// 1: コピーしたい先のアドレス
		// 2: コピー先の確保されているデータサイズ
		// 3: コピーしたいデータが入っている先頭アドレス
		// 4: コピーしたいデータのサイズ
		memcpy_s(pData.pData, pData.RowPitch, &cbdata, sizeof(cbdata));

		d3d->context->Unmap(gpConstBuffer, 0);  // ロックしたメモリを解除する
	}

	// 定数バッファをシェーダーからアクセスできるようにセットする
	d3d->context->VSSetConstantBuffers(1, 1, &gpConstBuffer);

	// これまでの設定で実際に描画する  第一引数：実際に描画する頂点数
	d3d->context->Draw(mModelData.mNumVertex, 0);
}

void Model::SetModelData(ModelData model)
{
	mModelData = model;
}

void Model::AddBlendState()
{	//加算合成
	d3d->context->OMSetBlendState(d3d->blendAdd, NULL, sampleMask);
}

void Model::SubBlendState()
{	//減算合成
	d3d->context->OMSetBlendState(d3d->blendSub, NULL, sampleMask);
}

void Model::AlphaBlendState()
{	//アルファブレンディング
	d3d->context->OMSetBlendState(d3d->blendAlpha, NULL, sampleMask);
}

void Model::GetBlendState()
{	//現在設定されているブレンドステートを取得する
	d3d->context->OMGetBlendState(&d3d->blendNow, NULL, &sampleMask);
}

void Model::BackBlendState()
{	//保存していたブレンドステートに戻す
	d3d->context->OMSetBlendState(d3d->blendNow, NULL, sampleMask);
}

void Model::DisableZBuffer()
{
	d3d->context->OMSetRenderTargets(1, &d3d->renderTarget, NULL);	//第3引数をNULLにするとZバッファ無効
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
