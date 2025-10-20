/*****************************************************************//**
 * @file    WalkPlayerState.h
 * @brief   歩行状態のプレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "WalkPlayerState.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/GameObjects/Player/State/WireActionPlayerState/WireActionPlayerState.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/ImaseLib/DebugFont.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
WalkPlayerState::WalkPlayerState()
{

}



/**
 * @brief デストラクタ
 */
WalkPlayerState::~WalkPlayerState()
{

}

/**
 * @brief 開始処理
 */
void WalkPlayerState::OnStartState()
{
	// アニメーションを変更
	GetOwner()->ChangeAnimation(Player::ANIM_WALKING);
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void WalkPlayerState::OnUpdate(float deltaTime)
{
	// マウストラック
	auto mouseTrack = GetOwner()->GetCommonResources()->GetMouseTracker();

	// 物理の適用
	GetOwner()->ApplyPhysic(deltaTime);

	// 移動入力の適用
	GetOwner()->ApplyMoveInput(deltaTime);

	if (GetOwner()->GetVelocity().LengthSquared() <= 0.0f)
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

	// 移動
	GetOwner()->Move(deltaTime);

	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"Walk");

}

/**
 * @brief 描画処理
 * 
 */
void WalkPlayerState::OnDraw()
{
}

