/*****************************************************************//**
 * @file    Wire.h
 * @brief   ワイヤーに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/02
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include "Game/GameObjects/Common/GameObject.h"

// シミュレーション関連
#include "Game/GameObjects/RopeObject/XPBDSimulator/XPBDSimulator.h"	// XPBDのシミュレーション
#include "Game/GameObjects/RopeObject/ParticleObject/ParticleObject.h"	// パーティクルオブジェクト
#include "Game/GameObjects/RopeObject/RopeObject.h"						// ロープオブジェクト

// ツール
#include "Library/MyLib/Ray/Ray.h"

// クラスの前方宣言 ===================================================
class WireSystemSubject;	// ワイヤーオブザーバーの観察対象
class Sphere;				// 球
class CollisionManager;		// 衝突管理
class IWireHolder;			// ワイヤーの保持者
class MovableObject;

// クラスの定義 ===============================================================

/**
 * @brief ワイヤーオブジェクト
 */
class Wire
	: public GameObject
{
// クラス定数の宣言 -------------------------------------------------
public:


// 構造体
private:

	/**
	 * @brief 所有者の情報
	 */
	struct OwnerData
	{
		const GameObject* pGameObject;		// ゲームオブジェクト
		const MovableObject* pMovableObject; // 移動オブジェクト
		IWireHolder*	 pHolderInterface;	// 保持者のインターフェース
	};



// データメンバの宣言 -----------------------------------------------
private:

	// シミュレーター
	std::unique_ptr<XPBDSimulator>					m_simulator;		///< シミュレータ
	std::unique_ptr<RopeObject>						m_ropeObject;		///< ロープオブジェクト
	std::vector< std::unique_ptr<ParticleObject>>	m_particleObjects;	///< パーティクル

	// システム
	WireSystemSubject*					m_wireSystemSubject;///< ワイヤーシステムの観察対象
	CollisionManager*					m_pCollisionManager;///< 衝突管理
	
	// 部品
	std::unique_ptr<Segment>	m_collider;		///< 線分コライダ
	MyLib::Ray			m_extentionRay;			///< 伸びる方向
	float				m_length;				///< 長さ
	XPBDSimulator::Parameter m_simulationParam;

	DirectX::SimpleMath::Vector3 m_wireVelocity;
	DirectX::SimpleMath::Vector3 m_wireTargetPosition; ///< ワイヤーの目標地点
	float m_wireSpeed; ///< ワイヤーの速さ

	bool m_isActive;

	OwnerData m_owner;

	// 仮
	bool m_isExtention = false;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Wire();

	// デストラクタ
	~Wire();


// 操作
public:
	// 初期化処理
	void Initialize(
		const CommonResources*	pCommonResources,
		CollisionManager*	pCollisionManager,
		const XPBDSimulator::Parameter& simulationParam,
		const float&		length,
		const GameObject*	pOwnerGameObject,
		const MovableObject* pMovableObject,
		IWireHolder*		pHolderInterface,
		WireSystemSubject*	wireSystemSubject = nullptr);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(const Camera& camera);

	// 終了処理
	void Finalize();

	// リセット
	void Reset();

	// ワイヤーシミュレータの適用
	void ApplyWireSimulator(const float& deltaTime);

	// ワイヤーの発射
	void ShootWire(const DirectX::SimpleMath::Vector3&  origin, const DirectX::SimpleMath::Vector3& wireVelocity);
	void ShootWireToTarget(const DirectX::SimpleMath::Vector3&  origin, const DirectX::SimpleMath::Vector3& targetPosition, const float& speed);

	// ワイヤーの作成
	bool CreateRope(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& direction, const  int& particleNum, const float& length, const XPBDSimulator::Parameter& param);

	bool CreateRope(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& targetPoint, const XPBDSimulator::Parameter& param, float particleDistance);

	// 衝突処理
	void OnCollision(const CollisionInfo& info) override;



// 取得/設定
public:
	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::WIRE; }

	bool IsActive() const override{ return m_isActive; }

	// ワイヤーの終端座標の取得
	DirectX::SimpleMath::Vector3 GetEndPosition() const;

	// 要修正
	DirectX::SimpleMath::Vector3 GetStartVelocity() const;

	// 主軸のパーティクルの取得
	ParticleObject* GetBackPivot() const;
	ParticleObject* GetFrontPivot() const;

	// ワイヤーの原点座標の設定
	void SetOriginPosition(const DirectX::SimpleMath::Vector3& origin);
	

// 内部実装
private:


};
