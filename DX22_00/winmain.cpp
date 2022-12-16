#undef UNICODE  // Unicode�ł͂Ȃ��A�}���`�o�C�g�������g��

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
#include "CDWriteTest.h"	// �����`��p
#include <string>
#include <map>  // �A�z�z��
#include <vector> // �ϓ��I�z��
#include<algorithm>
#include<random>


#pragma comment(lib,"d2d1.lib")		//DirectWrite�p
#pragma comment(lib,"dwrite.lib")	//DirectWrite�p
#pragma comment (lib, "winmm.lib")	// timeGetTime�֐��̂���

// �}�N����`
#define CLASS_NAME    "DX21Smpl"// �E�C���h�E�N���X�̖��O
#define WINDOW_NAME  "�X�P���g���v���O����"// �E�B���h�E�̖��O

#define SCREEN_WIDTH (1024)	// �E�C���h�E�̕�
#define SCREEN_HEIGHT (576+30)	// �E�C���h�E�̍���

// �\���̂̒�`


// �֐��̃v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// �Q�[���̉�ʕ`��̊֐�
void Game_Init();
void Game_Draw();
void Game_Update();
void Game_Release();

// �r���{�[�h�p�̕��ʃ|���S�������֐�
ModelData CreateSquarePolygon(float width, float height, float uvWidth, float uvHeight, const wchar_t* texFile);

//Char[]�̒��g���������������ɓ���ւ���֐�
void SwapChar(void);

// �O���[�o���ϐ��̐錾

// ���_�o�b�t�@�p�̕ϐ�
ID3D11Buffer* gpVertexBuffer;

// ���_�������ϐ�
int gNumVertex;

// �e�N�X�`���p�̕ϐ�
ID3D11ShaderResourceView* gpTextureHalOsaka; // HAL OSAKA


extern ID3D11Buffer* gpConstBuffer;

using namespace DirectX;

// GameObject�N���X�̕ϐ�
GameObject* gpCottage;
GameObject* gpGun;

// �e�̕ϐ�
GameObject* gpShot;

#define MAX_CHAR  3
// 2D�L�����̕ϐ�	����ւ����ړ��p��+1
GameObject* gp2DChar[MAX_CHAR + 1];

//2D�L�����̉e�̕ϐ�
GameObject* gpShadow[MAX_CHAR];

#define MAX_GROUND  10
// ���P�O�~�c�P�O�̓񎟌��z��
GameObject* gpGround[MAX_GROUND][MAX_GROUND];

// �e�Ǘ��z��
std::vector<GameObject*> gShotManager;

//�����`��p
DirectWrite* DWrite;
FontData* font01;
FontData* font02;

// ���f���}�l�[�W���[
// �A�z�z�񁁓Y�����𐮐��ȊO�ɂł���z��
std::map<std::string, ModelData> gModelManager;

// �J�����N���X�̕ϐ�
Camera* gpCamera;

// �f���^�^�C���p�̕ϐ�
DWORD gDeltaTime;


// WinMain�֐������
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

	RegisterClassEx(&wc); // Windows�ɃE�C���h�E����o�^

	HWND hWnd; // H=Handle=�|�C���^ WND=Window
	hWnd = CreateWindowEx(0,// �g���E�B���h�E�X�^�C��
		CLASS_NAME,// �E�B���h�E�N���X�̖��O
		WINDOW_NAME,// �E�B���h�E�̖��O
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,// �E�B���h�E�X�^�C��
		CW_USEDEFAULT,// �E�B���h�E�̍���w���W
		CW_USEDEFAULT,// �E�B���h�E�̍���x���W 
		SCREEN_WIDTH,// �E�B���h�E�̕�
		SCREEN_HEIGHT,// �E�B���h�E�̍���
		NULL,// �e�E�B���h�E�̃n���h��
		NULL,// ���j���[�n���h���܂��͎q�E�B���h�EID
		hInstance,// �C���X�^���X�n���h��
		NULL);// �E�B���h�E�쐬�f�[�^

	// �w�肳�ꂽ�E�B���h�E�̕\����Ԃ�ݒ�(�E�B���h�E��\��)
	ShowWindow(hWnd, nCmdShow);
	// �E�B���h�E�̏�Ԃ𒼂��ɔ��f(�E�B���h�E�̃N���C�A���g�̈���X�V)
	UpdateWindow(hWnd);

	// Direct3D�̏������֐����Ăяo��
	Direct3D_Init(hWnd);

	Game_Init();



	MSG msg;
	// ���C�����[�v
	for (;;) {
		// �O��̃��[�v���烆�[�U�[���삪�����������ׂ�
		BOOL doesMessageExist = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		if (doesMessageExist)
		{
			// �ԐړI�ɃE�C���h�E�v���V�[�W�����Ăяo��
			DispatchMessage(&msg);

			// �A�v���I�����߂�����
			if (msg.message == WM_QUIT) {
				break;
			}
		}
		else
		{
			// �f���^�^�C���̌v��
			static DWORD lastTime = timeGetTime(); // �O��v�����̎���
			timeBeginPeriod(1); // ���x���P�~���b�ɏグ��
			DWORD nowTime = timeGetTime(); // ���݂̎���
			timeEndPeriod(1); // ���x�����ɖ߂�
			gDeltaTime = nowTime - lastTime; // �������f���^�^�C��
			lastTime = nowTime; // �O��v�����ԂƂ��ĕۑ�

			// �Q�[�����[�v
			Game_Update(); // �Q�[������
			Game_Draw();   // �Q�[���`��
			Input_Refresh(); // �L�[��Ԃ̍X�V
		}
	}

	Game_Release();
	// Direct3D�̉���֐����Ăяo��
	Direct3D_Release();

	UnregisterClass(CLASS_NAME, hInstance);

	return (int)msg.wParam;
}


// �E�C���h�E�v���V�[�W���֐������
// ���֐�������̂̓O���[�o���̈�(=�ǂ̊֐��̒��ł��Ȃ��ꏊ)�����I
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// uMsg�i���̊֐��̑�2�����j���A���[�U�[�����ID���
	switch (uMsg)
	{
	case WM_DESTROY:// �E�B���h�E�j���̃��b�Z�[�W
		PostQuitMessage(0);// �gWM_QUIT�h���b�Z�[�W�𑗂�@���@�A�v���I��
		break;

	case WM_CLOSE:  // x�{�^���������ꂽ��
		DestroyWindow(hWnd);  // �gWM_DESTROY�h���b�Z�[�W�𑗂�
		break;

	case WM_LBUTTONDOWN: // ���N���b�N���ꂽ�Ƃ�
		break;

	case WM_RBUTTONDOWN: // �E�N���b�N���ꂽ�Ƃ�
		break;

	case WM_MOUSEMOVE: // �}�E�X�J�[�\�����������Ƃ�
		break;

	case WM_KEYDOWN:
		// �L�[�������ꂽ���̃��A�N�V����������
		// ESC�������ꂽ�̂��ǂ����`�F�b�N
		if (LOWORD(wParam) == VK_ESCAPE)
		{
			// ���b�Z�[�W�{�b�N�X�ŏC���m�F
			int result;
			result = MessageBox(NULL, "�I�����Ă�낵���ł����H",
				"�I���m�F", MB_YESNO | MB_ICONQUESTION);
			if (result == IDYES) // �u�͂��v�{�^���������ꂽ��
			{
				// x�{�^���������ꂽ�̂Ɠ������ʂ𔭊�����
				PostMessage(hWnd, WM_CLOSE, wParam, lParam);
			}
		}
		Input_SetKeyDown(LOWORD(wParam));
		break;

	case WM_KEYUP: // �L�[�������ꂽ�C�x���g
		Input_SetKeyUp(LOWORD(wParam));
		break;

	default:
		// ���case�ȊO�̏ꍇ�̏��������s
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}

void Game_Init()
{
	// �萔�o�b�t�@�쐬
	// �R���X�^���g�o�b�t�@�Ƃ��č쐬���邽�߂̏��ݒ�
	D3D11_BUFFER_DESC contstat_buffer_desc;
	contstat_buffer_desc.ByteWidth = 4 * 4 * 4 * 4;	// �o�b�t�@�̃T�C�Y�i4x4�s��x4�j
	contstat_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;		// �g�p���@
	contstat_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �o�b�t�@�̎��(�R���X�^���g�o�b�t�@)
	contstat_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// CPU�A�N�Z�X�ݒ�
	contstat_buffer_desc.MiscFlags = 0;				// ���̑��̃I�v�V����
	contstat_buffer_desc.StructureByteStride = 0;			// �\���̃T�C�Y(�s����g���������0�ł����삷�邱�Ƃ����؂���)

	Direct3D_Get()->device->CreateBuffer(&contstat_buffer_desc,
		nullptr, &gpConstBuffer);

	// �J�����쐬
	gpCamera = new BackCamera();
	// �J���������l
	// eye��focus���������W���ƃ_��
	gpCamera->SetEye(XMFLOAT3(0.0f, 0.0f, -2.0f));
	gpCamera->SetFocus(XMFLOAT3(0.0f, 0.0f, 0.0f));
	// up��(0.0f,0.0f,0.0f)���ƃ_��
	gpCamera->SetUp(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// �R�e�[�W���f���ǂݍ���
	ObjModelLoader loader;
	gModelManager["cottage"] = loader.Load(
		"assets/cottage.obj",
		L"assets/cottage.png");

	// �e���f���ǂݍ���
	ObjModelLoader loader2;
	gModelManager["gun"] = loader2.Load(
		"assets/gun.obj", L"assets/gun.png");

	// �n�ʃ��f���ǂݍ���
	ObjModelLoader loader3;
	gModelManager["ground1"] = loader3.Load(
		"assets/ground1.obj", L"assets/ground1.jpg"
	);

	// �r���{�[�h�i�e�j�p���f���ǂݍ���
	ObjModelLoader loader4;
	gModelManager["shot"] = loader4.Load(
		"assets/billboard.obj", L"assets/shot.png"
	);

	// 2D�L�����p���f���쐬
	gModelManager["2Dchar01"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");

	gModelManager["2Dchar02"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");

	gModelManager["2Dchar03"] =
		CreateSquarePolygon(1.0f, 1.2f, 0.33f, 0.25f,
			L"assets/char01.png");

	//�ۉe�p���f���ǂݍ���
	loader = ObjModelLoader();
	gModelManager["Shadow"] = loader.Load("assets/shadow01.obj", L"assets/effect000.jpg");

	// �R�e�[�W�pModel�I�u�W�F�N�g����
	gpCottage = new NormalObject();
	Model* pModel = gpCottage->GetModel();
	pModel->SetModelData(gModelManager["cottage"]); // 3D�f�[�^���Z�b�g
	pModel->SetScale(0.001f);
	pModel->SetPos(0.0f, 0.0f, 4.0f);
	pModel->mCamera = gpCamera;

	// �e�pModel�I�u�W�F�N�g����
	gpGun = new NormalObject();
	pModel = gpGun->GetModel();
	pModel->SetModelData(gModelManager["gun"]);
	pModel->SetScale(1.5f);
	pModel->SetPos(0.0f, 0.2f, 1.0f);
	pModel->SetRot(0.0f, 90.0f, 0.0f);
	pModel->mCamera = gpCamera;

	// �e�I�u�W�F�N�g����
	gpShot = new BillboardObject();
	pModel = gpShot->GetModel();
	pModel->SetModelData(gModelManager["shot"]);
	pModel->SetScale(0.1f);
	pModel->SetPos(0.0f, 0.2f, 1.0f);
	pModel->mCamera = gpCamera;

	// 2D�L�����I�u�W�F�N�g����
	gp2DChar[0] = new BillboardObject();
	pModel = gp2DChar[0]->GetModel();
	pModel->SetModelData(gModelManager["2Dchar01"]);
	pModel->SetScale(1.0f);
	pModel->SetPos(2.0f, 0.6f, 5.0f);
	pModel->mCamera = gpCamera;

	// 2D�L�����I�u�W�F�N�g����
	gp2DChar[1] = new BillboardObject();
	pModel = gp2DChar[1]->GetModel();
	pModel->SetModelData(gModelManager["2Dchar02"]);
	pModel->SetScale(1.0f);
	pModel->SetPos(3.0f, 0.6f, 7.0f);
	pModel->mCamera = gpCamera;

	// 2D�L�����I�u�W�F�N�g����
	gp2DChar[2] = new BillboardObject();
	pModel = gp2DChar[2]->GetModel();
	pModel->SetModelData(gModelManager["2Dchar03"]);
	pModel->SetScale(1.0f);
	pModel->SetPos(4.0f, 0.6f, 9.0f);
	pModel->mCamera = gpCamera;

	//�ۉe�I�u�W�F�N�g����
	for (int i = 0; i < MAX_CHAR; i++) {
		gpShadow[i] = new Shadow(gp2DChar[i]);
		pModel = gpShadow[i]->GetModel();
		pModel->SetModelData(gModelManager["Shadow"]);
		pModel->SetScale(0.5f);
		pModel->mCamera = gpCamera;
	}

	// �n�ʂ𐶐�
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

	//�����`��
	font01 = new FontData();
	font02 = new FontData();
	font02->Color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	font02->fontSize = 60;

	DWrite = new DirectWrite(font01);
	DWrite->Init();

	// �Ǐ]�J�������Ǐ]����Ώۂ�ݒ�
	((BackCamera*)gpCamera)->SetTarget(gpGun);
}



void Game_Draw()
{
	// DIRECT3D�\���̂ɃA�N�Z�X����
	DIRECT3D* d3d = Direct3D_Get();

	// ��ʃN���A�i�w��F�œh��Ԃ��j
	// �h��Ԃ��F�̎w��i�����F0.0f�`1.0f�j
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha

	d3d->context->ClearRenderTargetView(d3d->renderTarget, clearColor);

	// Z�o�b�t�@�A�X�e���V���o�b�t�@�N���A
	d3d->context->ClearDepthStencilView(
		d3d->depthStencilView,			// �f�v�X�X�e���V���r���[
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// �y�o�b�t�@��1.0�ŃN���A����(0.0f�`1.0f)
		0);				// �X�e���V���o�b�t�@��0�ŃN���A����

	// ���@���O�̕`�揈���������ɏ��� *******

	// �Q�[���I�u�W�F�N�g��`��




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

	// �e�Ǘ��z��̒��g��S��Draw����
	for (int i = 0; i < gShotManager.size(); i++) {
		gShotManager[i]->GetModel()->GetBlendState();
		gShotManager[i]->GetModel()->AddBlendState();
		gShotManager[i]->Draw();
		gShotManager[i]->GetModel()->BackBlendState();
	}

	// �_�u���o�b�t�@�̐؂�ւ�
	d3d->swapChain->Present(0, 0);
}

void Game_Update()
{
	// �f���^�^�C�����z��O�̒l�ɂȂ����ꍇ�̏���
	if (gDeltaTime >= 100)
	{
		gDeltaTime = 0;
	}
	if (gDeltaTime <= 0)
	{
		gDeltaTime = 1;
	}

	/*
	2D�`��Ŏg����Dwrite��3D�`��Ŏg�����@���Ⴄ
	z���W����͂ł��Ȃ��̂����H
	�G���[�͏o�Ȃ�
	DWrite->DrawString("DirectWrite����", DirectX::XMFLOAT2(0.0f, 0.0f), D2D1_DRAW_TEXT_OPTIONS_NONE);
	*/
	
	

	static float angle = 0.0f;
	static float zoom = 5.0f;

	//angle += 0.01f;

	// �@�L�[����ŃJ���������E�Ɉړ�������
	if (Input_GetKeyDown(VK_RIGHT))
		angle += 0.05f * gDeltaTime;
	if (Input_GetKeyDown(VK_LEFT))
		angle -= 0.05f * gDeltaTime;

	// �A�L�[����ŃY�[���ł���悤�ɂ���
	if (Input_GetKeyDown(VK_UP))
		zoom -= 0.01f * gDeltaTime;
	if (Input_GetKeyDown(VK_DOWN))
		zoom += 0.01f * gDeltaTime;

	// �B�L�[����Œ����_�̎�����J��������]�ł���
//	�J������X���W�@���@sinf(�p�x���W�A��);
//	�J������Z���W�@���@cosf(�p�x���W�A��);

	// ���_�𒆐S�ɔ��a1.0f�̉~����̓_���v�Z����
	float radian = XMConvertToRadians(angle);
	gpCamera->mEye.x = sinf(radian) * zoom + 0.0f;
	gpCamera->mEye.z = cosf(radian) * zoom + 2.0f;
	gpCamera->mEye.y = 2.0f;

	// �R�e�[�W�𒍎��_
	gpCamera->SetFocus(gpCottage->GetModel()->GetPos());

	// �L�����N�^�[�ړ�
	// ���@�L�����N�^�[�������Ă�������ɐi�܂���ɂ́H
	// �@���@���i�K�ňړ��ł���
	// �@���@�u�O�����x�N�g���v���g��
	gpGun->mSpeed = 0.0f;

	float speed = 0.002f;

	// �e�̑O�i
	if (Input_GetKeyDown('W'))
		gpGun->mSpeed = speed * gDeltaTime;
	if (Input_GetKeyDown('S'))
		gpGun->mSpeed = -speed * gDeltaTime;

	Model* pGunModel = gpGun->GetModel();

	float rotSpeed = 0.09f;
	// �e�̕����]��
	if (Input_GetKeyDown('A') && !Input_GetKeyDown('S'))
		pGunModel->AddRot(0.0f, -rotSpeed * gDeltaTime, 0.0f);
	else if (Input_GetKeyDown('A') && Input_GetKeyDown('S'))
		pGunModel->AddRot(0.0f, rotSpeed * gDeltaTime, 0.0f);
	if (Input_GetKeyDown('D') && !Input_GetKeyDown('S'))
		pGunModel->AddRot(0.0f, rotSpeed * gDeltaTime, 0.0f);
	else if (Input_GetKeyDown('D') && Input_GetKeyDown('S'))
		pGunModel->AddRot(0.0f, -rotSpeed * gDeltaTime, 0.0f);

	// �e�̔���
	float scale = 0.002f;	//�g��{��

	if (Input_GetKeyDown(VK_SPACE)) {
		if (Input_GetKeyTrigger(VK_SPACE))
		{
			// �e�I�u�W�F�N�g����
			gpShot = new BillboardObject();
			Model* pShotModel = gpShot->GetModel();
			pShotModel->SetModelData(gModelManager["shot"]);
			pShotModel->SetScale(0.1f);
			pShotModel->SetPos(0.0f, 0.3f, 1.0f);
			pShotModel->mCamera = gpCamera;
			// �X�s�[�h�̒l��ݒ肷��
			gpShot->mSpeed = 0.00f;

			// �e�Ǘ��z��ɒǉ�����
			gShotManager.emplace_back(gpShot);
		}
		// �e����ԕ�����ݒ肷��
		gpShot->GetModel()->SetRot(0.0f, pGunModel->GetRotY(), 0.0f);

		//�e�̏ꏊ
		//�e���f���̈ʒu����O�����x�N�g�� * ���l(forward)�������O�Ɏw��
		float forward = 1.0f;
		gpShot->GetModel()->SetPos(
			gpGun->GetModel()->GetPosX() + gpGun->GetForwardVector().x * forward,
			0.3f,
			gpGun->GetModel()->GetPosZ() + gpGun->GetForwardVector().z * forward);
		gpShot->GetModel()->AddScale(scale * gDeltaTime);	//�e�̊g��
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

	// �e�Ǘ��z��̒��g��S��Update����
	for (int i = 0; i < gShotManager.size(); i++)
		gShotManager[i]->Update();

	// �e�������ɍs���Ă��܂��������
	for (int i = 0; i < gShotManager.size(); i++)
	{
		// (a)�J�����̈ʒu���擾����
		XMFLOAT3 cameraPos = gpCamera->mEye;
		// (b)�e�̈ʒu���擾����
		XMFLOAT3 shotPos = gShotManager[i]->GetModel()->GetPos();
		// (a)-(b)���v�Z����(b)��(a)�x�N�g�������
		XMVECTOR baVec =
			XMLoadFloat3(&cameraPos) - XMLoadFloat3(&shotPos);
		// (b)��(a)�x�N�g���̒����𑪂�
		XMVECTOR baVecLength = XMVector3Length(baVec);
		// ���ۂ̋��������o��
		float distance = XMVectorGetX(baVecLength);
		// distance�������ȏ�Ȃ�e���������
		if (distance >= 50.0f)
		{
			// erase�֐��͎����I��delete�܂ł��Ă���Ȃ�
			delete gShotManager[i];
			// �e�Ǘ��z�񂩂�e���폜
			gShotManager.erase(gShotManager.begin() + i);
		}
	}

	// �J�����̍X�V�����i�r���[�ϊ��s��v�Z�j
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
	//DWrite�n����
	delete font02;
	delete font01;
	DWrite->Release();

	// �e�Ǘ��z��̒��g��S��delete����
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

	// ���f���}�l�[�W���[���Ǘ����郂�f����S�������
	for (auto i = gModelManager.begin();// ��ԑO�̗v�f�̊Ǘ�����Ԃ�
		i != gModelManager.end();// ��ԍŌ�̊Ǘ�����Ԃ�
		i++)
	{
		// first �c �Y����
		// second �c �i�[����Ă���v�f���̂���
		COM_SAFE_RELEASE(i->second.mSRV);
		COM_SAFE_RELEASE(i->second.mVertexBuffer);
	}
	// �A�z�z��̗v�f��S�폜
	gModelManager.clear();
}

ModelData CreateSquarePolygon(float width, float height, float uvWidth, float uvHeight, const wchar_t* texFile)
{
	// ���f�����_�f�[�^�쐬
	const float w = width / 2.0f;
	const float h = height / 2.0f;
	ModelVertex vx[] = {
		// (x, y, z), (r, g, b, a), (u, v)
		// ����
		{ -w,  h, 0, 1, 1, 1, 1, 0.0f, 0.0f },
		{  w,  h, 0, 1, 1, 1, 1, uvWidth, 0.0f },
		{  w, -h, 0, 1, 1, 1, 1, uvWidth, uvHeight },

		{  w, -h, 0, 1, 1, 1, 1, uvWidth, uvHeight },
		{ -w, -h, 0, 1, 1, 1, 1, 0.0f, uvHeight },
		{ -w,  h, 0, 1, 1, 1, 1, 0.0f, 0.0f },
	};

	ModelData modelData;

	modelData.mNumVertex = sizeof(vx) / sizeof(ModelVertex); // ���_�����v�Z

	// ���f���p�̒��_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(vx);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// �o�b�t�@�̏����l�w��
	D3D11_SUBRESOURCE_DATA initialVertexData;
	// ���_�o�b�t�@�̏����l
	initialVertexData.pSysMem = vx;
	// ���_�o�b�t�@�ł͎g�p���Ȃ�
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

//gp2DChar�̒��g���J��������̋������������ɕ��ёւ���
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
		//�ő�l�̓Y�����擾����
		std::vector<float>::iterator fIter = std::max_element(fDis.begin(), fDis.end());	//�ő�l�܂ł̋������擾
		size_t index = std::distance(fDis.begin(), fIter);	//�ő�l�܂ł̋�������ő�l�̓Y�����擾

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

