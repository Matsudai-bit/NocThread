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
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"

#include "Game/GameObjects/Wire/Wire.h"

// パラメータ
#include "Game/Common/Database/PlayerParameter.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
WireActionPlayerState::WireActionPlayerState()
	: m_effectId{ -1 }
	, m_pConcentrationLines{ nullptr }
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
	GetOwner()->ChangeAnimation(PlayerParameter::ANIM_WIREACTION);

	auto concentrationLines = std::make_unique<ConcentrationLines>(GetOwner()->GetCommonResources()->GetDeviceResources(), 0.24f, 0.3f);
	m_pConcentrationLines = concentrationLines.get();
	auto clip = GameEffectManager::EffectClip(true);

	m_effectId = GameEffectController::GetInstance()->PlayEffect(std::move(concentrationLines), clip);

	// 状態を設定
	GetOwner()->SetState(Player::State::WIRE_ACTION);
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void WireActionPlayerState::OnUpdate(float deltaTime)
{
	GetOwner()->SetVelocity(GetOwner()->GetWire()->GetStartVelocity());
	// ワイヤーアクション挙動
	GetOwner()->BehaviourWireAction(deltaTime, 1.4f);

	// 物理の適用
	//GetOwner()->ApplyPhysic(deltaTime);

	// 移動入力の適用
	//GetOwner()->ApplyMoveInput(deltaTime);

	GetOwner()->RotateForMoveDirection(deltaTime);
	m_pConcentrationLines->SetLineLengthRate(GetOwner()->GetVelocity().LengthSquared()/ 3100.f);
	GetOwner()->ResetVelocity();

	// 移動
	GetOwner()->Move(deltaTime);

	if (GetOwner()->GetPlayerInput()->IsInput(InputActionType::PlyayerActionID::RELEASE_WIRE, InputSystem< InputActionType::PlyayerActionID>::InputOption::RELEASED))
	{
		//GetOwner()->GetWire()->Reset();
		GetOwner()->GetStateMachine()->ChangeState<WalkPlayerState>();

	}
	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"WireAction");

}

void WireActionPlayerState::OnExitState()
{
	GetOwner()->ReleaseWire();

	GameEffectController::GetInstance()->StopEffect(m_effectId);
}



