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


#include "Game/Scene/GameplayScene/GameplayScene.h"
#include "Game/Scene/GameplayScene/State/PoseGameplayState/PoseGameplayState.h"

#include "Game/Common/CommonResources/CommonResources.h"

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

void NormalGameplayState::OnStartState()
{

}

void NormalGameplayState::OnUpdate(float elapsedTime)
{
	auto keyboardStateTracker = GetOwner()->GetCommonResources()->GetKeyboardTracker();

	// ゲームオブジェクトの更新処理
	GetOwner()->UpdateInGameObjects(elapsedTime);


	if (keyboardStateTracker->IsKeyPressed(Keyboard::Escape))
	{
		// ポーズ状態にする
		GetStateMachine()->ChangeState<PoseGameplayState>();
	}
}

void NormalGameplayState::OnDraw()
{
	GetOwner()->DrawInGameObjects();

}

