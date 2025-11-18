/*****************************************************************//**
 * @file    PlayerCamera.cpp
 * @brief   プレイヤー視点カメラの制御クラスの実装
 *
 * プレイヤーの位置や方向に応じてカメラの視点と注視点を計算し、
 * マウス入力によって回転を加えることができるカメラ機能を提供します。
 * DirectXTKのSimpleMathを活用し、簡潔に回転計算やベクトル処理を行っています。
 *
 * @author  松下大暉
 * @date    2025/6/15
 *********************************************************************/

#include "pch.h"
#include "PlayerCamera.h"
#include "Mouse.h"
#include "Game/GameObjects/Player/Player.h"

#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

#include "Library/MyLib/DirectXMathUtils/DirectXMathUtils.h"
#include "Library/MyLib/MathUtils/MathUtils.h"

#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/ImaseLib/DebugFont.h"

using namespace DirectX;



/**
 * @brief コンストラクタ
 * 
 * @param[in] windowWidth　 ウィンドウの幅
 * @param[in] windowHeight	ウィンドウの高さ
 * @param[in] pMouseTracker マウストラッカー
 */
PlayerCamera::PlayerCamera(int windowWidth, int windowHeight,  DirectX::Mouse::ButtonStateTracker* pMouseTracker)
	: m_screenW(windowWidth)
	, m_screenH(windowHeight)
	, m_sx{}, m_sy{}
	, m_mousePrevX{}, m_mousePrevY{}
	, m_pPlayer{ nullptr }
	, m_isStartUpdating{ true }
	, m_pMouseTracker{ pMouseTracker }
{
	SetWindowSize(windowWidth, windowHeight);
	SetUp(SimpleMath::Vector3::Up);  ///< 上方向ベクトルをY軸に設定

	// **** マウスの設定 ****
	// 非表示
	ShowCursor(false);

	// 相対モード
	Mouse::Get().SetMode(Mouse::MODE_RELATIVE);

	m_rotate = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(0.0f), 0.0f, 0.0f);


}

/**
 * @brief 初期化処理
 *
 * @param[in] pCommonResources　共通リソース
 * @param[in] pCollisionManager 衝突管理
 */
void PlayerCamera::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	SetCommonResources(pCommonResources);

	// 球コライダーの作成
	m_sphereCollider = std::make_unique<Sphere>();
	m_sphereCollider->SetRadius(COLLIDER_RADIUS);

	// 衝突管理への登録
	pCollisionManager->AddCollisionObjectData(this, m_sphereCollider.get());

	m_nextCameraTargetPosition = SimpleMath::Vector3::Zero;
}

/**
 * @brief カメラの更新処理
 *
 * 毎フレーム呼び出され、マウスの現在位置に応じてカメラの回転を更新し、
 * プレイヤーの位置と方向からビュー行列用の視点と注視点を設定します。
 */
void PlayerCamera::Update(float deltaTime)
{

	int mouseX = 0;
	int mouseY = 0;

	// マウス状態を取得
	auto gamepad = GetCommonResources()->GetGamePadTracker()->GetLastState();
	if (gamepad.IsConnected())
	{
		mouseX = static_cast<int>(gamepad.thumbSticks.rightX * 15.0f);
		mouseY = static_cast<int>(-gamepad.thumbSticks.rightY * 10.0f);
	}
	else
	{
		auto state = m_pMouseTracker->GetLastState();

		// 相対モードでばない（カメラFPS視点など）場合は処理をスキップ
		if (state.positionMode != Mouse::MODE_RELATIVE) return;

		mouseX = state.x;
		mouseY = state.y;
	}


	// 回転の更新
	UpdateRotation(mouseX, mouseY);

	// カメラ座標と注視点の更新処理
	UpdateCameraPosition(deltaTime, m_nextCameraTargetPosition);

	// 次のカメラの目標位置を計算
	m_nextCameraTargetPosition = CalculateCameraTargetPosition(m_rotate, m_pPlayer->GetPosition(), m_nextCameraTargetPosition);

	//// カメラの追従位置と最終設定を更新
	//GetCommonResources()->GetDebugFont()->AddString(10, 50, Colors::Red, L"EyePosition (%f, %f, %f) ", GetEye().x, GetEye().y, GetEye().z);
	//GetCommonResources()->GetDebugFont()->AddString(10, 100, Colors::Red, L"Target (%f, %f, %f)", GetTarget().x, GetTarget().y, GetTarget().z);

	// ビュー行列の算出
	CalcViewMatrix();

}

/**
 * @brief ウィンドウサイズの設定
 *
 * マウスの移動量を正規化するためのスケーリング係数を計算します。
 *
 * @param[in] windowWidth   画面の横幅（ピクセル）
 * @param[in] windowHeight  画面の縦幅（ピクセル）
 */
void PlayerCamera::SetWindowSize(int windowWidth, int windowHeight)
{
	m_sx = 1.0f / float(windowWidth);
	m_sy = 1.0f / float(windowHeight);
}

/**
 * @brief ウィンドウサイズの取得
 *
 * @param[out] windowWidth   画面の横幅（ピクセル）
 * @param[out] windowHeight  画面の縦幅（ピクセル）
 */
void PlayerCamera::GetWindowSize(int& windowWidth, int& windowHeight)
{
	windowWidth = m_screenW;
	windowHeight = m_screenH;
}

/**
 * @brief プレイヤーの設定
 *
 * カメラが追従する対象となるプレイヤーを動的に変更できます。
 *
 * @param[in] pPlayer プレイヤーへのポインタ
 */
void PlayerCamera::SetPlayer(const Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

/**
 * @brief 衝突処理
 *
 * @param[in] pHitObject	衝突したオブジェクト
 * @param[in] pHitCollider	衝突したコライダ^
 */
void PlayerCamera::OnCollision(GameObject* pHitObject, ICollider* pHitCollider)
{

	if (pHitObject->GetTag() == GameObjectTag::BUILDING)
	{
		OnCollisionWithBuilding(pHitObject, pHitCollider);
	}
}


/**
 * @brief 衝突をした直後に呼ばれる
 *
 */
void PlayerCamera::PostCollision()
{
	if (std::abs(m_overlapTotal.LengthSquared()) > OVERLAP_THRESHOLD)
	{

		SimpleMath::Vector3 overlap = m_overlapTotal;

		// **** 押し出し ****
		SimpleMath::Vector3 nextCameraTargetPositionTmp = PhysicsHelper::PushOut(overlap, GetEye());

		if (SimpleMath::Vector3::DistanceSquared(nextCameraTargetPositionTmp, m_nextCameraTargetPosition) > DISTANCE_SQUARED_THRESHOLD)
		{
			SetEye(nextCameraTargetPositionTmp);
		}
	}

}

void PlayerCamera::PreCollision()
{
	m_overlapTotal = SimpleMath::Vector3::Zero;

}

/**
 * @brief 回転の更新処理
 *
 * @param[in] mouseX　マウス座標X
 * @param[in] mouseY　マウス座標Y
 */
void PlayerCamera::UpdateRotation(int mouseX, int mouseY)
{
	// マウス入力による回転更新 (Motion関数)
	Motion(mouseX, mouseY);

	// 初回フレームの初期化
	if (m_isStartUpdating)
	{
		m_mousePrevX = mouseX;
		m_mousePrevY = mouseY;
		m_isStartUpdating = false;
	}
}

/**
 * @brief カメラの目標地点の算出
 * 
 * @param[in] cameraRotate					カメラの回転
 * @param[in] playerPos						プレイヤー座標
 * @param[in] prevNextCameraTargetPosition	前フレームの目標地点
 * @return 
 */
DirectX::SimpleMath::Vector3 PlayerCamera::CalculateCameraTargetPosition(
	const DirectX::SimpleMath::Quaternion& cameraRotate,
	const DirectX::SimpleMath::Vector3& playerPos,
	const DirectX::SimpleMath::Vector3& prevNextCameraTargetPosition)
{
	using namespace SimpleMath;

	// クォータニオンから行列に変換
	Matrix rot = Matrix::CreateFromQuaternion(cameraRotate);

	// プレイヤーの前方方向を取得
	Vector3 localForward = Vector3(0.0f, LOCAL_FORWARD_Y, LOCAL_FORWARD_Z); // 単位ベクトル (-1.0f)
	localForward.Normalize();
	Vector3 forward = Vector3::TransformNormal(localForward, rot);

	// 理想的な視点目標位置の計算
	// プレイヤーの後方 (距離 10.0f) + 高さ補正
	Vector3 nextPosition = prevNextCameraTargetPosition;
	Vector3 nextPositionTmp = playerPos + (-forward * FOLLOW_DISTANCE) + Vector3(0.0f, FOLLOW_OFFSET_Y, 0.0f);

	// 目標位置の更新（滑らかさ維持のため、わずかな変化でも更新を維持）
	if (Vector3::Distance(nextPositionTmp, nextPosition) >= TARGET_POSITION_UPDATE_THRESHOLD)
	{
		nextPosition = nextPositionTmp;
	}
	return nextPosition;
}

/**
 * @brief カメラ座標と注視点の更新処理
 * 
 * @param[in] deltaTime				経過時間
 * @param[in] targetCameraPosition	カメラの目標座標
 */
void PlayerCamera::UpdateCameraPosition(float deltaTime, const DirectX::SimpleMath::Vector3& targetCameraPosition)
{
	using namespace SimpleMath;


	// Upベクトルの計算
	// 注: Upベクトル計算は、CalculateCameraTarget 内の rot を使用するため、ロジックを調整する必要があります
	const Matrix rot = Matrix::CreateFromQuaternion(m_rotate);

	//  現在の視点位置を取得
	Vector3 currentEye = GetEye();

	// 目標位置への滑らかな追従処理 (線形補間 LERP / 減速運動)

	// 目標位置までのベクトル
	Vector3 eyeVelocity = (targetCameraPosition - currentEye) * FOLLOW_SPEED_MULTIPLIER * deltaTime;



	// プレイヤー位置と現在の視点位置の距離

	// 距離が一定以上離れていれば追従
	if (Vector3::Distance(targetCameraPosition, currentEye) > TARGET_STOP_DISTANCE_LARGE)
	{
		currentEye += eyeVelocity;
	}
	else if (Vector3::Distance(targetCameraPosition, currentEye) > TARGET_STOP_DISTANCE_SMALL)
	{
		// 収束時の振動防止のため、最後の微調整を行う
		currentEye = targetCameraPosition;
	}

	// **** 注視点の算出処理 ****
	// カメラ座標からプレイヤーを見た方向を算出
	Vector3 lookPlayerDirection = m_pPlayer->GetPosition() - currentEye;
	lookPlayerDirection.Normalize();
	// 注視点の算出
	Vector3 target = m_pPlayer->GetPosition() + lookPlayerDirection * LOOK_TARGET_DISTANCE;

	Vector3 up(0.0f, 1.0f, 0.0f);
	up = Vector3::TransformNormal(up, rot);

	// 1/10 にする (ロールを制御する特殊な補正。通常は行いませんが、元のコードを踏襲)
	up.x *= UP_VECTOR_CORRECTION_FACTOR;
	up.z *= UP_VECTOR_CORRECTION_FACTOR;


	// 基底クラスへの最終設定
	SetEye(currentEye);
	SetTarget(target);
	SetUp(up);

	// ゲームオブジェクト側の座標・コライダー更新
	SetPosition(GetEye());
	m_sphereCollider->Transform(GetEye()); // 追従目標位置でコライダーを更新
}

/**
 * @brief 建物との衝突判定処理
 * 
 * @param[in] pHitObject	衝突したオブジェクト
 * @param[in] pHitCollider	衝突したコライダ^
 */
void PlayerCamera::OnCollisionWithBuilding(GameObject* pHitObject, ICollider* pHitCollider)
{
	UNREFERENCED_PARAMETER(pHitObject);

	// 矩形にキャスト
	const AABB* pHitBox = dynamic_cast<const AABB*>(pHitCollider);

	SimpleMath::Vector3 overlapDir = GetTarget() - GetEye();
	overlapDir.Normalize();

	// 押し出しの計算
	SimpleMath::Vector3 overlap = CalcOverlap(*pHitBox, *m_sphereCollider, overlapDir);


	// 現在の押し出しの単位ベクトル
	SimpleMath::Vector3 totalOverlapNormal = m_overlapTotal;
	totalOverlapNormal.Normalize();

	// 押し出し方向ベクトル（累積方向 + 今回の方向）
	SimpleMath::Vector3 combinedDirection = totalOverlapNormal + overlapDir;

	if (combinedDirection.LengthSquared() < SQUARED_ZERO_THRESHOLD)
		return;

	combinedDirection.Normalize();

	// それぞれの押し出しを、合成方向に投影して必要量を求める
	float push1 = std::max(0.0f, overlap.Dot(combinedDirection));
	float push2 = std::max(0.0f, m_overlapTotal.Dot(combinedDirection));

	// より大きな押し出し量を採用
	float projectedDepth = std::max(push1, push2);

	// 合成押し出しを更新]
 	m_overlapTotal += combinedDirection * projectedDepth;

}


/**
 * @brief マウスによるカメラの回転処理
 *
 * マウスのドラッグに応じて、カメラのY軸（水平）とX軸（垂直）回転を行い、
 * 回転クォータニオンに反映します。
 *
 * @param[in] x 現在のマウスX座標（ピクセル）
 * @param[in] y 現在のマウスY座標（ピクセル）
 */
void PlayerCamera::Motion(int x, int y)
{
	// マウスの相対移動量を計算（スケール付き）
	float dx = static_cast<float>(x) * m_sx;
	float dy = static_cast<float>(y) * m_sy;

	// 変化があった場合のみ回転を更新
	if ((std::abs(dx) + std::abs(dy)) > MOUSE_INPUT_THRESHOLD)
	{

		// ラジアン変換（1画面分をπラジアンに相当）
		float rotY = dx * ROTATION_SENSITIVITY;
		float rotX = -dy * ROTATION_SENSITIVITY;

		// 現在の回転クォータニオンを行列化
		SimpleMath::Matrix rotateMatrix = SimpleMath::Matrix::CreateFromQuaternion(m_rotate);

		// 仮想的なX軸（カメラ空間での水平ベクトル）
		SimpleMath::Vector3 horizonal = SimpleMath::Vector3::UnitX;

		// 回転軸の定義
		SimpleMath::Vector3 axisY = -SimpleMath::Vector3::UnitY;
		SimpleMath::Vector3 axisX = SimpleMath::Vector3::TransformNormal(horizonal, rotateMatrix);
		axisX.y = 0.0f;

		SimpleMath::Quaternion q = SimpleMath::Quaternion::Identity;

		// クォータニオンによる回転の合成（Y軸→X軸の順）
		q *= SimpleMath::Quaternion::CreateFromAxisAngle(axisY, XMConvertToRadians(rotY));
		q *= SimpleMath::Quaternion::CreateFromAxisAngle(axisX, XMConvertToRadians(rotX));


		m_rotate *= q;


		// 現在のオイラー角の取得 
		SimpleMath::Vector3 euler = m_rotate.ToEuler();

		// X軸（ピッチ角）を範囲に直接クランプする
		euler.x = MyLib::Clamp(euler.x, MIN_PITCH_RADIAN, MAX_PITCH_RADIAN);

		// クランプしたオイラー角から新しいクォータニオンを生成し直す
		m_rotate = SimpleMath::Quaternion::CreateFromYawPitchRoll(euler.y, euler.x, 0);


	}
	// 現在のマウス位置を保存
	m_mousePrevX = x;
	m_mousePrevY = y;
}

