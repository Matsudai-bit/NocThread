/*****************************************************************//**
 * @file    NormalGameplayState.h
 * @brief   ゲームプレイシーンの通常状態に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "NormalGameplayState.h"

// ライブラリ関連
#include "Library/DirectXFramework/DeviceResources.h"

// データベース関連
#include "Game/Common/Database/TextureDatabase.h"

// シーン関連
#include "Game/Scene/GameplayScene/GameplayScene.h"

// シーンの状態関連
#include "Game/Scene/GameplayScene/State/PoseGameplayState/PauseGameplayState.h"

// ファクトリー関連
#include "Game/Common/Factory/InputBindingFactory/InputBindingFactory.h"

// フレームワーク関連
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/CommonResources/CommonResources.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/StageManager/StageManager.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"

// グラフィック関連
#include "Game/Common/Screen.h"
#include "Game/Common/Graphics/TransitionMask/TransitionMask.h"
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Graphics/GameEffect/GameEffectController.h"





using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
NormalGameplayState::NormalGameplayState()
	: m_isPrevConnectedGamepad{ false }
{

}

/**
 * @brief デストラクタ
 */
NormalGameplayState::~NormalGameplayState()
{

}

void NormalGameplayState::OnStartState()
{
	auto screen = Screen::Get();

	// 入力の作成
	m_systemInput = InputBindingFactory::SystemInputFactory().Create(DefaultSpawnDesc());

	m_manualSprite = std::make_unique<Sprite>();

	// キャンバス
	Canvas* pCanvas = GetOwner()->GetCanvas();

	// キャンバスへ登録
	pCanvas->AddSprite(m_manualSprite.get());

	// スプライトのテクスチャ設定
	m_manualSprite->Initialize(GetOwner()->GetCommonResources()->GetResourceManager()->CreateTexture(
		TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::UI_GUIDE_INGAME_KEYBOARD)));


	// スプライトの座標設定
	m_manualSprite->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + 200.0f * screen->GetScreenScale(), screen->GetBottomF() - 40.0f * screen->GetScreenScale()));

	// スプライトの拡大率設定
	m_manualSprite->SetScale(0.2f * screen->GetScreenScale());
	
	// ゲームパッドへの接続されているかどうかの初期化
	m_isPrevConnectedGamepad = false;

	// **** タスク管理へ登録 ****

	// ステージ管理の更新を開始する
	GetOwner()->GetStageManager()->StartUpdating();

}

void NormalGameplayState::OnExitState()
{
	// キャンバスから削除
	GetOwner()->GetCanvas()->RemoveSprite(m_manualSprite.get());


}

void NormalGameplayState::OnUpdate(float deltaTime)
{
	
	// 入力の更新処理
	m_systemInput->Update(
	GetOwner()->GetCommonResources()->GetKeyboardTracker(), 
	GetOwner()->GetCommonResources()->GetMouseTracker(), 
	GetOwner()->GetCommonResources()->GetGamePadTracker());


	// タスク管理の更新処理
	GetOwner()->GetTaskManager()->Update(deltaTime);


	if (m_systemInput->IsInput(InputActionType::SystemActionID::PAUSE, InputSystem<InputActionType::SystemActionID>::InputOption::PRESSED))
	{
		// ポーズ状態にする
		GetStateMachine()->ChangeState<PauseGameplayState>();
		GetOwner()->GetCommonResources()->SetCopyScreenRequest(true);
	}

	// ガイドUIの変更を試みる
	TryChangeCurrentGuideUI();


	
}

void NormalGameplayState::OnDraw()
{
	// 現在のカメラの取得
	const Camera* pCurrentCamera = MainCamera::GetInstance()->GetCamera();
	// エフェクトにカメラを設定する
	GameEffectController::GetInstance()->SetCamera(pCurrentCamera);
	// タスク管理の描画処理
	GetOwner()->GetTaskManager()->Render(*pCurrentCamera);


}

/**
 * @brief 現在のガイドガイドUIの変更を試みる 
 * 
 * 簡易実装用の後にクラス化する
 * 
 * @returns true 成功
 * @returns false 失敗
 */
bool NormalGameplayState::TryChangeCurrentGuideUI()
{
	bool requestChange = false;
	bool changePC = true;;

	// ゲームパッドの接続されたらパッドガイドにに切り替える
	if (!m_isPrevConnectedGamepad &&  GetOwner()->GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = false;
		requestChange = true;
		m_isPrevConnectedGamepad = true;
	}

	// ゲームパッドが接続されなくなったらPCガイドに切り替える
	if (m_isPrevConnectedGamepad && !GetOwner()->GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = true;
		requestChange = true;
		m_isPrevConnectedGamepad = false;
	}

	if (requestChange)
	{
		std::string filePath;

		if (changePC)
		{
			filePath = TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::UI_GUIDE_INGAME_KEYBOARD);
		}

		else 
		{
			filePath = TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::UI_GUIDE_INGAME_GAMEPAD);
		}

		// 拡大率と座標を保持する
		float scale = m_manualSprite->GetScale();
		SimpleMath::Vector2 position = m_manualSprite->GetPosition();
		m_manualSprite->Initialize(GetOwner()->GetCommonResources()->GetResourceManager()->CreateTexture(filePath));
		m_manualSprite->SetScale(scale);
		m_manualSprite->SetPosition(position);

		return true;
	}

	return false;
}

