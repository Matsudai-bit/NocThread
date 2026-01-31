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
#include "Game/Common/Framework/Input/InputActionMap/InputActionMap.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/StageManager/StageManager.h"
#include "Game/Common/GameplayLogic/PauseNavigator/PauseNavigator.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/InputDeviceSpriteResolver/InputDeviceSpriteResolver.h"


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
{

}

/**
 * @brief デストラクタ
 */
NormalGameplayState::~NormalGameplayState()
{

}

/**
 * @brief 状態の開始
 */
void NormalGameplayState::OnStartState()
{
	// ナビゲーターの作成
	CreatePauseNavigator();
	// ステージ管理の更新を開始する
	GetOwner()->GetStageManager()->StartUpdating();
}


/**
 * @brief 状態の更新処理
 * 
 * @param[in] deltaTime　前フレームからの経過時間
 */
void NormalGameplayState::OnUpdate(float deltaTime)
{
	// タスク管理の更新処理
	GetOwner()->GetTaskManager()->Update(deltaTime);
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
 * @brief ナビゲーターの作成
 */
void NormalGameplayState::CreatePauseNavigator()
{
	// ポーズナビゲーターの作成
	m_pauseNavigator = std::make_unique<PauseNavigator>();

	// ポーズナビゲーターの初期化処理
	m_pauseNavigator->Initialize(
		GetOwner()->GetCommonResources()->GetResourceManager(),
		GetOwner()->GetCommonResources()->GetInputDeviceSpriteResolver(),
		GetOwner()->GetCanvas(),
		GetOwner()->GetCommonResources()->GetInputManager());

	m_pauseNavigator->SetInputCallback([this](const InputEventData& data) {OnOpenPause(data); });
}

/**
 * @brief ポーズ画面を開く処理
 * 
 * @param[in] data 入力イベントデータ
 */
void NormalGameplayState::OnOpenPause(const InputEventData& data)
{
	if (data.inputOption.pressed)
	{
		// ポーズ状態にする
		GetStateMachine()->ChangeState<PauseGameplayState>();
		GetOwner()->GetCommonResources()->SetCopyScreenRequest(true);

	}
}

