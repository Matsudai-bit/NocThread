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

#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

#include "Library/DirectXFramework/DeviceResources.h"
#include "Game/Common/Camera/Camera.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Building::Building()
{
	m_extends = SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

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
	// 共通リソースの設定
	SetCommonResources(pCommonResources);

	GetTransform()->SetScale(0.2f);
	
	m_colliderExtendOffset.y = -1.0f;
	m_model = GetCommonResources()->GetResourceManager()->CreateModel("building_01.sdkmesh");

	for (auto& mesh : m_model.meshes)
	{
		auto boundingBox = mesh->boundingBox;



		SimpleMath::Vector3 extents = boundingBox.Extents;
		extents *= m_extends  *  /** 2.0f*/  GetTransform()->GetScale()  * 2.0f;

		extents.y = boundingBox.Extents.y * m_extends.y * GetTransform()->GetScale().x * 1.95f;

		

		//m_positionOffset.y = extents.y / 2.0f;
		
		std::unique_ptr<AABB> collider = std::make_unique<AABB>(GetTransform()->GetPosition() + SimpleMath::Vector3(0.0f, (extents.y / 2.0f) - 0.5f, 0.0f), extents + m_colliderExtendOffset);

		// 衝突管理に登録
		pCollisionManager->AddCollisionObjectData(this, collider.get());

		// 登録
		m_collider.emplace_back(std::move(collider));


	}

	

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

	
	
	world *= Matrix::CreateScale(m_extends * GetTransform()->GetScale());
	world *= Matrix::CreateFromQuaternion(GetTransform()->GetRotation());
	world *= Matrix::CreateTranslation(GetTransform()->GetPosition());

	m_model.Draw(context, *states, world, camera.GetViewMatrix(), camera.GetProjectionMatrix());

	//for (auto& collider : m_collider)
	//{
	//	collider->Draw(context, view, proj);
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
 * @param[in] pHitObject
 * @param[in] pHitCollider
 */
void Building::OnCollision(GameObject* pHitObject, ICollider* pHitCollider)
{
	UNREFERENCED_PARAMETER(pHitObject);
	UNREFERENCED_PARAMETER(pHitCollider);
}


