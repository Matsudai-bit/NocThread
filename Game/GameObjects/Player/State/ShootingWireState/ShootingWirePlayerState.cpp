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
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
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
ShootingWirePlayerState::ShootingWirePlayerState(const DirectX::SimpleMath::Vector3& targetPosition)
	: m_isJumping		{ false }
	, m_targetPosition	{ targetPosition }
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
	// 状態を設定
	GetOwner()->SetState(Player::State::WIRE_SHOOTING);

	// ワイヤーを飛ばす
	GetOwner()->ShootWire(m_targetPosition);

	// ワイヤー衝突時に呼ばれる関数の登録
	GetOwner()->SetWireCollisionFunction([this](const GameObject* pHitObject) { OnCollisionWire(pHitObject); });

	// Y軸の速度をなくす
	//GetOwner()->ResetVelocityY();

	// SEの再生
	SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_PLAYER_SHOOTINGWIRE), false);

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
		GetOwner()->GetWire()->Reset();

		GetOwner()->GetStateMachine()->ChangeState<IdlePlayerState>();
	}


	// ふわっとする処理をする
	GetOwner()->AddForceToVelocityY(FLY_UPP_ACCELERATION * deltaTime);
	GetOwner()->ApplyGravity(deltaTime);

	GetOwner()->Move(deltaTime);

	// ワイヤーの原点の設定
	GetOwner()->GetWire()->SetOriginPosition(GetOwner()->GetTransform()->GetPosition());

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
	UNREFERENCED_PARAMETER(pHitObject);

	GetOwner()->GetStateMachine()->ChangeState<WireActionPlayerState>();

	//if (pHitObject->GetTag() == GameObjectTag::WALL ||
	//pHitObject->GetTag() == GameObjectTag::BUILDING ||
	//pHitObject->GetTag() == GameObjectTag::ESCAPE_HELICOPTER)
	//{
	//	GetOwner()->RequestChangeState(Player::State::WIRE_ACTION);
	//}
}

/**
 * @brief 状態終了時に呼ばれる
 * 
 */
void ShootingWirePlayerState::OnExitState()
{
	GetOwner()->SetWireCollisionFunction([](const GameObject* pHitObject) {UNREFERENCED_PARAMETER(pHitObject); });
}

