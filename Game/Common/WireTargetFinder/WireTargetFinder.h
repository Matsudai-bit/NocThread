/*****************************************************************//**
 * @file    WireTargetFinder.h
 * @brief   ワイヤーの照準を合わせるクラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/05
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/Common/Collision/Collision.h"
#include "Library/MyLib/MathUtils/MathUtils.h"
#include "Library/MyLib/Ray/Ray.h"



// クラスの前方宣言 ===================================================
class CollisionManager; // 衝突管理
class CommonResources;	// 共通リソース
class Camera;			// カメラ
class Player;			// プレイヤー

// クラスの定義 ===============================================================
/**
 * @brief ワイヤーの照準を合わせるクラス
 */
class WireTargetFinder :
	public GameObject
{
// 構造体の宣言 -------------------------------------------------
private:

	/**
	 * @brief 照準対象データ
	 */
	struct GrappleTargetData
	{
		DirectX::SimpleMath::Vector3	targetPosition;		// 照準座標
		const GameObject*				pTargetGameObject;	// 照準があるゲームオブジェクト

		GrappleTargetData(const DirectX::SimpleMath::Vector3& targetPosition, const GameObject* pTargetGameObject)
			: targetPosition	{targetPosition}
			, pTargetGameObject { pTargetGameObject }{}
	};
	 
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::vector<GrappleTargetData> m_grappleTargetData; ///< 照準対象データ群

	std::unique_ptr<Sphere> m_sphereCollider;	///< 球コライダー

	std::unique_ptr<Capsule> m_capsuleCollider;///< 筒コライダー

	float m_detectionRadius;	///< 検出半径

	const Camera* m_pCamera; ///< カメラ

	const Player* m_pPlayer; ///< プレイヤー

	// デバック用
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	WireTargetFinder();

	// デストラクタ
	~WireTargetFinder();


// 操作
public:
	// 初期化処理
	void Initialize(
		const CommonResources* pCommonResources, 
		CollisionManager* pCollisionManager, 
		const float& detectionRadius,
		const Player* pPlayer);

	// 更新処理
	void Update(float elapsedTime, const Camera* pCamera, MyLib::Ray wireShootingRay, const float& length, const float& radius);

	// 描画処理
	void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);

	// 終了処理
	void Finalize();


	void PreCollision() override;
	// 衝突処理
	void OnCollision(GameObject* pHitObject, ICollider* pHitCollider) override ;



	// 取得/設定
public:

	// 活動しているかどうか
	bool IsActive() const override { return true; }

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::WIRE_GRAPPING_AREA; }


	DirectX::SimpleMath::Vector3 GetTargetPosition() ;

	// 照準を見つけたかどうか
	bool IsFindTarget() const;


// 内部実装
private:


};
