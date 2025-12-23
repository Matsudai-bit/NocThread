/*****************************************************************//**
 * @file    Checkpoint.h
 * @brief   ゲームのチェックポイントに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/12/17
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <unordered_map>
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/Common/Collision/Collision.h"
#include "Library/MyLib/DirectXMyToolKit/ModelPart/ModelPart.h"
// クラスの前方宣言 ===================================================
class CommonResources; // 共通リソース
class CollisionManager;// 衝突管理
class CheckpointObjectController; // チェックポイントオブジェクトの操作クラス


// クラスの定義 ===============================================================
/**
 * @brief チェックポイント
 */
class Checkpoint
	: public GameObject	// ゲームオブジェクトの基底クラス
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	bool m_isEnabled; ///< 有効化されているかどうか
	std::unique_ptr<AABB> m_collider;

	std::unique_ptr< CheckpointObjectController> m_checkpointObject;	///< チェックポイントオブジェクト

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Checkpoint();

	// デストラクタ
	~Checkpoint();


// 操作
public:
	// 初期化処理
	void Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(const Camera& camera);

	// 終了処理
	void Finalize();

	// 衝突処理
	void OnCollision(const CollisionInfo& info) override;


	// 取得/設定
public:


	// 活動しているかどうか
	bool IsActive() const override { return m_isEnabled; };

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::CHECKPOINT; };


// 内部実装
private:


};
