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

	//for (size_t i = 0; i < _countof(worldTransform_); i++) {}
		//大元
		worldTransform_[PartId::Root].Initialize();
		//背骨
		worldTransform_[PartId::Spine].translation_ = {0, 4.5f, 0};
		worldTransform_[PartId::Spine].parent_ = &worldTransform_[PartId::Root];
		worldTransform_[PartId::Spine].Initialize();

		//上半身
	    worldTransform_[PartId::Chest].translation_ = {0, 0, 0};
	    worldTransform_[PartId::Chest].parent_ = &worldTransform_[PartId::Spine];
	    worldTransform_[PartId::Chest].Initialize();

		worldTransform_[PartId::Head].translation_ = {0, 4.5f, 0};
	    worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Chest];
	    worldTransform_[PartId::Head].Initialize();
		
		worldTransform_[PartId::ArmL].translation_ = {-4.5f, 0, 0};
	    worldTransform_[PartId::ArmL].parent_ = &worldTransform_[PartId::Chest];
	    worldTransform_[PartId::ArmL].Initialize();

		worldTransform_[PartId::ArmR].translation_ = {4.5f, 0, 0};
	    worldTransform_[PartId::ArmR].parent_ = &worldTransform_[PartId::Chest];
	    worldTransform_[PartId::ArmR].Initialize();

		//下半身

		worldTransform_[PartId::Hip].translation_ = {0, -4.5f, 0};
	    worldTransform_[PartId::Hip].parent_ = &worldTransform_[PartId::Spine];
	    worldTransform_[PartId::Hip].Initialize();

		worldTransform_[PartId::LegL].translation_ = {-4.5f, -4.5f, 0};
	    worldTransform_[PartId::LegL].parent_ = &worldTransform_[PartId::Hip];
	    worldTransform_[PartId::LegL].Initialize();

		worldTransform_[PartId::LegR].translation_ = {4.5f, -4.5f, 0};
	    worldTransform_[PartId::LegR].parent_ = &worldTransform_[PartId::Hip];
	    worldTransform_[PartId::LegR].Initialize();

	viewProjection_.Initialize();
}
//更新
void GameScene::Update() {
	//デバッグ
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	//デバッグ
	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
	  viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	worldTransform_[0].UpdateMatrix();
	worldTransform_[1].UpdateMatrix();
	{
		XMFLOAT3 move{0, 0, 0};

		const float kCharacterSpeed = 0.2f;

		if (input_->PushKey(DIK_LEFT)) {
			move = {-kCharacterSpeed, 0, 0};
		} else if (input_->PushKey(DIK_RIGHT)) {
			move = {kCharacterSpeed, 0, 0};
		}
		worldTransform_[PartId::Root].translation_.x += move.x;
		worldTransform_[PartId::Root].translation_.y += move.y;
		worldTransform_[PartId::Root].translation_.z += move.z;
		debugText_->SetPos(50, 150);
		debugText_->Printf(
		  "Root:(%f,%f,%f)", worldTransform_[PartId::Root].translation_.x,
		  worldTransform_[PartId::Root].translation_.y,
		  worldTransform_[PartId::Root].translation_.z);
	}
	//上半身
	{
		const float kChestRotSpeed = 0.05f;

		if (input_->PushKey(DIK_U)) {
			worldTransform_[PartId::Chest].rotation_.y -= kChestRotSpeed;
		} else if (input_->PushKey(DIK_I)) {
			worldTransform_[PartId::Chest].rotation_.y += kChestRotSpeed;
		}
	}
	//下半身
	{
		const float kHipRotSpeed = 0.05f;

		if (input_->PushKey(DIK_J)) {
			worldTransform_[PartId::Hip].rotation_.y -= kHipRotSpeed;
		} else if (input_->PushKey(DIK_K)) {
			worldTransform_[PartId::Hip].rotation_.y += kHipRotSpeed;
		}
	}
		//
		worldTransform_[PartId::Root].UpdateMatrix();
		worldTransform_[PartId::Spine].UpdateMatrix();
		worldTransform_[PartId::Chest].UpdateMatrix();
		worldTransform_[PartId::Head].UpdateMatrix();
		worldTransform_[PartId::ArmL].UpdateMatrix();
		worldTransform_[PartId::ArmR].UpdateMatrix();
		worldTransform_[PartId::Hip].UpdateMatrix();
		worldTransform_[PartId::LegL].UpdateMatrix();
		worldTransform_[PartId::LegR].UpdateMatrix();
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
	//	model_->Draw(worldTransform_[0], viewProjection_, textureHandle_);
	//	model_->Draw(worldTransform_[1], viewProjection_, textureHandle_);
		model_->Draw(worldTransform_[PartId::Chest], viewProjection_, textureHandle_);
		model_->Draw(worldTransform_[PartId::Head], viewProjection_, textureHandle_);
		model_->Draw(worldTransform_[PartId::ArmL], viewProjection_, textureHandle_);
		model_->Draw(worldTransform_[PartId::ArmR], viewProjection_, textureHandle_);
		model_->Draw(worldTransform_[PartId::Hip], viewProjection_, textureHandle_);
		model_->Draw(worldTransform_[PartId::LegL], viewProjection_, textureHandle_);
		model_->Draw(worldTransform_[PartId::LegR], viewProjection_, textureHandle_);
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