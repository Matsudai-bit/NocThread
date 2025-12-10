/*****************************************************************//**
 * @file    WireTargetFinder.cpp
 * @brief   ワイヤーの照準を合わせるクラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/05
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "WireTargetFinder.h"

#include <algorithm>

// リソース関連
#include "Game/Common/CommonResources/CommonResources.h"

// ゲームシステム関連
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

// ゲームオブジェクト関連
#include "Game/Common/Camera/Camera.h"
#include "Game/GameObjects/Player/Player.h"

// 外部ライブラリ・ツール
#include "Library/ImaseLib/DebugFont.h"
#include "Library/ImaseLib/DebugDraw.h"


using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
WireTargetFinder::WireTargetFinder()
	: m_detectionRadius{ 0.0f }
	, m_pCamera{ nullptr }
	, m_pPlayer{ nullptr }
	, m_pCollisionManager{ nullptr }
	, m_wireLength{}
	, m_wireRadius{}
	
{

}



/**
 * @brief デストラクタ
 */
WireTargetFinder::~WireTargetFinder()
{

}



/**
 * @brief 初期化処理
 * 
 * @param[in] pCommonResources		共通リソース
 * @param[in] pCollisionManager		衝突管理
 * @param[in] pPlayer				プレイヤーのポインタ
 */
void WireTargetFinder::Initialize(const CommonResources* pCommonResources,
	CollisionManager* pCollisionManager,
	const Player* pPlayer,
	const Camera* pCamera)
{
	m_pCamera = pCamera;

	// 共通リソースへ登録
	SetCommonResources(pCommonResources);

	m_grappleTargetPositionCache.clear();

	m_pPlayer = pPlayer;

	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext());

	m_pCollisionManager = pCollisionManager;
}

/**
 * @brief 更新処理
 */
void WireTargetFinder::Update()
{
	// 検索要求
	RequestSearchingTargetPosition();
}

/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void WireTargetFinder::Draw(const Camera& camera)
{
	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();


	std::unique_ptr<GeometricPrimitive>  a = GeometricPrimitive::CreateSphere(context, 0.5f);

	// デバック表示
	//m_capsuleCollider->Draw(context, view, projection);

	//DX::DrawRay(m_primitiveBatch.get(), m_capsuleCollider->GetPosition() + (-m_capsuleCollider->GetAxis() * (m_capsuleCollider->GetLength() / 2.0f)), m_capsuleCollider->GetAxis() * m_capsuleCollider->GetLength(), false, Colors::Red);

	m_primitiveBatch->Begin();
	for (auto direction : GetSearchDirections())
	{
		DX::DrawRay(m_primitiveBatch.get(), m_pPlayer->GetTransform()->GetPosition(), direction * 30.0f, false, Colors::Yellow);
	}
	
	m_primitiveBatch->End();

	for (auto& data : m_grappleTargetPositionCache)
	{

		a->Draw(SimpleMath::Matrix::CreateTranslation(data), camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::Blue, nullptr, true);
	}

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void WireTargetFinder::Finalize()
{

}

void WireTargetFinder::PreCollision()
{
	m_grappleTargetPositionCache.clear();
}

/**
 * @brief 衝突処理
 *
 * @param[in] info 衝突情報
 */
void WireTargetFinder::OnCollision(const CollisionInfo& info)
{


	SimpleMath::Vector3 targetPosition;

	auto capsule = dynamic_cast<const Capsule*>(info.pMyCollider);

	// 目標座標の算出
	if (CalcWireTargetPosition(&targetPosition, *capsule, info.pOtherObject, info.pOtherCollider))
	{
		m_grappleTargetPositionCache.emplace_back(targetPosition);
	}
}

/**
 * @brief 検索するパラメータの設定
 * 
 * @param[in] wireShootingRay
 * @param[in] length
 * @param[in] radius
 */
void WireTargetFinder::SetSearchParameters(DirectX::SimpleMath::Vector3 wireShootingDirection, const float& length, const float& radius)
{
	m_shootingRay.direction = wireShootingDirection;
	m_wireLength = length;
	m_wireRadius = radius;
}

DirectX::SimpleMath::Vector3 WireTargetFinder::GetTargetPosition()
{

	return 	GetFarTargetPosition(m_grappleTargetPositionCache);
}

/**
 * @brief 照準を見つけたかどうか
 *
 * @return 照準を見つけたかどうか
 */
bool WireTargetFinder::IsFindTarget() const
{
	

	return m_grappleTargetPositionCache.size() > 0;

}


/**
 * @brief 目標座標を探す
 * 
 */
void WireTargetFinder::RequestSearchingTargetPosition()
{
	//if (m_isCheckedTargetPosition) return;

	//m_isCheckedTargetPosition = true;

	SearchTargetPositionForSeveralPosition();
	//SearchTargetPositionForShootingRay(m_shootingRay);

}

/**
 * @brief 飛ばすレイを元に目標座標を取得する(旧バージョン)
 *
 * @param[in] shootingRay 飛ばすレイ
 *
 * @return 目標座標
 */
void WireTargetFinder::SearchTargetPositionForShootingRay(MyLib::Ray shootingRay)
{
	// 目標座標候補の取得
	auto targetPositionCandidates = GetTargetPositionCandidatesForDirection(std::vector<DirectX::SimpleMath::Vector3>{shootingRay.direction});

	// キャッシュの登録
	m_grappleTargetPositionCache = targetPositionCandidates;
}

/**
 * @brief いくつかの点から目標座標を取得する
 *
 * @return 目標座標
 */
void WireTargetFinder::SearchTargetPositionForSeveralPosition()
{
	using namespace SimpleMath;

	auto searchDirections = GetSearchDirections();

	// 方向を元に衝突判定を取り、ワイヤーの接地座標を算出する
	std::vector<Vector3> targetPositions;

	// 目標座標候補の取得
	auto targetPositionCandidates = GetTargetPositionCandidatesForDirection(searchDirections);

	// キャッシュの登録
	m_grappleTargetPositionCache = targetPositionCandidates;

}

/**
 * @brief 調べる方向を取得する
 * 
 * @return 調べる方向
 */
std::vector<DirectX::SimpleMath::Vector3> WireTargetFinder::GetSearchDirections() const
{
	using namespace SimpleMath;
	// 結果を格納する
	std::vector<DirectX::SimpleMath::Vector3> result;

	if (m_pCamera == nullptr) { return result; }
	Vector3 cameraDirection = m_pCamera->GetTarget() - m_pCamera->GetEye();
	cameraDirection.Normalize();
	// 検索の中心線
	Vector3 centerDirection = cameraDirection;
	centerDirection.y = 0.0f;

	// XとYのオフセットを計算する行列 (回転はcenterDirection基準)
	Matrix baseRotation = Matrix::CreateFromQuaternion(Quaternion::FromToRotation(Vector3::Forward, centerDirection));

	for (int y = 0; y <= SEARCH_DIRECTION_NUM; ++y)
	{
		for (int x = -SEARCH_DIRECTION_NUM; x <= SEARCH_DIRECTION_NUM; ++x)
		{
			

			// 視線方向からのオフセット角度
			float offset = SEARCH_ANGLE_OFFSET_YAW;
			offset *= (x > 0.0f) ? -1.0f : 1.0f;

			float yaw = SEARCH_ANGLE_STEP_YAW * x +offset;
			float pitch = SEARCH_ANGLE_START_PITCH + std::abs(SEARCH_ANGLE_STEP_PITCH * y);

			// 視線方向を基準に回転を適用
			Vector3 offsetDirection = Vector3::TransformNormal(Vector3::Forward,
				Matrix::CreateRotationY(XMConvertToRadians(yaw)) * Matrix::CreateRotationX(XMConvertToRadians(pitch)));

			// 視線方向の回転を適用
			Vector3 finalDirection = Vector3::TransformNormal(offsetDirection, baseRotation);

			result.emplace_back(finalDirection);
		}
	}
	return result;
}

/**
 * @brief カプセルコライダーの作成
 * 
 * @param[in] origin	原点
 * @param[in] direction 向き
 * @param[in] length	長さ
 * @param[in] radius	半径
 * 
 * @return 作成したカプセルコライダー
 */
Capsule WireTargetFinder::CreateCapsuleCollider(
	const DirectX::SimpleMath::Vector3& origin, 
	const DirectX::SimpleMath::Vector3& direction,
	const float& length,
	const float& radius)
{
	Capsule capsule ;

	SimpleMath::Vector3 center = origin + (direction * (length / static_cast<float>(2.0)));

	// 座標の設定
	capsule.SetPosition(center);

	// 半径
	capsule.SetRadius(radius);

	// 長さ
	// カプセルのため理想の長さにするため半径を削る
	capsule.SetLength(length - radius * 2.0f);

	capsule.SetAxis(direction);

	return capsule;
}


/**
 * @brief 衝突情報を元に接地座標を算出する
 * 
 * @param[out]pTargetPosition	 目標座標
 * @param[in] hitCapsuleCollider 衝突したカプセルコライダー
 * @param[in] pHitObject		衝突したオブジェクト
 * @param[in] pHitCollider		衝突したコライダー
 * 
 * @returns true 算出できた
 * @returns false 算出できなかった
 */
bool WireTargetFinder::CalcWireTargetPosition(
	DirectX::SimpleMath::Vector3* pTargetPosition,
	const Capsule& hitCapsuleCollider,
	const GameObject* pHitObject,
	const ICollider* pHitCollider)
{
	const float MIN_GRAPPLE_DISTANCE_THRESHOLD = 1.1f;
	
	if (pHitObject->GetTag() == GameObjectTag::BUILDING || pHitObject->GetTag() == GameObjectTag::ESCAPE_HELICOPTER)
	{
		// AABBにキャスト
		if (const auto aabb = dynamic_cast<const AABB*>(pHitCollider))
		{
			SimpleMath::Vector3 targetPosition;
			if (GetIntersectionPointOnSurface(*aabb, hitCapsuleCollider, &targetPosition))
			{
				// 距離が MIN_GRAPPLE_DISTANCE_THRESHOLD 以下の場合除外
				if (MIN_GRAPPLE_DISTANCE_THRESHOLD * MIN_GRAPPLE_DISTANCE_THRESHOLD < SimpleMath::Vector3::DistanceSquared(GetTransform()->GetPosition(), targetPosition))
				{
					*pTargetPosition = targetPosition;
					return true;
				}
			}

		}

	}

	return false;
}

/**
 * @brief 最も遠い目標座標を取得する
 * 
 * @param[in] targetPositions 目標座標の候補
 * 
 * @return 最も遠い目標座標 
 */
DirectX::SimpleMath::Vector3 WireTargetFinder::GetFarTargetPosition(std::vector<DirectX::SimpleMath::Vector3> targetPositions)
{
	using namespace SimpleMath;

	// 近い順にソートする
	std::sort(targetPositions.begin(), targetPositions.end(), [&](const Vector3& lhs, const Vector3& rhs)
		{
			return Vector3::DistanceSquared(lhs, m_pPlayer->GetTransform()->GetPosition()) < Vector3::DistanceSquared(rhs, m_pPlayer->GetTransform()->GetPosition());
		});


	return (targetPositions.size() != 0) ? targetPositions.back() : SimpleMath::Vector3::Zero;
}

/**
 * @brief 方向からカプセルコライダーを算出し目標座標の候補を取得する
 * 
 * @param[in] directions 方向
 * 
 * @return 最も遠い目標座標
 */
std::vector<DirectX::SimpleMath::Vector3> WireTargetFinder::GetTargetPositionCandidatesForDirection(std::vector<DirectX::SimpleMath::Vector3> searchDirections)
{
	using namespace SimpleMath;
	// 目標座標の候補
	std::vector<Vector3> targetPositions;

	bool registry = false;
	// サイズが違う場合コライダを登録し直す
	if (m_capsules.size() != searchDirections.size())
	{
		registry = true;

		for (auto& capsule : m_capsules)
		{
			m_pCollisionManager->RemoveCollisionObjectData(this, &capsule);
		}
	}
	
	// サイズを設定
	m_capsules.resize(searchDirections.size());




	for (size_t i = 0; i < searchDirections.size(); i++)
	{
		Capsule capsule = CreateCapsuleCollider(m_pPlayer->GetTransform()->GetPosition(), searchDirections[i], m_wireLength * std::abs(searchDirections[i].y * 1.3f), m_wireRadius);

		(m_capsules[i]) = capsule;
		
		

		//// 衝突情報
		//std::vector<const GameObject*> hitGameObjects{};
		//std::vector<const ICollider*> hitColliders{};

		//// 衝突判定
		//if (m_pCollisionManager->RetrieveCollisionData(&capsule, &hitGameObjects, &hitColliders))
		//{

		//	for (size_t i = 0; i < hitGameObjects.size(); i++)
		//	{
		//		// 処理用変数
		//		auto pHitObject		= hitGameObjects[i];
		//		auto pHitCollider	= hitColliders[i];

		//		SimpleMath::Vector3 targetPosition;

		//		// 目標座標の算出
		//		if (CalcWireTargetPosition(&targetPosition, capsule, pHitObject, pHitCollider))
		//		{
		//			targetPositions.emplace_back(targetPosition);
		//		}
		//	}
		//}

	}
	if (registry)
	{
		for (auto& capsule : m_capsules)
		{
			m_pCollisionManager->AddCollisionObjectData(this, &capsule);
		}
	}
	return targetPositions;
}
