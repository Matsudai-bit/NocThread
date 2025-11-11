/*****************************************************************//**
 * @file    IdlePlayerState.h
 * @brief   待機状態のプレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "IdlePlayerState.h"

#include "Game/GameObjects/Player/Player.h"

#include "Game/GameObjects/Player/State/WalkPlayerState/WalkPlayerState.h"
#include "Game/GameObjects/Player/State/WireActionPlayerState/WireActionPlayerState.h"
#include "Library/ImaseLib/DebugFont.h"

#include "Game/Common/CommonResources/CommonResources.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
IdlePlayerState::IdlePlayerState()
{

}



/**
 * @brief デストラクタ
 */
IdlePlayerState::~IdlePlayerState()
{

}

/**
 * @brief 開始処理
 * 
 */
void IdlePlayerState::OnStartState()
{
	// アニメーションを変更
	GetOwner()->ChangeAnimation(Player::ANIM_IDLENG);
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void IdlePlayerState::OnUpdate(float deltaTime)
{
	// 物理の適用
	GetOwner()->ApplyPhysic(deltaTime);

	// 移動入力されているかどうか
	if (GetOwner()->IsMovingRequested())
	{
		GetOwner()->RequestChangeState(Player::State::WALKING);
	}

	// プレイヤーの入力機構の取得
	auto inputSystem = GetOwner()->GetPlayerInput();

	// ワイヤー発射の入力がされたかどうか
	if (inputSystem->IsInput(InputActionType::PlyayerActionID::WIRE_SHOOTING))
	{
		// ワイヤー発射が可能かどうか
		if (!GetOwner()->IsGround() && GetOwner()->CanShootWire())
		{
			// ワイヤー発射状態にする
			GetOwner()->RequestChangeState(Player::State::WIRE_SHOOTING);
		}
	}

	// 移動する
	GetOwner()->Move(deltaTime);

	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"Idle");

}

/**
 * @brief 描画処理
 * 
 */
void IdlePlayerState::OnDraw()
{
}

