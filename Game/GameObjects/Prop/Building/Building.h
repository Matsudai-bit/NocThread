/*****************************************************************//**
 * @file    Building.h
 * @brief   建物に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/25
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/GameObjects/Prop/Base/Prop.h"
#include "Game/Common/Collision/Collision.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 建物
 */
class Building
	: public Prop
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	DirectX::Model m_model;

	std::vector<std::unique_ptr<AABB>> m_collider;

	DirectX::SimpleMath::Vector3 m_positionOffset; // 底面がゲームオブジェクト座標になるように調整する

	DirectX::SimpleMath::Vector3 m_colliderExtendOffset; // 底面がゲームオブジェクト座標になるように調整する

	DirectX::SimpleMath::Vector3 m_extends;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Building();

	// デストラクタ
	~Building();

	// 操作
public:
	// 初期化処理
	void Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager) override ;

	// 更新処理
	void Update(float deltaTime) override ;

	// 描画処理
	void Draw(const Camera& camera) override ;
	  
	// 終了処理
	void Finalize()override ;


	// 衝突処理
	void OnCollision(GameObject* pHitObject, ICollider* pHitCollider) override ;



	// 取得/設定
public:

	// 活動しているかどうか
	bool IsActive() const override { return true; };

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::BUILDING; }
	DirectX::SimpleMath::Vector3 GetExtends() const { return m_extends; }

	void SetExtends(const DirectX::SimpleMath::Vector3& extends) 
	{
		m_extends = extends;
	}

// 内部実装
private:


};
