/*****************************************************************//**
 * @file    Building.h
 * @brief   建物に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/25
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Building.h"

// ライブラリ関連
#include "Library/DirectXFramework/DeviceResources.h"

// フレームワーク関連
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/Collision/CollisionManager/CollisionManager.h"

// グラフィック関連
#include "Game/Common/Graphics/Camera/Camera.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Building::Building()
{
	m_extraScale = SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

}

/**
 * @brief デストラクタ
 */
Building::~Building()
{

	

}


/**
 * @brief 初期化処理
 *
 * @param[in] pCommonResources
 * @param[in] pCollisionManager
 */
void Building::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager)
{
	// 1. 基本設定とモデル読み込み
	SetCommonResources(pCommonResources);

	auto* transform = GetTransform();
	transform->SetScale(0.2f);

	m_colliderExtendOffset = { 0.0f, -1.0f, 0.0f };
	m_model = GetCommonResources()->GetResourceManager()->CreateModel("building_01.sdkmesh");

	// スケール計算の共通化
	const float baseScale = transform->GetScale().x;
	const SimpleMath::Vector3 totalScale = m_extraScale * baseScale;

	// 2. メッシュからコライダを自動生成
	InitializeMeshColliders(totalScale);

	// 3. 手動設定の追加コライダ (アンテナや装飾部分など)
	AddManualColliders();

	// 4. カリング用球体の設定 (最初のコライダを基準にする)
	if (!m_collider.empty())
	{
		const auto& mainExt = m_collider.front()->GetExtend();
		float maxAxis = std::max({ mainExt.x, mainExt.y, mainExt.z });
		m_cullingSphere = std::make_unique<Sphere>(m_collider.front()->GetCenter(), maxAxis / 2.0f);
	}

	// 5. 衝突判定マネージャーへの登録
	RegisterToCollisionManager(pCollisionManager);

}

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void Building::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}

/**
 * @brief 描画処理
 *
 * @param[in] view
 * @param[in] proj
 */
void Building::Draw(const Camera& camera)
{
	using namespace SimpleMath;

	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetCommonResources()->GetCommonStates();

	Matrix world = Matrix::Identity;



	world *= Matrix::CreateScale(m_extraScale * GetTransform()->GetScale());
	world *= Matrix::CreateFromQuaternion(GetTransform()->GetRotation());
	world *= Matrix::CreateTranslation(GetTransform()->GetPosition());

	m_model.Draw(context, *states, world, camera.GetViewMatrix(), camera.GetProjectionMatrix());

	//for (auto& collider : m_collider)
	//{
	//	collider->Draw(context, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	//}
}





/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Building::Finalize()
{

}

/**
 * @brief 衝突管理
 * 
 * @param[in] info 衝突情報
 */
void Building::OnCollision(const CollisionInfo& info)
{
	UNREFERENCED_PARAMETER(info);

}

/**
 * @brief メッシュコライダの初期化
 *
 * @param[in] totalScale 総合スケール
 */
void Building::InitializeMeshColliders(const SimpleMath::Vector3& totalScale)
{
	for (const auto& mesh : m_model.meshes)
	{
		// スケールを考慮したサイズの計算
		SimpleMath::Vector3 extents = mesh->boundingBox.Extents * totalScale * 1.75f;
		extents.y = mesh->boundingBox.Extents.y * totalScale.y * 1.95f;

		if (m_extent.y < extents.y) m_extent = extents;

		// オフセット計算と生成
		SimpleMath::Vector3 centerOffset(0.0f, (extents.y * 0.5f) - 0.5f, 0.0f);
		auto collider = std::make_unique<AABB>(
			GetTransform()->GetPosition() + centerOffset,
			extents + m_colliderExtendOffset
		);

		m_collider.emplace_back(std::move(collider));
	}

	// 親コライダ（全体を包む判定）の設定
	if (!m_collider.empty())
	{
		auto parentExtents = m_collider[0]->GetExtend() * SimpleMath::Vector3(1.0f, 1.5f, 1.0f);
		auto parent = std::make_unique<AABB>(m_collider[0]->GetCenter(), parentExtents);
		m_parentColliderObject.SetCollider(std::move(parent));
	}
}

/**
 * @brief 手動コライダの追加
 */
void Building::AddManualColliders()
{
	struct ManualColliderDef { SimpleMath::Vector3 offset; SimpleMath::Vector3 size; };
	const ManualColliderDef extraParts[] = {
		{ {-1.225f, 20.0f, -0.7f}, {1.5f, 2.0f, 1.75f} }, // パーツA
		{ { 4.300f, 19.75f, -0.6f}, {2.15f, 1.4f, 1.6f} } // パーツB
	};

	for (const auto& part : extraParts)
	{
		auto collider = std::make_unique<AABB>(
			GetTransform()->GetPosition() + (part.offset * m_extraScale),
			part.size * m_extraScale
		);
		m_collider.emplace_back(std::move(collider));
	}
}

/**
 * @brief 衝突管理へ登録
 *
 * @param[in] pCollisionManager 衝突管理
 */
void Building::RegisterToCollisionManager(CollisionManager* pCollisionManager)
{
	if (!pCollisionManager || !m_parentColliderObject.GetCollider()) return;

	// 親を登録
	pCollisionManager->AddCollisionData(CollisionData(&m_parentColliderObject, m_parentColliderObject.GetCollider(), true));

	// 子（各部位）を親に紐付けて登録
	for (auto& collider : m_collider)
	{
		pCollisionManager->AddCollisionData(CollisionData(this, collider.get(), true), m_parentColliderObject.GetCollider());
	}
}



