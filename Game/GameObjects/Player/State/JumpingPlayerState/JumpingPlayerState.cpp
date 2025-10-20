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
#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/ImaseLib/DebugFont.h"

#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"

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
	
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void JumpingPlayerState::OnUpdate(float deltaTime)
{
	// マウストラック
	auto mouseTrack = GetOwner()->GetCommonResources()->GetMouseTracker();

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
		GetOwner()->RequestChangeState(Player::State::IDLE);
	}

	if (mouseTrack->leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		if (GetOwner()->CanShootWire())
		{
			GetOwner()->ShootWire();
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

