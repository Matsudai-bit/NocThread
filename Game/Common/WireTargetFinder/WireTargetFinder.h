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

	// 上下左右に広がるグリッドを想定したサンプリング
	static constexpr int	SEARCH_DIRECTION_NUM = 3; 
	static constexpr float	SEARCH_ANGLE_STEP_YAW = 8.0f;
	static constexpr float	SEARCH_ANGLE_OFFSET_YAW = 48.0f;
	static constexpr float	SEARCH_ANGLE_STEP_PITCH = 10.0f;

	static constexpr float	SEARCH_ANGLE_START_PITCH = 20.f;

// データメンバの宣言 -----------------------------------------------
private:


	// 依存関係
	CollisionManager* m_pCollisionManager;	///< 衝突判定管理
	const Player* m_pPlayer;				///< プレイヤー

	// ワイヤー探索設定 
	float m_detectionRadius;
	float m_wireLength;  ///< ワイヤーの長さ
	float m_wireRadius;  ///< 判定する半径
	MyLib::Ray m_shootingRay;

	// 探索結果のキャッシュ (管理対象)
	std::vector<DirectX::SimpleMath::Vector3> m_grappleTargetPositionCache;	///< 目標座標キャッシュ

	// デバッグ・描画リソース
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch; ///< プリミティブバッチ（レイ描画用）

	const Camera* m_pCamera;

	std::vector<Capsule> m_capsules; ///< カプセル群

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
		const Player* pPlayer,
		const Camera* pCamera);

	// 更新処理
	void Update();

	// 描画処理
	void Draw(const Camera& camera);

	// 終了処理
	void Finalize();

	void PreCollision() override;
	// 衝突処理
	void OnCollision(const CollisionInfo& info) override ;



	// 取得/設定
public:
	// 検索するパラメータの設定
	void SetSearchParameters(DirectX::SimpleMath::Vector3 wireShootingDirection, const float& length, const float& radius);

	// 活動しているかどうか
	bool IsActive() const override { return true; }

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::WIRE_GRAPPING_AREA; }


	DirectX::SimpleMath::Vector3 GetTargetPosition() ;

	// 照準を見つけたかどうか
	bool IsFindTarget() const;


// 内部実装
private:

	// 目標座標を探す
	void RequestSearchingTargetPosition();

	// 飛ばすレイを元に目標座標を取得する(旧バージョン)
	void SearchTargetPositionForShootingRay(MyLib::Ray shootingRay);

	// いくつかの点から目標座標を取得する
	void SearchTargetPositionForSeveralPosition();

	// 調べる方向を取得する
	std::vector<DirectX::SimpleMath::Vector3> GetSearchDirections() const;

	// カプセルコライダーの作成
	static Capsule CreateCapsuleCollider(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& direction, const float& length, const float& radius) ;

	// 衝突情報を元に接地座標を算出する
	bool CalcWireTargetPosition(
		DirectX::SimpleMath::Vector3* pTargetPosition, 
		const Capsule& hitCapsuleCollider,
		const GameObject* pHitObject, 
		const ICollider* pHitCollider);

	// 方向からカプセルコライダーを算出し目標座標の候補を取得する
	std::vector<DirectX::SimpleMath::Vector3> GetTargetPositionCandidatesForDirection(std::vector<DirectX::SimpleMath::Vector3> searchDirections);

	// 最も遠い目標座標を取得する
	DirectX::SimpleMath::Vector3 GetFarTargetPosition(std::vector<DirectX::SimpleMath::Vector3> targetPositions);

};
