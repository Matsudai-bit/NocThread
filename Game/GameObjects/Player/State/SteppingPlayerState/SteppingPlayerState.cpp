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

#include "Game/Common/Camera/Camera.h"

#include "Library/ImaseLib/DebugFont.h"
#include "Library/MyLib/DirectXMyToolKit/DirectXMyToolKit.h"


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

	m_startPosition = GetOwner()->GetPosition();
	GetOwner()->ResetVelocity();
	m_targetPosition = GetOwner()->GetPosition() + m_impulseDirection * 9.0f;
}

/**
 * @brief 更新処理
 * 
 * @param[in] elapsedTime　経過時間
 */
void SteppingPlayerState::OnUpdate(float elapsedTime)
{
	using namespace SimpleMath;

	m_targetCounter.UpperTime(elapsedTime);

	m_currentLerpValue = m_targetCounter.GetElapsedTime() / 0.2f;
	
	Vector3 currentPosition = Vector3::Lerp(m_startPosition, m_targetPosition, m_currentLerpValue);

	//GetOwner()->SetPosition(currentPosition);

	GetOwner()->SetVelocity((currentPosition - GetOwner()->GetPosition())/elapsedTime);

	GetOwner()->Move(elapsedTime);

	if (m_targetCounter.GetElapsedTime() >= 0.2f)
	{
		GetOwner()->SetPosition(m_targetPosition);
		GetOwner()->RequestChangeState(Player::State::IDLE);
	}

	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"Walk");

}

/**
 * @brief 描画処理
 * 
 */
void SteppingPlayerState::OnDraw()
{
}

