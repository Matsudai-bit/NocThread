/*****************************************************************//**
 * @file    JumpingPlayerState.h
 * @brief   ジャンプ状態のプレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "JumpingPlayerState.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/GameObjects/Player/State/WireActionPlayerState/WireActionPlayerState.h"
#include "Game/GameObjects/Player/State/ShootingWireState/ShootingWirePlayerState.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"

#include "Game/Common/WireTargetFinder/WireTargetFinder.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
JumpingPlayerState::JumpingPlayerState()
	: m_isJumping{ false }
{

}



/**
 * @brief デストラクタ
 */
JumpingPlayerState::~JumpingPlayerState()
{

}


/**
 * @brief 状態開始に呼ばれる
 */
void JumpingPlayerState::OnStartState()
{
	// 状態を設定
	GetOwner()->SetState(Player::State::JUMPING);
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void JumpingPlayerState::OnUpdate(float deltaTime)
{
	if (m_isJumping == false)
	{
		// ジャンプ
		GetOwner()->Jump(deltaTime);
		m_isJumping = true;
	}


	// 物理の適用
	GetOwner()->ApplyPhysic(deltaTime);

	// 移動入力の適用
	GetOwner()->ApplyMoveInput(deltaTime);
	// 移動
	GetOwner()->Move(deltaTime);

	if (GetOwner()->IsGround())
	{
		GetOwner()->GetStateMachine()->ChangeState<IdlePlayerState>();
	}

	if (GetOwner()->GetPlayerInput()->IsInput(InputActionType::PlyayerActionID::WIRE_SHOOTING))
	{
		if (!GetOwner()->IsGround() && GetOwner()->CanShootWire())
		{
			GetOwner()->GetStateMachine()->ChangeState<ShootingWirePlayerState>(GetOwner()->GetWireTargetFinder()->GetTargetPosition());
		}
	}

	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"Walk");

}

/**
 * @brief 描画処理
 * 
 */
void JumpingPlayerState::OnDraw()
{
}

