/*****************************************************************//**
 * @file    EscapeHelicopter.h
 * @brief   脱出用ヘリコプターに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/03
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
// 
// フレームワーク関連
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/IGameFlowObserver.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/GameObjects/Common/MovableObject/MovableObject.h"



// クラスの前方宣言 ===================================================
class CommonResources;	// 共通リソース
class CollisionManager;	// 衝突管理

// クラスの定義 ===============================================================
/**
 * @brief 脱出用ヘリコプター
 */
class EscapeHelicopter
	: public MovableObject
	, public IGameFlowObserver
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	DirectX::Model m_helicopterModel;	///< ヘリコプターのモデル

	std::unique_ptr<AABB> m_collider; ///< AABBのコライダ

	bool m_isGameEnd;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EscapeHelicopter();

	// デストラクタ
	~EscapeHelicopter();


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


// 取得/設定
public:
	// 活動しているかどうか
	bool IsActive() const override { return true; }


	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::ESCAPE_HELICOPTER; }

	// イベント関連
public:

	// 衝突時に呼ばれる
	void OnCollision(const CollisionInfo& info ) override;

	// イベントメッセージを受け取る
	void OnGameFlowEvent(GameFlowEventID eventID) override;

// 内部実装
private:


};
