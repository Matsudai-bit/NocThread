/*****************************************************************//**
 * @file    WireGrabbingPlayerState.cpp.h
 * @brief   ワイヤーで掴んでいる状態のプレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "WireGrabbingPlayerState.cpp.h"

// ライブラリ関連
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Library/MyLib/Ray/Ray.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Wire/Wire.h"

// プレイヤーの状態
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/GameObjects/Player/State/WalkPlayerState/WalkPlayerState.h"
#include "Game/GameObjects/Player/State/WireThrowing/WireThrowingPlayerState.cpp.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/Event/WireSystemObserver/WireSystemSubject.h"
#include "Game/Common/Framework/Event/WireSystemObserver/WireEventData.h"

// グラフィック関連
#include "Game/Common/Camera/Camera.h"
#include "Game/Common/Screen.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
WireGrabbingPlayerState::WireGrabbingPlayerState()
{

}



/**
 * @brief デストラクタ
 */
WireGrabbingPlayerState::~WireGrabbingPlayerState()
{

}

/**
 * @brief 状態の開始処理
 *
 */
void WireGrabbingPlayerState::OnStartState()
{

	// 状態を設定
	GetOwner()->SetState(Player::State::WIRE_GRABBING);

	WireEventData eventData;
	eventData.grabPos = GetOwner()->CalcGrabbingPosition();
	eventData.pOwner = GetOwner();

	GetOwner()->GetWireSystem()->NotifyGrab(eventData);
}

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime　経過時間
 */
void WireGrabbingPlayerState::OnUpdate(float deltaTime)
{
	// マウストラック
	auto mouseTrack = GetOwner()->GetCommonResources()->GetMouseTracker();

	WireEventData eventData;
	eventData.grabPos = GetOwner()->CalcGrabbingPosition();
	eventData.pOwner = GetOwner();

	// ワイヤーの終点位置を設定
	GetOwner()->GetWire()->GetBackPivot()->SetPosition(eventData.grabPos);

	// ワイヤーの始点位置を設定

	GetOwner()->GetWire()->GetFrontPivot()->SetPosition(GetOwner()->GetTransform()->GetPosition());

	// 掴んでいることを通知
	GetOwner()->GetWireSystem()->NotifyHover(eventData);

	// 入力の受付
	GetOwner()->ApplyMoveInput(deltaTime);

	// 移動
	GetOwner()->Move(deltaTime);


	GetOwner()->ApplyPhysic(deltaTime);

	GetOwner()->RotateForMoveDirection(deltaTime);

	// マウスを離したら
	if (mouseTrack->leftButton == Mouse::ButtonStateTracker::RELEASED)
	{
		//GetOwner()->ReleaseWire();
		GetOwner()->GetStateMachine()->ChangeState<WireThrowingPlayerState>();
	}

	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"WireGrabbing");

}



