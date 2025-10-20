/*****************************************************************//**
 * @file    ShootingWirePlayerState.h
 * @brief   ワイヤー発射状態のプレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/21
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ShootingWirePlayerState.h"

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
ShootingWirePlayerState::ShootingWirePlayerState()
	: m_isJumping{ false }
{

}



/**
 * @brief デストラクタ
 */
ShootingWirePlayerState::~ShootingWirePlayerState()
{

}


/**
 * @brief 状態開始に呼ばれる
 */
void ShootingWirePlayerState::OnStartState()
{
	// ワイヤーを飛ばす
	GetOwner()->ShootWire();

	// ワイヤー衝突時に呼ばれる関数の登録
	GetOwner()->SetWireCollisionFunction([this](const GameObject* pHitObject) { OnCollisionWire(pHitObject); });
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void ShootingWirePlayerState::OnUpdate(float deltaTime)
{
	// マウストラック
	auto mouseTrack = GetOwner()->GetCommonResources()->GetMouseTracker();

	// マウスを離したら強制終了する

	// ふわっとする処理をする

}

/**
 * @brief 描画処理
 * 
 */
void ShootingWirePlayerState::OnDraw()
{
}

/**
 * @brief ワイヤーが衝突した時によばれる
 * 
 * @param[in] pHitObject 衝突したオブジェクト
 */
void ShootingWirePlayerState::OnCollisionWire(const GameObject* pHitObject)
{
	if (pHitObject->GetTag() == GameObjectTag::WALL ||
	pHitObject->GetTag() == GameObjectTag::BUILDING ||
	pHitObject->GetTag() == GameObjectTag::ESCAPE_HELICOPTER)
	{
		GetOwner()->RequestChangeState(Player::State::WIRE_ACTION);
	}
}

