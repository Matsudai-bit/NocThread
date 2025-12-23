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

#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/GameObjects/Player/State/WalkPlayerState/WalkPlayerState.h"
#include "Game/Common/Event/WireSystemObserver/WireSystemSubject.h"
#include "Game/Common/Event/WireSystemObserver/WireEventData.h"
#include "Game/GameObjects/Player/State/WireThrowing/WireThrowingPlayerState.cpp.h"
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Game/Common/Camera/Camera.h"

#include "Game/Common/Screen.h"
#include "Library/MyLib/Ray/Ray.h"
#include "Game/GameObjects/Wire/Wire.h"
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

	//// テスト **************************************:::

	//using namespace SimpleMath;
	//const float EPSILON = 0.0001f;


	//// カレントとターゲットの方向をXZ平面に投影
	//Vector3 currentForward = GetOwner()->GetForward();
	//currentForward.y = 0.0f;
	//using namespace SimpleMath;
	//// 飛ばすスクリーン座標
	//float screenX = Screen::Get()->GetCenterXF();
	//float screenY = Screen::Get()->GetCenterYF();
	//MyLib::Ray ray = GetOwner()->CalcScreenToWorldRay(screenX, screenY, *GetOwner()->GetCamera(), GetOwner()->GetProj());

	//Vector3 moveDir = ray.direction;
	//moveDir.y = 0.0f;

	//currentForward.Normalize();
	//moveDir.Normalize();

	//// ほぼ同じならスキップ
	//if ((moveDir - currentForward).LengthSquared() >= EPSILON)
	//{
	//	// クォータニオンで向きを補間
	//	Quaternion currentRotation = GetOwner()->GetRotate();
	//	Quaternion targetRotation = Quaternion::CreateFromRotationMatrix(Matrix::CreateWorld(Vector3::Zero, moveDir, Vector3::Up));


	//	//float t = deltaTime / Player::ROTATION_SPEED;
	//	Quaternion result = Quaternion::Slerp(currentRotation, targetRotation, 1.0f); // 補間率を調整
	//	GetOwner()->SetRotate(result);


	//}
	


	// *******************************************::

	// マウスを離したら
	if (mouseTrack->leftButton == Mouse::ButtonStateTracker::RELEASED)
	{
		//GetOwner()->ReleaseWire();
		GetOwner()->RequestChangeState(Player::State::WIRE_THROWING);
	}

	//GetOwner()->GetCommonResources()->GetDebugFont()->AddString(10, 90, Colors::White, L"WireGrabbing");

}



