/*****************************************************************//**
 * @file    PlayerCamera.h
 * @brief   プレイヤーのカメラ
 * 
 * @author  松下大暉
 * @date    2025/6/15
 *********************************************************************/
#pragma once

#include <memory>

// フレームワーク関連
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/IGameFlowObserver.h"

// グラフィック関連
#include "Game/Common/Graphics/Camera/Camera.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/GameObjects/Common/MovableObject/MovableObject.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"
#include "Game/Common/Utillities/Helper/PhysicsHelper/PhysicsHelper.h"


class Player;
class CollisionManager;

// デバッグ用カメラクラス
class PlayerCamera 
	: public Camera
	, public MovableObject
	, public IGameFlowObserver

{
// 定数
public:


	// カメラの距離
	static constexpr float DEFAULT_CAMERA_DISTANCE = 5.0f;


	static constexpr float MAX_PITCH_RADIAN = DirectX::XMConvertToRadians(60.0f);
	static constexpr float MIN_PITCH_RADIAN = DirectX::XMConvertToRadians(-80.0f);

	// 衝突体関連 
	static constexpr float COLLIDER_RADIUS = 1.0f;                             // 衝突判定用球コライダーの半径

	// 物理・衝突処理の閾値 
	static constexpr float SQUARED_ZERO_THRESHOLD = 1e-6f;                     // ベクトル長二乗のゼロ判定閾値 (OnCollisionWithBuildingで使用)
	static constexpr float OVERLAP_THRESHOLD = 0.0f;                           // 押し出し処理を行うベクトル長二乗の閾値 (PostCollisionで使用)
	static constexpr float DISTANCE_SQUARED_THRESHOLD = 2.0f * 2.0f;           // 押し出し後の強制位置更新を行う距離の閾値 (PostCollisionで使用, 2.0f^2)

	// マウス入力・感度 
	static constexpr float MOUSE_INPUT_THRESHOLD = 0.001f;                     // マウス入力変化を検出する閾値
	static constexpr float ROTATION_SENSITIVITY = 280.0f;                      // マウス移動量から回転ラジアンへの変換係数（感度）

	// カメラの追従設定 
	static constexpr float FOLLOW_DISTANCE = 12.5f;                            // プレイヤー後方からの理想的な追従距離
	static constexpr float FOLLOW_OFFSET_Y = 0.5f;                             // 理想的な追従位置のY軸（高さ）補正
	static constexpr float TARGET_POSITION_UPDATE_THRESHOLD = 0.01f;           // 目標位置の更新を維持するための微小な変化閾値
	static constexpr float TARGET_STOP_DISTANCE_LARGE = 0.1f;                  // 追従を続ける距離の閾値 (速度 LERP)
	static constexpr float TARGET_STOP_DISTANCE_SMALL = 0.001f;                // 追従を停止し、位置を一致させる距離の閾値

	// カメラの速度設定 
	static constexpr float FOLLOW_SPEED_MULTIPLIER = 20.0f;                    // 目標位置への追従速度係数 (LERP/減速運動係数)

	// 注視点とローカル方向 
	static constexpr float LOOK_TARGET_DISTANCE = 20.0f;                       // 注視点計算に使う、プレイヤーからの前方距離
	static constexpr float LOCAL_FORWARD_Y = -0.1f;                            // 理想的なカメラ視点計算に使うローカル前方ベクトルのY成分
	static constexpr float LOCAL_FORWARD_Z = -1.0f;                            // 理想的なカメラ視点計算に使うローカル前方ベクトルのZ成分

	// Upベクトル補正 
	static constexpr float UP_VECTOR_CORRECTION_FACTOR = 0.1f;

private:

	const Player* m_pPlayer;

	bool m_isStartUpdating;

	// 回転
	DirectX::SimpleMath::Quaternion m_rotate;

	float m_sx, m_sy;

	float m_rotX;
	float m_rotY;

	int m_mousePrevX;
	int m_mousePrevY;

	// スクリーンサイズ
	int m_screenW, m_screenH;
	HWND m_hWnd;

	// マウストラッカー
	DirectX::Mouse::ButtonStateTracker* m_pMouseTracker;

	// コライダ
	std::unique_ptr<Sphere> m_sphereCollider;	///< 球コライダー

	DirectX::SimpleMath::Vector3 m_overlapTotal;
	DirectX::SimpleMath::Vector3 m_nextCameraTargetPosition;

	bool m_isStoppingUpdate;
	bool m_isFirstFrameUpdate; // 最初のフレーム更新フラグ

private:



public:

	// コンストラクタ
	PlayerCamera(int windowWidth, int windowHeight,  DirectX::Mouse::ButtonStateTracker* pMouseTracker);

	// 初期化処理
	void Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager);

	// デバッグカメラの更新
	bool UpdateTask(float deltaTime) override;

	// 画面サイズの設定関数
	void SetWindowSize(int windowWidth, int windowHeight);

	// 画面サイズの取得関数
	void GetWindowSize(int& windowWidth, int& windowHeight);

	// プレイヤーの設定
	void SetPlayer(const Player* pPlayer);

	// 衝突処理
	void OnCollision(const CollisionInfo& info) override;

	void PostCollision() override;
	void PreCollision() override;

	// イベントメッセージを受け取る
	void OnGameFlowEvent(GameFlowEventID eventID) override;

	// プレイヤーの向いている方向をみる
	void LookAtPlayerForward();

	// 取得/設定
public:

	// 活動しているかどうか
	bool IsActive() const override { return true; };

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::CAMERA; };

// 内部実装
private:

	// 回転の更新処理
	void UpdateRotation(int mouseX, int mouseY);

	// カメラの目標地点の算出
	static DirectX::SimpleMath::Vector3 CalculateCameraTargetPosition(
		const DirectX::SimpleMath::Quaternion& cameraRotate, 
		const DirectX::SimpleMath::Vector3& playerPos,
		const DirectX::SimpleMath::Vector3& prevNextCameraTargetPosition);

	// カメラの注視点の算出
	static DirectX::SimpleMath::Vector3 CalculateCameraTarget(
		const DirectX::SimpleMath::Vector3& playerPosition,
		const DirectX::SimpleMath::Vector3& cameraEye);// カメラ座標からプレイヤーを見た方向を算出


	// カメラの位置の更新処理
	void UpdateCameraPosition(float deltaTime, const DirectX::SimpleMath::Vector3& targetCameraPosition);


	// 建物との衝突判定処理
	void OnCollisionWithBuilding(GameObject* pHitObject, ICollider* pHitCollider);
	
	// マウスによるカメラの回転処理
	void Motion(int x, int y);


};

