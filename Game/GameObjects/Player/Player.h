/*****************************************************************//**
 * @file    Player.h
 * @brief   プレイヤーに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/06/12
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>

#include "Game/GameObjects/Common/GameObject.h"
#include "Game/GameObjects/Common/MovableObject/MovableObject.h"



#include "Game/Common/StateMachine/StateMachine.h"

#include "Game/GameObjects/Wire/IWireHolder/IWireHolder.h"
#include "Game/Common/Event/Messenger/GameFlowMessenger/IGameFlowObserver.h"

#include "Library/DirectXFramework/Animation.h"

// クラスの前方宣言 ===================================================
class SimpleModel;		// モデル
class CommonResources;	// 共通リソース
class Sphere;			// 球
class CollisionManager; // 衝突管理
class Camera;			// カメラ
class WireSystemSubject;// ワイヤーシステムの観察対象
class Wire;				// ワイヤー
class WireTargetFinder;		// ワイヤー照準

namespace MyLib
{
	/**
	 * @brief レイ
	 */
	struct  Ray;
}



// クラスの定義 ===============================================================
/**
 * @brief プレイヤー
 */
class Player 
	: public GameObject		// ゲームオブジェクト
	, public MovableObject	// 移動可能オブジェクト
	, public IWireHolder	// ワイヤーの保持者インターフェース
	, public IGameFlowObserver// ゲーム進行のオブザーバーインターフェース
{
public:

	/**
	 * @brief 状態
	 */
	enum class State
	{
		IDLE,			// 待機
		WALKING,		// 歩く
		JUMPING,		// 跳ぶ
		STTEPPING,		// ステップ
		WIRE_ACTION,	// ワイヤーアクション
		WIRE_GRABBING,	// ワイヤーで掴む
		WIRE_THROWING	// ワイヤーで投げる
	};

// クラス定数の宣言 -------------------------------------------------
public:

	// プレイヤーモデルのファイル名
	static const constexpr char* PLAYER_MODEL_FILE_NAME = "player.sdkmesh";

	// プレイヤーの挙系
	static constexpr float FRICTION = 3.0f;		// 摩擦係数
	static constexpr float AIR_RESISTANCE = 0.05f;  // 空気抵抗係数
	static constexpr float ROTATION_SPEED = 1.0f;	// 回転速度(s)

	const float ACCELERATION = 100.0f;				// 加速力（加速の強さ）
	const float DECELERATION = 60.0f;				// 減速力（ブレーキの強さ）
	const float MAX_MOVE_SPEED = 20.0f;					// 最大速度
	const float MAX_FLYING_MOVE_SPEED = 15.0f;			// 空中速度
	static constexpr float JUMPING_POWER = 1200.0f;

	// ワイヤー関連
	static constexpr float WIRE_LENGTH = 60.0f; // ワイヤーの長さ

	// アニメーション関連
	const static constexpr char* ANIM_WALKING		= "Resources/Models/player_walk.sdkmesh_anim";
	const static constexpr char* ANIM_IDLENG		= "Resources/Models/player_idle.sdkmesh_anim";
	const static constexpr char* ANIM_WIREACTION	= "Resources/Models/player_wireAction.sdkmesh_anim";



	// データメンバの宣言 -----------------------------------------------

	

// データメンバの宣言 -----------------------------------------------
private:

	// グラフィック関連
	const Camera* m_pCamera;
	DirectX::SimpleMath::Matrix m_proj;
	DX::AnimationSDKMESH m_animation;	///< アニメーション
	DirectX::Model m_model;					///< モデル

	// 衝突関連
	std::unique_ptr<Sphere> m_collider;	///< コライダー
	DirectX::SimpleMath::Vector3 m_overlapTotal;

	// システム
	CollisionManager* m_pCollisionManager;
	std::unique_ptr<StateMachine<Player>>	m_stateMachine;	///< ステートマシーン

	// イベントに関するモノ
	DirectX::SimpleMath::Vector3 m_requestedMove;			///<　リクエストされた移動ベクトル


	// デバック用
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
	DirectX::SimpleMath::Vector2  m_cursorPos;


	// シミュレーター
	std::unique_ptr<WireSystemSubject>		m_wireSystemSubject;		///< ワイヤーシステムの観察対象


	// ワイヤー関連
	std::unique_ptr<Wire> m_wire; ///< ワイヤー
	std::unique_ptr<WireTargetFinder> m_wireTargetFinder;	///< ワイヤーの照準を探す

	// フラグ関連
	bool m_isGround;
	bool m_isActive;
	bool m_canStep; ///< ステップ可能かどうか

	// 状態
	State m_state;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect; ///< ベーシックエフェクト　ものを描画する時の必要なものが用意されているクラス　基本形態


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Player();

	// デストラクタ
	~Player();


// 操作
public:
	// 初期化処理
	void Initialize(CommonResources* pCommonResources, CollisionManager* pCollisionManager);

	// 更新処理
	void Update(float elapsedTime, const Camera& camera, const DirectX::SimpleMath::Matrix& proj);

	// 描画処理
	void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);

	// 終了処理
	void Finalize();

// 動作要求関連
public:
	// 移動要求
	void RequestedMovement(DirectX::SimpleMath::Vector3 moveDirection);
	// ジャンプに要求
	void RequestJump();
	// ステップ要求
	void RequestStep();

// 動作処理関連
public:
	// 物理的な移動の適用
	void ApplyPhysic(const float& elapsedTime);
	// 重力の適用
	void ApplyGravity(const float& elapsedTime);

	// 移動入力の適用
	void ApplyMoveInput(const float& elapsedTime);

	// 移動方向へ回転する
	void RotateForMoveDirection(const float& elapsedTime);

	// ワイヤーを離す
	void ReleaseWire();

	// 移動の適用
	void Move(const float& elapsedTime);

	// ワイヤーアクション挙動
	void BehaviourWireAction(const float& elapsedTime);
	// シミュレータの更新処理
	void ApplyWireSimulator(const float& elapsedTime);


	// ワイヤーを飛ばす
	void ShootWire();

	void ResolvePushOutAndBounce(DirectX::SimpleMath::Vector3 overlap, const float& restitution);

	// ジャンプ
	void Jump(float elapsedTime);

	// アニメーション変更要求
	void ChangeAnimation(const std::string& animationFilePath);

	// 押し出し
	void PushOut(DirectX::SimpleMath::Vector3 overlap);

	// ワイヤー発射方向の取得
	MyLib::Ray GetWireShootingRay() const;

// 通知関連
public:
	// 衝突処理
	void OnCollision(GameObject* pHitObject, ICollider* pHitCollider) override;

	// 所持するワイヤーが衝突した時に呼ばれる
	void OnCollisionWire(GameObject* pHitObject) override;	// 移動の要求

	void OnCollisionWithWall(GameObject* pHitObject, ICollider* pHitCollider);

	void OnCollisionWithBuilding(GameObject* pHitObject, ICollider* pHitCollider);

	void PreCollision() override;
	void PostCollision() override;

	// ワイヤーが掴んだ時に呼ばれる
	void OnWireGrabbed(GameObject* pGrabGameObject) override;

	// イベントメッセージを受け取る
	void OnGameFlowEvent(GameFlowEventID eventID) override;


	// 状態の変更要求
	void RequestChangeState(State state);



// 取得/設定
public:

	// 移動要求がされているかどうか
	bool IsMovingRequested();

	// 活動しているかどうか
	bool IsActive() const override {return m_isActive;}

	// カメラの取得
	const Camera* GetCamera() const { return m_pCamera; }

	// 射影行列の取得
	DirectX::SimpleMath::Matrix GetProj() const { return m_proj; }

	// ワイヤーシステムの取得
	WireSystemSubject* GetWireSystem() { return m_wireSystemSubject.get(); }
	
	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::PLAYER; }

	// ワイヤーの取得
	Wire* GetWire() const { return m_wire.get(); }

	// 掴む位置
	DirectX::SimpleMath::Vector3 CalcGrabbingPosition() const;

	const Sphere* GetCollider() const { return m_collider.get(); }

	// ワイヤーを発射できるかどうか
	bool CanShootWire() const;

	// 地面にいるかどうか
	bool IsGround() const { return m_isGround; }




// 内部実装
private:


	// 摩擦の適用
	void ApplyFriction(float elapsedTime);

	// 空気抵抗の適用
	void ApplyDrag(float elapsedTime);



	//// ワイヤーの作成
	//void CreateRope(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& direction, const  int& particleNum, const float& length, const XPBDSimulator::Parameter& param);

	//void CreateRope(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& targetPoint,  const XPBDSimulator::Parameter& param, float particleDistance);

};
