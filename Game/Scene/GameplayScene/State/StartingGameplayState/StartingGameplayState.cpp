/*****************************************************************//**
 * @file    StartingGameplayState.h
 * @brief   ゲームプレイシーンの開始状態に関するソースファイル
 *
 * @author  松下大暉
 * @date    2026/01/27
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StartingGameplayState.h"



// ライブラリ

// ファクトリー関連

// データベース関連

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Common/Framework/TaskManager/TaskManager.h"

// ゲームプレイロジック関連

// グラフィック関連
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"

// 状態
#include "Game/Scene/GameplayScene/State/NormalGameplayState/NormalGameplayState.h"

// 所有者
#include "Game/Scene/GameplayScene/GameplayScene.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
StartingGameplayState::StartingGameplayState()
{

}



/**
 * @brief デストラクタ
 */
StartingGameplayState::~StartingGameplayState()
{

}





/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @return なし
 */
void StartingGameplayState::OnUpdate(float deltaTime)
{
	GetOwner()->GetTaskManager()->Update(deltaTime);

	auto kt = GetOwner()->GetCommonResources()->GetKeyboardTracker();

	if (kt->IsKeyReleased(Keyboard::Space))
	{
		GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::GAME_START);
		GetStateMachine()->ChangeState<NormalGameplayState>();
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StartingGameplayState::OnDraw()
{
	GetOwner()->GetTaskManager()->Render(*(MainCamera::GetInstance()->GetCamera()));

}

