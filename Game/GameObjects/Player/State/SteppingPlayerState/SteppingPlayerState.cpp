/*****************************************************************//**
 * @file    SteppingPlayerState.h
 * @brief   ジャンプ状態のプレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SteppingPlayerState.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/GameObjects/Player/State/WireActionPlayerState/WireActionPlayerState.h"
#include "Game/Common/CommonResources/CommonResources.h"

#include "Game/Common/Camera/PlayerCamera/PlayerCamera.h"

#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Library/MyLib/DirectXMyToolKit/DirectXUtils.h"


using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
SteppingPlayerState::SteppingPlayerState()
	: m_currentLerpValue{}
{

}



/**
 * @brief デストラクタ
 */
SteppingPlayerState::~SteppingPlayerState()
{

}


/**
 * @brief 状態開始に呼ばれる
 */
void SteppingPlayerState::OnStartState()
{
	// カメラの正面方向を取得する
	m_impulseDirection = GetOwner()->GetCamera()->GetTarget() - GetOwner()->GetCamera()->GetEye();
	m_impulseDirection.Normalize();

	m_currentLerpValue = 0.0f;

	m_startPosition = GetOwner()->GetTransform()->GetPosition();
	GetOwner()->ResetVelocity();
	m_targetPosition = GetOwner()->GetTransform()->GetPosition() + m_impulseDirection * 9.0f;
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void SteppingPlayerState::OnUpdate(float deltaTime)
{
	using namespace SimpleMath;

	const float STEP_TIME = 0.2f;

	m_targetCounter.UpperTime(deltaTime);

	m_currentLerpValue = m_targetCounter.GetElapsedTime() / STEP_TIME;

	Vector3 currentPosition = Vector3::Lerp(m_startPosition, m_targetPosition, m_currentLerpValue);


	GetOwner()->SetVelocity((currentPosition - GetOwner()->GetTransform()->GetPosition()) / deltaTime);

	GetOwner()->Move(deltaTime);

	if (m_targetCounter.GetElapsedTime() >= STEP_TIME)
	{
		GetOwner()->GetTransform()->SetPosition(m_targetPosition);
		GetOwner()->RequestChangeState(Player::State::IDLE);
	}

}

/**
 * @brief 描画処理
 * 
 */
void SteppingPlayerState::OnDraw()
{
}

