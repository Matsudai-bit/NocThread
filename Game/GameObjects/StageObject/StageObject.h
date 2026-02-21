/*****************************************************************//**
 * @file    StageObject.h
 * @brief   ステージオブジェクトに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/06/25
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// フレームワーク関連
#include "Game/Common/Framework/Event/WireSystemObserver/IWireEventObserver.h"
#include "Game/Common/Framework/StateMachine/StateMachine.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/GameObjects/Common/MovableObject/MovableObject.h"



// クラスの前方宣言 ===================================================
class CollisionManager;

// クラスの定義 ===============================================================
/**
 * @brief ステージオブジェクト
 */
class StageObject
	: public IWireEventObserver
	, public MovableObject
{
// クラス定数の宣言 -------------------------------------------------
public:

	// ステージオブジェクトモデルのファイル名
	static const constexpr char* STAGE_OBJECT_MODEL_FILE_NAME = "StageObject.sdkmesh";

	static constexpr float FRICTION = 0.95f;		// 摩擦係数


// データメンバの宣言 -----------------------------------------------
private:

	DirectX::Model m_model;					///< モデル

	std::unique_ptr<StateMachine<StageObject>> m_stateMachine; ///< ステートマシーン

	std::unique_ptr<Sphere> m_collider;		///< コライダ

	bool m_isActive; ///<　活動しているかどうか

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageObject();

	// デストラクタ
	~StageObject();


// 操作
public:
	// 初期化処理
	void Initialize(CommonResources* pCommonResources, CollisionManager* pCollisionManager);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(const Camera& camera);

	// 終了処理
	void Finalize();

	// 物理的な移動の適用
	void ApplyPhysic(const float& deltaTime);
	// 摩擦の適用
	void ApplyFriction();
	// 重力の適用
	void ApplyGravity(const float& deltaTime);
	// 移動の適用
	void Move(const float& deltaTime);


	//  ワイヤーに掴まれたときの挙動
	void OnWireGrab(const WireEventData& eventData) ;

	// ワイヤーから離れたとき
	void OnWireRelease(const WireEventData& eventData) ;

	// ワイヤーで引き寄せられるとき
	void OnWirePull(const WireEventData& eventData) ;

	// ワイヤーで投げられるとき
	void OnWireThrow(const WireEventData& eventData, const DirectX::SimpleMath::Vector3& throwImpulse) ;

	// ワイヤーに掴まれている時
	void OnWireHover(const WireEventData& eventData) ;


	// 衝突処理
	void OnCollision(const CollisionInfo& info) override;


	// 取得/設定
public:

	// 活動しているかどうか
	virtual bool IsActive() const { return m_isActive; };

	void SetActive(const bool& isActive) { m_isActive = isActive; }

	// ゲームオブジェクトタグの取得
	virtual GameObjectTag GetTag() const { return GameObjectTag::STAGE_OBJECT; };


// 内部実装
private:


};
