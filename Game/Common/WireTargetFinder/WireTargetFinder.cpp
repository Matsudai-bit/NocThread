/*****************************************************************//**
 * @file    WireTargetFinder.h
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
 * @param[in] detectionRadius		検出半径
 * @param[in] pPlayer				プレイヤーのポインタ
 */
void WireTargetFinder::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager, const float& detectionRadius, const Player* pPlayer)
{
	// 共通リソースへ登録
	SetCommonResources(pCommonResources);

	// コライダーの作成
	m_sphereCollider = std::make_unique<Sphere>(GetPosition(), detectionRadius);
	m_capsuleCollider = std::make_unique<Capsule>();

	// コライダーの登録
	pCollisionManager->AddCollisionObjectData(this, m_capsuleCollider.get());

	m_grappleTargetData.clear();

	m_pPlayer = pPlayer;

	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext());
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void WireTargetFinder::Update(float elapsedTime, const Camera* pCamera, MyLib::Ray wireShootingRay, const float& length, const float& radius)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 座標更新
	SetPosition(m_pPlayer->GetPosition());

	// コライダーの更新処理
	m_sphereCollider->Transform(GetPosition());

	m_pCamera = pCamera;

	SimpleMath::Vector3 center = wireShootingRay.origin + (wireShootingRay.direction * (length / static_cast<float>(2.0))) ;

	// 座標の設定
	m_capsuleCollider->SetPosition(center);

	// 半径
	m_capsuleCollider->SetRadius(radius);

	// 長さ
	// カプセルのため理想の長さにするため半径を削る
	m_capsuleCollider->SetLength(length - radius * 2.0f);

	m_capsuleCollider->SetAxis(wireShootingRay.direction);




}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void WireTargetFinder::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	
	// デバック表示
	//m_capsuleCollider->Draw(context, view, projection);

	//m_primitiveBatch->Begin();
	//DX::DrawRay(m_primitiveBatch.get(), m_capsuleCollider->GetPosition() + (-m_capsuleCollider->GetAxis() * (m_capsuleCollider->GetLength() / 2.0f)), m_capsuleCollider->GetAxis() * m_capsuleCollider->GetLength(), false, Colors::Red);
	//m_primitiveBatch->End();

	std::unique_ptr<GeometricPrimitive>  a = GeometricPrimitive::CreateSphere(context, 0.5f);

	for (auto& data : m_grappleTargetData)
	{

		a->Draw(SimpleMath::Matrix::CreateTranslation(data.targetPosition), view, projection, Colors::Blue, nullptr, true);
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
	m_grappleTargetData.clear();
}

/**
 * @brief 衝突処理
 * 
 * @param[in] pHitObject	衝突したオブジェクト
 * @param[in] pHitCollider	衝突したコライダー
 */
void WireTargetFinder::OnCollision(GameObject* pHitObject, ICollider* pHitCollider)
{
	if (pHitObject->GetTag() == GameObjectTag::ENEMY)
	{
		if (const auto sphere = dynamic_cast<const Sphere*>(pHitCollider))
		{
			SimpleMath::Vector3 targetPosition = sphere->GetPosition();
			
			// 距離が 1.1f 以下の場合除外
			if (1.1f * 1.1f >= SimpleMath::Vector3::DistanceSquared(GetPosition(), targetPosition))
			{
				return;
			}
			m_grappleTargetData.emplace_back(targetPosition, pHitObject);


			//GetCommonResources()->GetDebugFont()->AddString(100, 70, Colors::White, L"degree : %f", XMConvertToDegrees(radian));
			//GetCommonResources()->GetDebugFont()->AddString(100, 90, Colors::White, L"ratio : %f", XMConvertToDegrees(ratio));
			//GetCommonResources()->GetDebugFont()->AddString(100, 110, Colors::White, L"position : %f, %f, %f ", targetPosition.x, targetPosition.y, targetPosition.z);
		}
	}
	if (pHitObject->GetTag() == GameObjectTag::BUILDING || pHitObject->GetTag() == GameObjectTag::ESCAPE_HELICOPTER)
	{
		if (const auto aabb = dynamic_cast<const AABB*>(pHitCollider))
		{

			SimpleMath::Vector3 targetPosition;
			if (GetIntersectionPointOnSurface( *aabb, *m_capsuleCollider, &targetPosition))
			{
				// 距離が 1.1f 以下の場合除外
				if (1.1f *1.1f >= SimpleMath::Vector3::DistanceSquared(GetPosition(), targetPosition))
				{
					return;
				}
				m_grappleTargetData.emplace_back(targetPosition, pHitObject);
			}

			//GetCommonResources()->GetDebugFont()->AddString(100, 70, Colors::White, L"degree : %f", XMConvertToDegrees(radian));
			//GetCommonResources()->GetDebugFont()->AddString(100, 90, Colors::White, L"ratio : %f", XMConvertToDegrees(ratio));
			//GetCommonResources()->GetDebugFont()->AddString(100, 110, Colors::White, L"position : %f, %f, %f ", targetPosition.x, targetPosition.y, targetPosition.z);
		}
		
	}

}

DirectX::SimpleMath::Vector3 WireTargetFinder::GetTargetPosition()
{
	if (m_grappleTargetData.size() <= 0)
	{
		return SimpleMath::Vector3::Forward;
	}


	// ソート前にプレイヤーの位置を一度だけ取得
	const SimpleMath::Vector3 playerPosition = m_pPlayer->GetPosition();

	// m_grappleTargetData をプレイヤーからの距離が近い順にソート
	// [this] をキャプチャすることで、ラムダ内でクラスのメンバー変数にアクセス可能にする
	std::sort(m_grappleTargetData.begin(), m_grappleTargetData.end(), [&playerPosition](const GrappleTargetData& lhs, const GrappleTargetData& rhs)
		{
			// lhsとrhsの、プレイヤーからの距離の2乗を比較
			// sqrt()を呼び出す必要がないため、計算が速い
			return (SimpleMath::Vector3::DistanceSquared(playerPosition, lhs.targetPosition) < SimpleMath::Vector3::DistanceSquared(playerPosition, rhs.targetPosition));
		});

	return m_grappleTargetData.front().targetPosition;

}

/**
 * @brief 照準を見つけたかどうか
 * 
 * @return 照準を見つけたかどうか
 */
bool WireTargetFinder::IsFindTarget() const
{
	return m_grappleTargetData.size() > 0;
}
