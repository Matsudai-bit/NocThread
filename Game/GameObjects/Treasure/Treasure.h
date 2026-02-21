/*****************************************************************//**
 * @file    Treasure.h
 * @brief   お宝クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>

#include "Game/GameObjects/Common/GameObject.h"
#include "Game/Common/Utillities/Collision/Collision.h"

// クラスの前方宣言 ===================================================
class CollisionManager; // 衝突管理
class CollisionInfo;	// 衝突情報

// クラスの定義 ===============================================================
/**
 * @brief お宝
 */
class Treasure 
	: public GameObject
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:


	std::unique_ptr<Sphere> m_collider; ///< 球コライダー

	DirectX::Model m_model; // モデル

	bool m_isActive; ///< 活動しているかどうか

	int m_effectId;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Treasure();

	// デストラクタ
	~Treasure();


// 操作
public:
	// 初期化処理
	void Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager);

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 描画処理
	void DrawTask(const Camera& camera) override;

	// 終了処理
	void Finalize();

// イベント関連
public:

	// 衝突処理
	void OnCollision(const CollisionInfo& info) override ;


// 取得/設定
public:
	// 活動しているかどうか
	bool IsActive() const override { return m_isActive; };

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::TREASURE; };

// 内部実装
private:


};
