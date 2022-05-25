#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

//コンストラクタ
GameScene::GameScene() {}

//デストラクタ
GameScene::~GameScene() {
	delete sprite_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");
	soundDataHandle_ = audio_->LoadWave("se_sad03.wav");
	// voiceHandle_ = audio_->PlayWave(soundDataHandle_, true);

	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	// audio_->PlayWave(soundDataHandle_);

	model_ = Model::Create();

	//乱数シード生成
	std::random_device seed_gen;
	//メルセンヌ
	std::mt19937_64 engine(seed_gen());
	//乱数範囲回転
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲座標
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		// XYZ　スケーリング
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		// XYZ　回転角
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		// XYZ　平行移動
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};
		//初期化
		worldTransform_[i].Initialize();
	}
//	viewProjection_.target = {10, 0, 0};
//	viewProjection_.up = {cosf(XM_PI / 4.0f), sinf(XM_PI / 4.0f), 0.0f};
//	viewProjection_.eye = {0, 0, -10};

//	viewProjection_.fovAngleY = XMConvertToRadians(10.0f);

	viewProjection_.aspectRatio = 1.0f;

	viewProjection_.nearZ = 52.0f;
	
	viewProjection_.farZ = 53.0f;

	viewProjection_.Initialize();
}
//更新
void GameScene::Update() {
	//	if (input_->TriggerKey(DIK_SPACE))
	//音声停止
	//		audio_->StopWave(voiceHandle_);

	//	debugText_->Print("ikisugii 19 19", 50, 50, 1.0f);
	//	debugText_->SetPos(50, 70);
	//	debugText_->Printf("year:%d", 2001);

	//	value_++;

	//	std::string strDebug = std::string("value") + std::to_string(value_);

	//	debugText_->Print(strDebug, 50, 50, 1.0f);

	//視点移動処理
	//	{
	//視点の移動ベクトル
	//		XMFLOAT3 move = {0, 0, 0};

	//		const float kEyeSpeed = 0.2f;

	//		if (input_->PushKey(DIK_W)) {
	//			move = {0, 0, kEyeSpeed};
	//		} else if (input_->PushKey(DIK_S)) {
	//			move = {0, 0, -kEyeSpeed};
	//		}
	//視点移動
	//		viewProjection_.eye.x += move.x;
	//		viewProjection_.eye.y += move.y;
	//		viewProjection_.eye.z += move.z;
	//行列
	//		viewProjection_.UpdateMatrix();
	//デバッグ
			debugText_->SetPos(50, 50);
			debugText_->Printf(
			  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y,
	viewProjection_.eye.z);
	//	}
	//視点移動処理
	//	{
	//視点の移動ベクトル
	//		XMFLOAT3 move = {0, 0, 0};

	//注視点の速さ
	//		const float kTargetSpeed = 0.2f;
	//押した方向でベク変
	//		if (input_->PushKey(DIK_LEFT)) {
	//			move = {-kTargetSpeed, 0, 0};
	//		} else if (input_->PushKey(DIK_RIGHT)) {
	//			move = {kTargetSpeed, 0, 0};
	//		}
	//視点移動
	//		viewProjection_.target.x += move.x;
	//		viewProjection_.target.y += move.y;
	//		viewProjection_.target.z += move.z;
	//行列
	//		viewProjection_.UpdateMatrix();
	//デバッグ
			debugText_->SetPos(50, 70);
			debugText_->Printf(
			  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
			  viewProjection_.target.z);
	//	}
	//上方向回転処理
	//	{
	//上方向の回転の速さ
	//		const float kUpRotSpeed = 0.05f;
	//
	//		if (input_->PushKey(DIK_SPACE)) {
	//			viewAngle += kUpRotSpeed;
	// 2πを戻す
	//			viewAngle = fmodf(viewAngle, XM_2PI);
	//		}
	//上方向ベクトル計算
	//		viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};
	//行列の再計算
//			viewProjection_.UpdateMatrix();
			debugText_->SetPos(50, 90);
			debugText_->Printf(
			  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y,
			  viewProjection_.up.z);
//		}
	// for変更処理
	{
		if (input_->PushKey(DIK_W)) {
			viewProjection_.fovAngleY += 0.01f;
			viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
	} else if (input_->PushKey(DIK_S)){
			viewProjection_.fovAngleY -= 0.01f;
			viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
	}
	viewProjection_.UpdateMatrix();

		debugText_->SetPos(50,110);
		debugText_->Printf("fovAngleY(Degree):%f",XMConvertToDegrees(viewProjection_.fovAngleY));
	}

	{
		if (input_->PushKey(DIK_UP)) {
			viewProjection_.nearZ+= 0.1f;
		} else if (input_->PushKey(DIK_DOWN)) {
			viewProjection_.nearZ -= 0.1f;
		}
		viewProjection_.UpdateMatrix();

		debugText_->SetPos(50, 130);
		debugText_->Printf("nearZ:%f",(viewProjection_.nearZ));
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// sprite_->Draw();

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();
	//スプライトの今の座標を取得
	//	XMFLOAT2 position = sprite_->GetPosition();
	//座標を｛2,0｝移動
	//	position.x += 2.0f;
	//	position.y += 1.0f;
	//移動した座標をスプライトに反映
	//	sprite_->SetPosition(position);

#pragma endregion
}