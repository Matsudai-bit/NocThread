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
#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"


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
	// 入力の作成
	m_systemInput = InputBindingFactory::CreateSystemInput();
	
}

void NormalGameplayState::OnUpdate(float deltaTime)
{
	// 入力の更新処理
	m_systemInput->Update(
	GetOwner()->GetCommonResources()->GetKeyboardTracker(), 
	GetOwner()->GetCommonResources()->GetMouseTracker(), 
	GetOwner()->GetCommonResources()->GetGamePadTracker());

	auto keyboardStateTracker = GetOwner()->GetCommonResources()->GetKeyboardTracker();

	// ゲームオブジェクトの更新処理
	GetOwner()->UpdateInGameObjects(deltaTime);


	if (m_systemInput->IsInput(InputActionType::SystemActionID::PAUSE, InputSystem<InputActionType::SystemActionID>::InputOption::PRESSED))
	{
		// ポーズ状態にする
		GetStateMachine()->ChangeState<PoseGameplayState>();
	}
}

void NormalGameplayState::OnDraw()
{
	GetOwner()->DrawInGameObjects();

}

