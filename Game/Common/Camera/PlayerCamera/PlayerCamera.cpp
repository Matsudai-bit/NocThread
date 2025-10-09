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

#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/ImaseLib/DebugFont.h"

using namespace DirectX;

/// @brief デフォルトのカメラ距離（未使用、将来的な拡張用）
const float PlayerCamera::DEFAULT_CAMERA_DISTANCE = 5.0f;

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
	, m_isStartUpdatig{ true }
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
void PlayerCamera::Initialize(CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	SetCommonResources(pCommonResources);

	// 球コライダーの作成
	m_sphereCollider = std::make_unique<Sphere>();
	m_sphereCollider->SetRadius(1.0f);

	// 衝突管理への登録
	pCollisionManager->AddCollisionObjectData(this, m_sphereCollider.get());

	m_eyeTargetPosition = SimpleMath::Vector3::Zero;
}

/**
 * @brief カメラの更新処理
 *
 * 毎フレーム呼び出され、マウスの現在位置に応じてカメラの回転を更新し、
 * プレイヤーの位置と方向からビュー行列用の視点と注視点を設定します。
 */
void PlayerCamera::Update(float elapsedTime)
{

	// マウス状態を取得
	auto state = m_pMouseTracker->GetLastState();

	// 相対モードでばない（カメラFPS視点など）場合は処理をスキップ
	if (state.positionMode != Mouse::MODE_RELATIVE) return;

	int mouseX = state.x;
	int mouseY = state.y;


	// クォータニオン回転を行列に変換
	if (m_isStartUpdatig)
	{
		m_mousePrevX = mouseX;
		m_mousePrevY = mouseY;
		m_isStartUpdatig = false;
	}



	// 現在のマウス位置に基づいて回転を更新
	Motion(mouseX, mouseY);

	// プレイヤーの前方方向を取得
	SimpleMath::Vector3 localForward = SimpleMath::Vector3(0.0f, 0.0f, -0.1f);
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateFromQuaternion(m_rotate);

	SimpleMath::Vector3 forward = SimpleMath::Vector3::TransformNormal(localForward, rot);;

	auto position = m_pPlayer->GetPosition();
	//position += SimpleMath::Vector3(4.0f, 0.0f, 0);
	// 注視点（プレイヤーの少し前方）
	SimpleMath::Vector3 target = position + forward * 2.0f;

	SimpleMath::Vector3 eye = GetEye();


	SimpleMath::Vector3 velocity = (m_eyeTargetPosition - eye) * 20.0f * elapsedTime;
	if (SimpleMath::Vector3::Distance(position, eye) > 2.0f)
	{
		eye += velocity;
	}


	// 視点（プレイヤーの後方、やや引いた位置）
	SimpleMath::Vector3 eyeTargetPositionTmp = position + -forward * 100.0f + SimpleMath::Vector3(0.0f, 2.5f, 0.0f);

	m_eyeTargetPosition = (SimpleMath::Vector3::Distance(eyeTargetPositionTmp, m_eyeTargetPosition) >= 0.01f  ) ? eyeTargetPositionTmp : m_eyeTargetPosition ;

	// ワールド上方向（Y軸）をカメラ空間で再計算
	SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);
	
	//auto rt = SimpleMath::Matrix::CreateFromQuaternion( m_rotate);

	//// 視線方向の算出
	//auto nor = target - eye;
	//nor.Normalize();

	//// カメラのX方向を算出
	//auto uniX = SimpleMath::Vector3::UnitX;
	//uniX = SimpleMath::Vector3::TransformNormal(uniX, rt);

	//up = uniX.Cross(nor);

	//up = SimpleMath::Vector3::TransformNormal(up, rt);



	// カメラの視点と注視点を設定（基底クラス Camera の機能）
	SetTarget(target);

	SetEye(eye);
	SetUp(up);

	// ゲームオブジェクト側の座標の設定
	SetPosition(GetEye());
	m_sphereCollider->Transform(m_eyeTargetPosition);

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
	float dx = static_cast<float>(x ) * m_sx;
	float dy = static_cast<float>(y ) * m_sy;

	// 変化があった場合のみ回転を更新
	if ((std::abs(dx) + std::abs(dy)) > 0.001f)
	{

		// ラジアン変換（1画面分をπラジアンに相当）
		float rotY = dx */* XM_PI * */ 200.0f;
		float rotX = -dy */* XM_PI **/ 200.0f;

		// 現在の回転クォータニオンを行列化
		SimpleMath::Matrix rotateMatrix = SimpleMath::Matrix::CreateFromQuaternion(m_rotate);

		// 仮想的なX軸（カメラ空間での水平ベクトル）
		SimpleMath::Vector3 horizonal = SimpleMath::Vector3::UnitX;

		// 回転軸の定義
		SimpleMath::Vector3 axisY = -SimpleMath::Vector3::UnitY;
		SimpleMath::Vector3 axisX =  SimpleMath::Vector3::TransformNormal(horizonal, rotateMatrix);
		axisX.y = 0.0f;

		SimpleMath::Quaternion q = SimpleMath::Quaternion::Identity;

		// クォータニオンによる回転の合成（Y軸→X軸の順）
		q *= SimpleMath::Quaternion::CreateFromAxisAngle(axisY, XMConvertToRadians(rotY));
		q *= SimpleMath::Quaternion::CreateFromAxisAngle(axisX, XMConvertToRadians(rotX));

		m_rotate *= q;


	}
	// 現在のマウス位置を保存
	m_mousePrevX = x;
	m_mousePrevY = y;
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
void PlayerCamera::SetPlayer(Player* pPlayer)
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

	if (pHitObject->GetTag() == GameObjectTag::FLOOR)
	{
		//OnCollisionWithFloor(pHitObject, pHitCollider);

	}

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
	if (std::abs(m_overlapTotal.LengthSquared()) > 0.0f)
	{

		SimpleMath::Vector3 overlap = m_overlapTotal;

		// **** 押し出し ****
		m_eyeTargetPosition = PhysicsHelper::PushOut(overlap  , m_eyeTargetPosition);



		m_overlapTotal = SimpleMath::Vector3::Zero;
	}
}

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

	if (combinedDirection.LengthSquared() < 1e-6f)
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

void PlayerCamera::OnCollisionWithFloor(GameObject* pHitObject, ICollider* pHitCollider)
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

	if (combinedDirection.LengthSquared() < 1e-6f)
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
