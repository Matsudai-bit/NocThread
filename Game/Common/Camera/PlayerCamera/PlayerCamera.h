/*****************************************************************//**
 * @file    PlayerCamera.h
 * @brief   プレイヤーのカメラ
 * 
 * @author  松下大暉
 * @date    2025/6/15
 *********************************************************************/
#pragma once
#include "Game/Common/Camera/Camera.h"

#include <memory>
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/GameObjects/Common/MovableObject/MovableObject.h"
#include "Game/Common/Collision/Collision.h"

#include "Game/Common/Helper/PhysicsHelper/PhysicsHelper.h"

class Player;
class CollisionManager;

// デバッグ用カメラクラス
class PlayerCamera 
	: public Camera
	, public GameObject
	, public MovableObject

{

	// カメラの距離
	static const float DEFAULT_CAMERA_DISTANCE;

private:

	const Player* m_pPlayer;

	bool m_isStartUpdatig;

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

private:



public:

	// コンストラクタ
	PlayerCamera(int windowWidth, int windowHeight,  DirectX::Mouse::ButtonStateTracker* pMouseTracker);

	// 初期化処理
	void Initialize(CommonResources* pCommonResources, CollisionManager* pCollisionManager);

	// デバッグカメラの更新
	void Update(float deltaTime);

	// 画面サイズの設定関数
	void SetWindowSize(int windowWidth, int windowHeight);

	// 画面サイズの取得関数
	void GetWindowSize(int& windowWidth, int& windowHeight);

	// プレイヤーの設定
	void SetPlayer(const Player* pPlayer);

	// 衝突処理
	void OnCollision(GameObject* pHitObject, ICollider* pHitCollider) override;

	void PostCollision() override;
	void PreCollision() override;

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

	// カメラの位置の更新処理
	void UpdateCameraPosition(float deltaTime, const DirectX::SimpleMath::Vector3& targetCameraPosition);

	// 建物との衝突判定処理
	void OnCollisionWithBuilding(GameObject* pHitObject, ICollider* pHitCollider);
	
	// マウスによるカメラの回転処理
	void Motion(int x, int y);


};

