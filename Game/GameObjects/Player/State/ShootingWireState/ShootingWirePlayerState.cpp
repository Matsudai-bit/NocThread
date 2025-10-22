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
#include "Game/Common/Input/InputSystem/InputSystem.h"
#include "Game/Common/Input/InputActionType/InputActionType.h"

#include "Game/GameObjects/Wire/Wire.h"

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

	// Y軸の速度をなくす
	//GetOwner()->ResetVelocityY();
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void ShootingWirePlayerState::OnUpdate(float deltaTime)
{

	// マウスを離したら強制終了する
	if (!GetOwner()->GetWire()->IsActive() ||
		GetOwner()->GetPlayerInput()->IsInput(InputActionType::PlyayerActionID::RELEASE_WIRE, InputSystem<InputActionType::PlyayerActionID>::InputOption::RELEASED))
	{
		GetOwner()->RequestChangeState(Player::State::IDLE);
	}


	// ふわっとする処理をする
	GetOwner()->AddForceToVelocityY(FLY_UPP_ACCELERATION * deltaTime);
	GetOwner()->ApplyGravity(deltaTime);

	GetOwner()->Move(deltaTime);

	// ワイヤーの原点の設定
	GetOwner()->GetWire()->SetOriginPosition(GetOwner()->GetPosition());

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

/**
 * @brief 状態終了時に呼ばれる
 * 
 */
void ShootingWirePlayerState::OnExitState()
{
	GetOwner()->SetWireCollisionFunction([](const GameObject* pHitObject) {});
}

