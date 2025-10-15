/*****************************************************************//**
 * @file    WireThrowingPlayerState.cpp.h
 * @brief   ワイヤーで投げる状態のプレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "WireThrowingPlayerState.cpp.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/GameObjects/Player/State/WalkPlayerState/WalkPlayerState.h"
#include "Game/Common/Event/WireSystemObserver/WireSystemSubject.h"
#include "Game/Common/Event/WireSystemObserver/WireEventData.h"

#include "Library/MyLib/DirectXMyToolKit/DirectXMyToolKit.h"

#include "Game/Common/Camera/Camera.h"
#include "Library/MyLib/Ray/Ray.h"

#include "Library/ImaseLib/DebugFont.h"

#include "Game/Common/Screen.h"

using namespace DirectX;



// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
WireThrowingPlayerState::WireThrowingPlayerState()
{

}



/**
 * @brief デストラクタ
 */
WireThrowingPlayerState::~WireThrowingPlayerState()
{

}

/**
 * @brief 状態の開始処理
 *
 */
void WireThrowingPlayerState::OnStartState()
{
}

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime　経過時間
 */
void WireThrowingPlayerState::OnUpdate(float deltaTime)
{

	WireEventData eventData;
	eventData.grabPos = GetOwner()->CalcGrabbingPosition();
	eventData.pOwner = GetOwner();


	MyLib::Ray ray = GetOwner()->GetWireShootingRay();

	SimpleMath::Vector3 throwImpulse = ray.direction * 100.0f;

	// 投げていることを通知
	GetOwner()->GetWireSystem()->NotifyThrow(eventData, throwImpulse);

	// 移動
	GetOwner()->Move(deltaTime);

	// 離す
	GetOwner()->ReleaseWire();
	// ワイヤーシステムの登録を削除する
	GetOwner()->GetWireSystem()->AllRemoveObserver();

	// 状態を待機状態に変更
	GetOwner()->RequestChangeState(Player::State::IDLE);

	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"WireThrowing");

	
}



