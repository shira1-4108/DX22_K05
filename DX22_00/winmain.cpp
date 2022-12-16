#undef UNICODE  // Unicodeではなく、マルチバイト文字を使う

#include <Windows.h>
#include "Direct3d.h"
#include "input.h"
#include <DirectXMath.h>
#include "WICTextureLoader.h"
#include "ObjModelLoader.h"
#include "Model.h"
#include "BackCamera.h"
#include "GameObject.h"
#include "NormalObject.h"
#include "Shadow.h"
#include "BillboardObject.h"
#include "CDWriteTest.h"	// 文字描画用
#include <string>
#include <map>  // 連想配列
#include <vector> // 可変動的配列
#include<algorithm>
#include<random>


#pragma comment(lib,"d2d1.lib")		//DirectWrite用
#pragma comment(lib,"dwrite.lib")	//DirectWrite用
#pragma comment (lib, "winmm.lib")	// timeGetTime関数のため

// マクロ定義
#define CLASS_NAME    "DX21Smpl"// ウインドウクラスの名前
#define WINDOW_NAME  "スケルトンプログラム"// ウィンドウの名前

#define SCREEN_WIDTH (1024)	// ウインドウの幅
#define SCREEN_HEIGHT (576+30)	// ウインドウの高さ

// 構造体の定義


// 関数のプロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ゲームの画面描画の関数
void Game_Init();
void Game_Draw();
void Game_Update();
void Game_Release();

// ビルボード用の平面ポリゴンを作る関数
ModelData CreateSquarePolygon(float width, float height, float uvWidth, float uvHeight, const wchar_t* texFile);

//Char[]の中身を距離が遠い順に入れ替える関数
void SwapChar(void);

// グローバル変数の宣言

// 頂点バッファ用の変数
ID3D11Buffer* gpVertexBuffer;

// 頂点数を持つ変数
int gNumVertex;

// テクスチャ用の変数
ID3D11ShaderResourceView* gpTextureHalOsaka; // HAL OSAKA


extern ID3D11Buffer* gpConstBuffer;

using namespace DirectX;

// GameObjectクラスの変数
GameObject* gpCottage;
GameObject* gpGun;

// 弾の変数
GameObject* gpShot;

#define MAX_CHAR  3
// 2Dキャラの変数	入れ替え時移動用に+1
GameObject* gp2DChar[MAX_CHAR + 1];

//2Dキャラの影の変数
GameObject* gpShadow[MAX_CHAR];

#define MAX_GROUND  10
// 横１０×縦１０の二次元配列
GameObject* gpGround[MAX_GROUND][MAX_GROUND];

// 弾管理配列
std::vector<GameObject*> gShotManager;

//文字描画用
DirectWrite* DWrite;
FontData* font01;
FontData* font02;

// モデルマネージャー
// 連想配列＝添え字を整数以外にできる配列
std::map<std::string, ModelData> gModelManager;

// カメラクラスの変数
Camera* gpCamera;

// デルタタイム用の変数
DWORD gDeltaTime;


// WinMain関数を作る
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc; // WND = Window

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc); // Windowsにウインドウ情報を登録

	HWND hWnd; // H=Handle=ポインタ WND=Window
	hWnd = CreateWindowEx(0,// 拡張ウィンドウスタイル
		CLASS_NAME,// ウィンドウクラスの名前
		WINDOW_NAME,// ウィンドウの名前
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,// ウィンドウスタイル
		CW_USEDEFAULT,// ウィンドウの左上Ｘ座標
		CW_USEDEFAULT,// ウィンドウの左上Ｙ座標 
		SCREEN_WIDTH,// ウィンドウの幅
		SCREEN_HEIGHT,// ウィンドウの高さ
		NULL,// 親ウィンドウのハンドル
		NULL,// メニューハンドルまたは子ウィンドウID
		hInstance,// インスタンスハンドル
		NULL);// ウィンドウ作成データ

	// 指定されたウィンドウの表示状態を設定(ウィンドウを表示)
	ShowWindow(hWnd, nCmdShow);
	// ウィンドウの状態を直ちに反映(ウィンドウのクライアント領域を更新)
	UpdateWindow(hWnd);

	// Direct3Dの初期化関数を呼び出す
	Direct3D_Init(hWnd);

	Game_Init();



	MSG msg;
	// メインループ
	for (;;) {
		// 前回のループからユーザー操作があったか調べる
		BOOL doesMessageExist = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		if (doesMessageExist)
		{
			// 間接的にウインドウプロシージャを呼び出す
			DispatchMessage(&msg);

			// アプリ終了命令が来た
			if (msg.message == WM_QUIT) {
				break;
			}
		}
		else
		{
			// デルタタイムの計測
			static DWORD lastTime = timeGetTime(); // 前回計測時の時間
			timeBeginPeriod(1); // 精度を１ミリ秒に上げる
			DWORD nowTime = timeGetTime(); // 現在の時間
			timeEndPeriod(1); // 精度を元に戻す
			gDeltaTime = nowTime - lastTime; // 差分がデルタタイム
			lastTime = nowTime; // 前回計測時間として保存

			// ゲームループ
			Game_Update(); // ゲーム処理
			Game_Draw();   // ゲーム描画
			Input_Refresh(); // キー状態の更新
		}
	}

	Game_Release();
	// Direct3Dの解放関数を呼び出す
	Direct3D_Release();

	UnregisterClass(CLASS_NAME, hInstance);

	return (int)msg.wParam;
}


// ウインドウプロシージャ関数を作る
// ※関数を作れるのはグローバル領域(=どの関数の中でもない場所)だけ！
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// uMsg（この関数の第2引数）が、ユーザー操作のID情報
	switch (uMsg)
	{
	case WM_DESTROY:// ウィンドウ破棄のメッセージ
		PostQuitMessage(0);// “WM_QUIT”メッセージを送る　→　アプリ終了
		break;

	case WM_CLOSE:  // xボタンが押されたら
		DestroyWindow(hWnd);  // “WM_DESTROY”メッセージを送る
		break;

	case WM_LBUTTONDOWN: // 左クリックされたとき
		break;

	case WM_RBUTTONDOWN: // 右クリックされたとき
		break;

	case WM_MOUSEMOVE: // マウスカーソルが動いたとき
		break;

	case WM_KEYDOWN:
		// キーが押された時のリアクションを書く
		// ESCが押されたのかどうかチェック
		if (LOWORD(wParam) == VK_ESCAPE)
		{
			// メッセージボックスで修了確認
			int result;
			result = MessageBox(NULL, "終了してよろしいですか？",
				"終了確認", MB_YESNO | MB_ICONQUESTION);
			if (result == IDYES) // 「はい」ボタンが押された時
			{
				// xボタンが押されたのと同じ効果を発揮する
				PostMessage(hWnd, WM_CLOSE, wParam, lParam);
			}
		}
		Input_SetKeyDown(LOWORD(wParam));
		break;

	case WM_KEYUP: // キーが離されたイベント
		Input_SetKeyUp(LOWORD(wParam));
		break;

	default:
		// 上のcase以外の場合の処理を実行
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}

void Game_Init()
{
	// 定数バッファ作成
	// コンスタントバッファとして作成するための情報設定
	D3D11_BUFFER_DESC contstat_buffer_desc;
	contstat_buffer_desc.ByteWidth = 4 * 4 * 4 * 4;	// バッファのサイズ（4x4行列x4個）
	contstat_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;		// 使用方法
	contstat_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// バッファの種類(コンスタントバッファ)
	contstat_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// CPUアクセス設定
	contstat_buffer_desc.MiscFlags = 0;				// その他のオプション
	contstat_buffer_desc.StructureByteStride = 0;			// 構造体サイズ(行列を使うが今回は0でも動作することを実証する)

	Direct3D_Get()->device->CreateBuffer(&contstat_buffer_desc,
		nullptr, &gpConstBuffer);

	// カメラ作成
	gpCamera = new BackCamera();
	// カメラ初期値
	// eyeとfocusが同じ座標だとダメ
	gpCamera->SetEye(XMFLOAT3(0.0f, 0.0f, -2.0f));
	gpCamera->SetFocus(XMFLOAT3(0.0f, 0.0f, 0.0f));
	// upは(0.0f,0.0f,0.0f)だとダメ
	gpCamera->SetUp(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// コテージモデル読み込み
	ObjModelLoader loader;
	gModelManager["cottage"] = loader.Load(
		"assets/cottage.obj",
		L"assets/cottage.png");

	// 銃モデル読み込み
	ObjModelLoader loader2;
	gModelManager["gun"] = loader2.Load(
		"assets/gun.obj", L"assets/gun.png");

	// 地面モデル読み込み
	ObjModelLoader loader3;
	gModelManager["ground1"] = loader3.Load(
		"assets/ground1.obj", L"assets/ground1.jpg"
	);

	// ビルボード（弾）用モデル読み込み
	ObjModelLoader loader4;
	gModelManager["shot"] = loader4.Load(
		"assets/billboard.obj", L"assets/shot.png"
	);

	// 2Dキャラ用モデル作成
	gModelManager["2Dchar01"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");

	gModelManager["2Dchar02"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");

	gModelManager["2Dchar03"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");

	//丸影用モデル読み込み
	loader = ObjModelLoader();
	gModelManager["Shadow"] = loader.Load("assets/shadow01.obj", L"assets/effect000.jpg");

	// コテージ用Modelオブジェクト生成
	gpCottage = new NormalObject();
	Model* pModel = gpCottage->GetModel();
	pModel->SetModelData(gModelManager["cottage"]); // 3Dデータをセット
	pModel->SetScale(0.001f);
	pModel->SetPos(0.0f, 0.0f, 4.0f);
	pModel->mCamera = gpCamera;

	// 銃用Modelオブジェクト生成
	gpGun = new NormalObject();
	pModel = gpGun->GetModel();
	pModel->SetModelData(gModelManager["gun"]);
	pModel->SetScale(1.5f);
	pModel->SetPos(0.0f, 0.2f, 1.0f);
	pModel->SetRot(0.0f, 90.0f, 0.0f);
	pModel->mCamera = gpCamera;

	// 弾オブジェクト生成
	gpShot = new BillboardObject();
	pModel = gpShot->GetModel();
	pModel->SetModelData(gModelManager["shot"]);
	pModel->SetScale(0.1f);
	pModel->SetPos(0.0f, 0.2f, 1.0f);
	pModel->mCamera = gpCamera;

	// 2Dキャラオブジェクト生成
	gp2DChar[0] = new BillboardObject();
	pModel = gp2DChar[0]->GetModel();
	pModel->SetModelData(gModelManager["2Dchar01"]);
	pModel->SetScale(1.0f);
	pModel->SetPos(2.0f, 0.6f, 5.0f);
	pModel->mCamera = gpCamera;

	// 2Dキャラオブジェクト生成
	gp2DChar[1] = new BillboardObject();
	pModel = gp2DChar[1]->GetModel();
	pModel->SetModelData(gModelManager["2Dchar02"]);
	pModel->SetScale(1.0f);
	pModel->SetPos(3.0f, 0.6f, 7.0f);
	pModel->mCamera = gpCamera;

	// 2Dキャラオブジェクト生成
	gp2DChar[2] = new BillboardObject();
	pModel = gp2DChar[2]->GetModel();
	pModel->SetModelData(gModelManager["2Dchar03"]);
	pModel->SetScale(1.0f);
	pModel->SetPos(4.0f, 0.6f, 9.0f);
	pModel->mCamera = gpCamera;

	//丸影オブジェクト生成
	for (int i = 0; i < MAX_CHAR; i++) {
		gpShadow[i] = new Shadow(gp2DChar[i]);
		pModel = gpShadow[i]->GetModel();
		pModel->SetModelData(gModelManager["Shadow"]);
		pModel->SetScale(0.5f);
		pModel->mCamera = gpCamera;
	}

	// 地面を生成
	for (int i = 0; i < MAX_GROUND; i++)
	{
		for (int j = 0; j < MAX_GROUND; j++)
		{
			gpGround[i][j] = new NormalObject();
			Model* pGroundModel = gpGround[i][j]->GetModel();
			pGroundModel->SetModelData(gModelManager["ground1"]);
			pGroundModel->SetScale(1.0f);
			pGroundModel->SetPos(-10.0f + 2.0f * j, -1.0f, -10.0f + 2.0f * i);
			pGroundModel->mCamera = gpCamera;
		}
	}

	//文字描画
	font01 = new FontData();
	font02 = new FontData();
	font02->Color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	font02->fontSize = 60;

	DWrite = new DirectWrite(font01);
	DWrite->Init();

	// 追従カメラが追従する対象を設定
	((BackCamera*)gpCamera)->SetTarget(gpGun);
}



void Game_Draw()
{
	// DIRECT3D構造体にアクセスする
	DIRECT3D* d3d = Direct3D_Get();

	// 画面クリア（指定色で塗りつぶし）
	// 塗りつぶし色の指定（小数：0.0f～1.0f）
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha

	d3d->context->ClearRenderTargetView(d3d->renderTarget, clearColor);

	// Zバッファ、ステンシルバッファクリア
	d3d->context->ClearDepthStencilView(
		d3d->depthStencilView,			// デプスステンシルビュー
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// Ｚバッファを1.0でクリアする(0.0f～1.0f)
		0);				// ステンシルバッファを0でクリアする

	// ↓　自前の描画処理をここに書く *******

	// ゲームオブジェクトを描画




	for (int i = 0; i < MAX_GROUND; i++)
	{
		for (int j = 0; j < MAX_GROUND; j++)
		{
			gpGround[i][j]->Draw();
		}
	}

	for (int i = 0; i < MAX_CHAR; i++)
		gpShadow[i]->Draw();

	gpGun->Draw();
	gpCottage->Draw();

	SwapChar();
	for (int i = 0; i < MAX_CHAR; i++) {
		gp2DChar[i]->Draw();
	}

	// 弾管理配列の中身を全てDrawする
	for (int i = 0; i < gShotManager.size(); i++) {
		gShotManager[i]->GetModel()->GetBlendState();
		gShotManager[i]->GetModel()->AddBlendState();
		gShotManager[i]->Draw();
		gShotManager[i]->GetModel()->BackBlendState();
	}

	// ダブルバッファの切り替え
	d3d->swapChain->Present(0, 0);
}

void Game_Update()
{
	// デルタタイムが想定外の値になった場合の処理
	if (gDeltaTime >= 100)
	{
		gDeltaTime = 0;
	}
	if (gDeltaTime <= 0)
	{
		gDeltaTime = 1;
	}

	/*
	2D描画で使えたDwriteを3D描画で使う方法が違う
	z座標を入力できないのが問題？
	エラーは出ない
	DWrite->DrawString("DirectWriteだよ", DirectX::XMFLOAT2(0.0f, 0.0f), D2D1_DRAW_TEXT_OPTIONS_NONE);
	*/
	
	

	static float angle = 0.0f;
	static float zoom = 5.0f;

	//angle += 0.01f;

	// ①キー操作でカメラを左右に移動させる
	if (Input_GetKeyDown(VK_RIGHT))
		angle += 0.05f * gDeltaTime;
	if (Input_GetKeyDown(VK_LEFT))
		angle -= 0.05f * gDeltaTime;

	// ②キー操作でズームできるようにする
	if (Input_GetKeyDown(VK_UP))
		zoom -= 0.01f * gDeltaTime;
	if (Input_GetKeyDown(VK_DOWN))
		zoom += 0.01f * gDeltaTime;

	// ③キー操作で注視点の周りをカメラが回転できる
//	カメラのX座標　＝　sinf(角度ラジアン);
//	カメラのZ座標　＝　cosf(角度ラジアン);

	// 原点を中心に半径1.0fの円周上の点を計算する
	float radian = XMConvertToRadians(angle);
	gpCamera->mEye.x = sinf(radian) * zoom + 0.0f;
	gpCamera->mEye.z = cosf(radian) * zoom + 2.0f;
	gpCamera->mEye.y = 2.0f;

	// コテージを注視点
	gpCamera->SetFocus(gpCottage->GetModel()->GetPos());

	// キャラクター移動
	// →　キャラクターが向いている方向に進ませるには？
	// 　→　無段階で移動できる
	// 　→　「前向きベクトル」を使う
	gpGun->mSpeed = 0.0f;

	float speed = 0.002f;

	// 銃の前進
	if (Input_GetKeyDown('W'))
		gpGun->mSpeed = speed * gDeltaTime;
	if (Input_GetKeyDown('S'))
		gpGun->mSpeed = -speed * gDeltaTime;

	Model* pGunModel = gpGun->GetModel();

	float rotSpeed = 0.09f;
	// 銃の方向転換
	if (Input_GetKeyDown('A') && !Input_GetKeyDown('S'))
		pGunModel->AddRot(0.0f, -rotSpeed * gDeltaTime, 0.0f);
	else if (Input_GetKeyDown('A') && Input_GetKeyDown('S'))
		pGunModel->AddRot(0.0f, rotSpeed * gDeltaTime, 0.0f);
	if (Input_GetKeyDown('D') && !Input_GetKeyDown('S'))
		pGunModel->AddRot(0.0f, rotSpeed * gDeltaTime, 0.0f);
	else if (Input_GetKeyDown('D') && Input_GetKeyDown('S'))
		pGunModel->AddRot(0.0f, -rotSpeed * gDeltaTime, 0.0f);

	// 弾の発射
	float scale = 0.002f;	//拡大倍率

	if (Input_GetKeyDown(VK_SPACE)) {
		if (Input_GetKeyTrigger(VK_SPACE))
		{
			// 弾オブジェクト生成
			gpShot = new BillboardObject();
			Model* pShotModel = gpShot->GetModel();
			pShotModel->SetModelData(gModelManager["shot"]);
			pShotModel->SetScale(0.1f);
			pShotModel->SetPos(0.0f, 0.3f, 1.0f);
			pShotModel->mCamera = gpCamera;
			// スピードの値を設定する
			gpShot->mSpeed = 0.00f;

			// 弾管理配列に追加する
			gShotManager.emplace_back(gpShot);
		}
		// 弾が飛ぶ方向を設定する
		gpShot->GetModel()->SetRot(0.0f, pGunModel->GetRotY(), 0.0f);

		//弾の場所
		//銃モデルの位置から前向きベクトル * 一定値(forward)分だけ前に指定
		float forward = 1.0f;
		gpShot->GetModel()->SetPos(
			gpGun->GetModel()->GetPosX() + gpGun->GetForwardVector().x * forward,
			0.3f,
			gpGun->GetModel()->GetPosZ() + gpGun->GetForwardVector().z * forward);
		gpShot->GetModel()->AddScale(scale * gDeltaTime);	//弾の拡大
		if (gpShot->GetModel()->GetScale().x > 5.0f || gpShot->GetModel()->GetScale().y > 5.0f || gpShot->GetModel()->GetScale().z > 5.0f) {
			gpShot->GetModel()->SetScale(5.0f);
		}
	}
	else {
		gpShot->mSpeed = 0.01f;
	}


	gpGun->Update();
	for (int i = 0; i < MAX_CHAR; i++) {
		gp2DChar[i]->Update();
	}
	for (int i = 0; i < MAX_CHAR; i++)
		gpShadow[i]->Update();

	// 弾管理配列の中身を全てUpdateする
	for (int i = 0; i < gShotManager.size(); i++)
		gShotManager[i]->Update();

	// 弾が遠くに行ってしまったら消す
	for (int i = 0; i < gShotManager.size(); i++)
	{
		// (a)カメラの位置を取得する
		XMFLOAT3 cameraPos = gpCamera->mEye;
		// (b)弾の位置を取得する
		XMFLOAT3 shotPos = gShotManager[i]->GetModel()->GetPos();
		// (a)-(b)を計算して(b)→(a)ベクトルを作る
		XMVECTOR baVec =
			XMLoadFloat3(&cameraPos) - XMLoadFloat3(&shotPos);
		// (b)→(a)ベクトルの長さを測る
		XMVECTOR baVecLength = XMVector3Length(baVec);
		// 実際の距離を取り出す
		float distance = XMVectorGetX(baVecLength);
		// distanceが○○以上なら弾を回収する
		if (distance >= 50.0f)
		{
			// erase関数は自動的にdeleteまでしてくれない
			delete gShotManager[i];
			// 弾管理配列から弾を削除
			gShotManager.erase(gShotManager.begin() + i);
		}
	}

	// カメラの更新処理（ビュー変換行列計算）
	gpCamera->Update();

	for (int i = 0; i < MAX_GROUND; i++)
	{
		for (int j = 0; j < MAX_GROUND; j++)
		{
			gpGround[i][j]->Update();
		}
	}
}

void Game_Release()
{
	//DWrite系列解放
	delete font02;
	delete font01;
	DWrite->Release();

	// 弾管理配列の中身を全てdeleteする
	for (int i = 0; i < gShotManager.size(); i++)
		delete gShotManager[i];

	for (int i = 0; i < MAX_CHAR; i++)
		delete gpShadow[i];

	for (int i = 0; i < MAX_CHAR; i++) {
		delete gp2DChar[i];
	}
	delete gpGun;
	delete gpCottage;

	for (int i = 0; i < MAX_GROUND; i++)
	{
		for (int j = 0; j < MAX_GROUND; j++)
		{
			delete gpGround[i][j];
		}
	}

	COM_SAFE_RELEASE(gpConstBuffer);

	// モデルマネージャーが管理するモデルを全解放する
	for (auto i = gModelManager.begin();// 一番前の要素の管理情報を返す
		i != gModelManager.end();// 一番最後の管理情報を返す
		i++)
	{
		// first … 添え字
		// second … 格納されている要素そのもの
		COM_SAFE_RELEASE(i->second.mSRV);
		COM_SAFE_RELEASE(i->second.mVertexBuffer);
	}
	// 連想配列の要素を全削除
	gModelManager.clear();
}

ModelData CreateSquarePolygon(float width, float height, float uvWidth, float uvHeight, const wchar_t* texFile)
{
	// モデル頂点データ作成
	const float w = width / 2.0f;
	const float h = height / 2.0f;
	ModelVertex vx[] = {
		// (x, y, z), (r, g, b, a), (u, v)
		// 正面
		{ -w,  h, 0, 1, 1, 1, 1, 0.0f, 0.0f },
		{  w,  h, 0, 1, 1, 1, 1, uvWidth, 0.0f },
		{  w, -h, 0, 1, 1, 1, 1, uvWidth, uvHeight },

		{  w, -h, 0, 1, 1, 1, 1, uvWidth, uvHeight },
		{ -w, -h, 0, 1, 1, 1, 1, 0.0f, uvHeight },
		{ -w,  h, 0, 1, 1, 1, 1, 0.0f, 0.0f },
	};

	ModelData modelData;

	modelData.mNumVertex = sizeof(vx) / sizeof(ModelVertex); // 頂点数を計算

	// モデル用の頂点バッファ作成
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(vx);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// バッファの初期値指定
	D3D11_SUBRESOURCE_DATA initialVertexData;
	// 頂点バッファの初期値
	initialVertexData.pSysMem = vx;
	// 頂点バッファでは使用しない
	initialVertexData.SysMemPitch = 0;
	initialVertexData.SysMemSlicePitch = 0;

	HRESULT hr = Direct3D_Get()->device->CreateBuffer(&bufferDesc, &initialVertexData, &modelData.mVertexBuffer);

	if (FAILED(hr))
	{
		throw hr;
	}

	Direct3D_LoadTexture(texFile, &modelData.mSRV);

	return modelData;
}

//gp2DCharの中身をカメラからの距離が遠い順に並び替える
void SwapChar(void)
{
	std::vector<float> fDis;
	std::vector<int> iNo;
	std::vector<GameObject*> gDis;

	for (int i = 0; i < MAX_CHAR; i++) {
		fDis.push_back(gp2DChar[i]->GetDistaceVector(gpCamera->mEye));
		iNo.push_back(gp2DChar[i]->GetDistaceVector(gpCamera->mEye));
		gDis.push_back(gp2DChar[i]);
	}

	for (int i = 0; i < MAX_CHAR; i++) {
		//最大値の添字を取得する
		std::vector<float>::iterator fIter = std::max_element(fDis.begin(), fDis.end());	//最大値までの距離を取得
		size_t index = std::distance(fDis.begin(), fIter);	//最大値までの距離から最大値の添字を取得

		iNo[index] = i;
		fDis[index] = -1.0f;
	}

	for (int i = 0; i < MAX_CHAR; i++) {
		gDis[i] = gp2DChar[iNo[i]];
	}
	for (int i = 0; i < MAX_CHAR; i++) {
		gp2DChar[i] = gDis[i];
	}
}

