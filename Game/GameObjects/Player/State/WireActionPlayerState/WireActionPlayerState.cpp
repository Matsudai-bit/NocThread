/*****************************************************************//**
 * @file    WireActionPlayerState.h
 * @brief   ワイヤーアクション状態のプレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "WireActionPlayerState.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/GameObjects/Player/State/WalkPlayerState/WalkPlayerState.h"
#include "Library/ImaseLib/DebugFont.h"

#include "Game/GameObjects/Wire/Wire.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
WireActionPlayerState::WireActionPlayerState()
{

}



/**
 * @brief デストラクタ
 */
WireActionPlayerState::~WireActionPlayerState()
{

}

/**
 * @brief 状態の開始処理
 * 
 */
void WireActionPlayerState::OnStartState()
{
	GetOwner()->ChangeAnimation(Player::ANIM_WIREACTION);
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void WireActionPlayerState::OnUpdate(float deltaTime)
{
	// マウストラック
	auto mosueTrack = GetOwner()->GetCommonResources()->GetMouseTracker();

	GetOwner()->SetVelocity(GetOwner()->GetWire()->GetStartVelocity());

	// ワイヤーアクション挙動
	GetOwner()->BehaviourWireAction(deltaTime);

	// 物理の適用
	//GetOwner()->ApplyPhysic(deltaTime);

	// 移動入力の適用
	//GetOwner()->ApplyMoveInput(deltaTime);

	GetOwner()->RotateForMoveDirection(deltaTime);

	// 移動
	GetOwner()->Move(deltaTime);

	if (mosueTrack->leftButton == Mouse::ButtonStateTracker::RELEASED)
	{
		GetOwner()->RequestChangeState(Player::State::WALKING);

	}
	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"WireAction");

}

void WireActionPlayerState::OnExitState()
{
	GetOwner()->ReleaseWire();
}



