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

// ゲームオブジェクト関連
#include "Game/GameObjects/Player/Player.h"

// プレイヤーの状態関連
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/GameObjects/Player/State/WalkPlayerState/WalkPlayerState.h"

// ライブラリ関連
#include "Library/MyLib/DirectXMyToolKit/DirectXUtils.h"
#include "Library/MyLib/Ray/Ray.h"
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/Event/WireSystemObserver/WireSystemSubject.h"
#include "Game/Common/Framework/Event/WireSystemObserver/WireEventData.h"

// グラフィック関連
#include "Game/Common/Graphics/Camera/Camera.h"
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

	// 状態を設定
	GetOwner()->SetState(Player::State::WIRE_THROWING);
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
	GetOwner()->GetStateMachine()->ChangeState<IdlePlayerState>();

	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"WireThrowing");

	
}



