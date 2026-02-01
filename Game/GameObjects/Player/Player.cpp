/*****************************************************************//**
 * @file    Player.h
 * @brief   プレイヤーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/12
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "Player.h"

// シーン関連
#include "Game/Scene/GameplayScene/GameplayScene.h"

// ライブラリ関連
#include "Library/DirectXFramework/DebugDraw.h"
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Library/MyLib/DirectXMyToolKit/DirectXUtils.h"
#include "Library/MyLib/Ray/Ray.h"
#include "Library/DirectXFramework/DeviceResources.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/Event/WireSystemObserver/WireSystemSubject.h"
#include "Game/Common/Framework/Event/WireSystemObserver/IWireEventObserver.h"
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"

// グラフィック関連
#include "Game/Common/Graphics/SimpleModel/SimpleModel.h"
#include "Game/Common/Graphics/Camera/PlayerCamera/PlayerCamera.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"
#include "Game/Common/Utillities/Helper/MovementHelper/MovementHelper.h"
#include "Game/Common/Utillities/Helper/PhysicsHelper/PhysicsHelper.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/WireTargetFinder/WireTargetFinder.h"
#include "Game/Common/GameplayLogic/CollisionManager/CollisionManager.h"

// ゲームオブジェクト
#include "Game/GameObjects/Wire/Wire.h"
#include "Game/Common/Framework/GameObjectRegistry/GameObjectRegistry.h"

// プレイヤーの状態関連
#include "Game/GameObjects/Player/State/IdlePlayerState/IdlePlayerState.h"
#include "Game/GameObjects/Player/State/WalkPlayerState/WalkPlayerState.h"
#include "Game/GameObjects/Player/State/JumpingPlayerState/JumpingPlayerState.h"
#include "Game/GameObjects/Player/State/SteppingPlayerState/SteppingPlayerState.h"
#include "Game/GameObjects/Player/State/WireActionPlayerState/WireActionPlayerState.h"
#include "Game/GameObjects/Player/State/WireGrabbingPlayerState/WireGrabbingPlayerState.cpp.h"
#include "Game/GameObjects/Player/State/WireThrowing/WireThrowingPlayerState.cpp.h"
#include "Game/GameObjects/Player/State/ShootingWireState/ShootingWirePlayerState.h"


// パラメータ
#include "Game/Common/Database/PlayerParameter.h"
#include "Game/Common/Database/PhysicsParameter.h"

using namespace DirectX;



// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Player::Player()
	: m_pCollisionManager	{ nullptr }
	, m_pPlayerCamera		{ nullptr }
	, m_isGround			{ false }
	, m_isActive			{ true }
	, m_canStep				{ true }
	, m_state				{ State::IDLE }
	, m_releaseWireRequested{ false }
	, m_shootWireRequested	{ false }
{
	m_cursorPos = DirectX::SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF() - PlayerParameter::CURSOR_Y_OFFSET_SCALE * Screen::Get()->GetScreenScale());
	// メッセージへの登録
	GameFlowMessenger::GetInstance()->RegistryObserver(this);
}



/**
 * @brief デストラクタ
 */
Player::~Player()
{
	// 衝突管理から削除
	GetCommonResources()->GetCollisionManager()->RemoveCollisionObjectData(this, m_collider.get());
}

/**
 * @brief 初期化処理
 *
 * @param[in] pCommonResources	共通リソース
 * @param[in] pCollisionManager	衝突管理
 * @param[in] pPlayerCamera		プレイヤーカメラ
 * @param[in] pPlayerInput		プレイヤー入力
 */
void Player::Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager, const PlayerCamera* pPlayerCamera)
{
	using namespace SimpleMath;
	m_pPlayerCamera = pPlayerCamera;

	// 共通リソースの設定
	SetCommonResources(pCommonResources);

	m_pCollisionManager = pCollisionManager;

	// モデルの取得
	m_model = GetCommonResources()->GetResourceManager()->CreateModel(PlayerParameter::PLAYER_MODEL_FILE_NAME);
	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto device = GetCommonResources()->GetDeviceResources()->GetD3DDevice();

	GetTransform()->SetPosition(SimpleMath::Vector3(0.0f, PlayerParameter::INITIAL_POS_Y, 0.0f));
	GetTransform()->SetScale(PlayerParameter::DEFAULT_MODEL_SCALE);

	// コライダの作成
	m_collider = std::make_unique<Sphere>(GetTransform()->GetPosition(), GetTransform()->GetScale().x * PlayerParameter::DEFAULT_COLLIDER_RADIUS_FACTOR);

	pCollisionManager->AddCollisionData(CollisionData(this, m_collider.get()));

	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(true);
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	CreateInputLayoutFromEffect<VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());

	// ワイヤーシステム
	m_wireSystemSubject = std::make_unique<WireSystemSubject>();

	// ワイヤーの作成
	m_wire = std::make_unique<Wire>();

	// ワイヤー照準検出器の作成
	m_wireTargetFinder = std::make_unique<WireTargetFinder>();
	m_wireTargetFinder->Initialize(GetCommonResources(), pCollisionManager, this, m_pPlayerCamera);

	XPBDSimulator::Parameter param;
	// 剛性（柔軟力）
	param.flexibility = GameplayScene::ROPE_FIXIBLILLITY;

	// 【ローカル定数化】XPBDの重力係数
	const float GRAVITY_DIVISOR = 2.f;
	param.gravity = Vector3(0.0f, -9.8f / GRAVITY_DIVISOR, 0.0f);
	param.iterations = PlayerParameter::WIRE_SIMULATION_ITERATIONS;

	// ワイヤーの初期化処理　
	// ワイヤーの長さは余裕をもつため1.5倍までとする
	m_wire->Initialize(pCommonResources, pCollisionManager, param, PlayerParameter::WIRE_LENGTH * 1.5f, this, this, this, m_wireSystemSubject.get());


	// デフォルト回転の初期化
	GetTransform()->SetInitialRotation(Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(PlayerParameter::DEFAULT_ROTATION_Y_DEGREE), 0, 0.0f));

	// 回転
	GetTransform()->SetRotation(Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(-90.0f), 0.0f, 0.0f));

	// ステートマシーンの作成
	m_stateMachine = std::make_unique<StateMachine<Player>>(this);
	// ステートマシーンの作成
	m_stateMachine = std::make_unique<StateMachine<Player>>(this);
	m_stateMachine->ChangeState<IdlePlayerState>();

	m_isActive = true;

	// 捕らえた時のイベントの追加
	AddEventListener<CaughtEventData>(GameObjectEventType::CAUGHT,
		[this](const CaughtEventData data)
		{
			if (IsActive())
			{
				m_isActive = false;
				GameFlowMessenger::GetInstance()->Notify(GameFlowEventID::PLAYER_DIE);

			}
		});

	// アニメーション関連の設定
	ChangeAnimation(PlayerParameter::ANIM_IDLENG);
}


/**
 * @brief 更新処理
 *
 * @param[in] deltaTime	経過時間
 * @param[in] camera		カメラ
 */
void Player::Update(float deltaTime)
{
	if (m_isActive == false) { return; }

	if (m_isGround)
	{
		m_canStep = true;
	}

	ApplyEvents();

	auto kb = Keyboard::Get().GetState();

	// ワイヤーの更新処理
	m_wire->Update(deltaTime);

	// 状態の更新処理
	m_stateMachine->Update(deltaTime);

	// アニメーション時間がアニメーション終了時間より小さい場合はアニメーションを繰り返す
	if (m_animation.GetAnimTime() < m_animation.GetEndTime())
	{
		// アニメーションを更新する
		m_animation.Update(deltaTime);
	}
	else
	{
		// アニメーションの開始時間を設定する
		m_animation.SetStartTime(0.0);
	}

	// ワイヤー照準検出器の設定
	m_wireTargetFinder->SetSearchParameters(GetWireShootingRay().direction, PlayerParameter::WIRE_LENGTH, 0.5f);
	m_wireTargetFinder->Update();

}



/**
 * @brief 描画処理
 *
 * @param[in] view　ビュー行列
 * @param[in] projection　射影行列
 */
void Player::Draw(const Camera& camera)
{
	const Transform* transform = GetTransform();

	using namespace SimpleMath;
	if (m_isActive == false) { return; }

	// ワイヤーの描画処理
	m_wire->Draw(camera);

	// 状態の描画処理
	m_stateMachine->Draw();

	auto states = GetCommonResources()->GetCommonStates();

	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();


	Matrix defaultRotation = Matrix::CreateFromQuaternion(transform->GetInitialRotation());

	Matrix defaultTransform = Matrix::CreateTranslation(Vector3(0.0f, PlayerParameter::MODEL_DEFAULT_OFFSET_Y * transform->GetScale().y, 0.0f));

	// ワイヤーアクション中の場合はモデルの位置を調整する
	if (m_state == State::WIRE_ACTION)
	{
		defaultTransform *= Matrix::CreateTranslation(Vector3(PlayerParameter::WIRE_ACTION_OFFSET_X, PlayerParameter::WIRE_ACTION_OFFSET_Y, 0.0f));
		// プレイヤーのY軸回転を
	}

	Matrix transformMat = Matrix::CreateTranslation(transform->GetPosition());
	Matrix rotation = Matrix::CreateFromQuaternion(transform->GetRotation());
	Matrix scale = Matrix::CreateScale(transform->GetScale());

	Matrix world = defaultRotation * defaultTransform * scale * rotation * transformMat;


	auto drawBones = DirectX::ModelBone::MakeArray(m_model.bones.size());
	// ボーン数を取得する
	size_t nbones = m_model.bones.size();

	// アニメションにモデルを適用する
	m_animation.Apply(m_model, nbones, drawBones.get());

	// スキン変形用行列を適用する
	m_animation.ApplySkinMatrix(m_model, nbones, drawBones.get());
	// アニメーションモデルを描画する
	m_model.DrawSkinned(context, *states, nbones, drawBones.get(), world, camera.GetViewMatrix(), camera.GetProjectionMatrix());


	//// **** 軸の描画 ****

	//// ブレンドステート
	//context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFF);
	//// 深度ステンシルバッファ
	//context->OMSetDepthStencilState(states->DepthDefault(), 0);
	//// カリング
	//context->RSSetState(states->CullNone());

	//// インプットレイアウトの設定
	//context->IASetInputLayout(m_inputLayout.Get());

	//// ベーシックエフェクト
	//m_basicEffect->SetView(camera.GetViewMatrix());
	//m_basicEffect->SetProjection(camera.GetProjectionMatrix());
	//m_basicEffect->Apply(context);

	//SimpleMath::Vector3 forward = transform->GetForward() * 1.5f;

	//

	/*m_primitiveBatch->Begin();
	DX::DrawRay(m_primitiveBatch.get(), GetPosition(), forward, false, Colors::Red);
	m_primitiveBatch->End();*/

	// 目標方向の描画
	//SimpleMath::Vector3 targetDirection = m_targetPosition - transform->GetPosition();
	//m_primitiveBatch->Begin();
	//DX::DrawRay(m_primitiveBatch.get(), transform->GetPosition(), targetDirection, false, m_targetGuideColor);
	//m_primitiveBatch->End();


	//GetCommonResources()->GetDebugFont()->AddString(100, 90, Colors::White, L"position : %f, %f, %f ", GetPosition().x, GetPosition().y, GetPosition().z);
	//GetCommonResources()->GetDebugFont()->AddString(100, 110, Colors::White, L"velocity : %f, %f, %f ", GetVelocity().x, GetVelocity().y, GetVelocity().z);
	//GetCommonResources()->GetDebugFont()->AddString(100, 130, Colors::White, L"speed : %f ", GetVelocity().Length());

	// ワイヤー照準検出器の表示
	//m_wireTargetFinder->Draw(camera);

	//m_collider->Draw(context, view, proj);

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Player::Finalize()
{
	// 状態の終了処理
	m_stateMachine->ClearState();
}

/**
 * @brief 衝突処理
 *
 * @param[in] info 衝突情報
 */
void Player::OnCollision(const CollisionInfo& info)
{
	if (info.pOtherObject->GetTag() == GameObjectTag::FLOOR)
	{
		OnCollisionWithBuilding(info.pOtherObject, info.pOtherCollider);
	}

	// 壁と衝突した場合
	else if (info.pOtherObject->GetTag() == GameObjectTag::WALL)
	{

		// 壁との衝突処理
		OnCollisionWithWall(info.pOtherObject, info.pOtherCollider);
	}
	else if (info.pOtherObject->GetTag() == GameObjectTag::BUILDING)
	{
		OnCollisionWithBuilding(info.pOtherObject, info.pOtherCollider);
	}

	if (info.pOtherObject->GetTag() == GameObjectTag::CHECKPOINT)
	{
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_PLAYER_PASSING), false);
	}

}

/**
 * @brief 所持するワイヤーが衝突した時に呼ばれる
 *
 * @param[in] pHitGameObject 衝突したもの
 */
void Player::OnCollisionWire(GameObject* pHitObject)
{
	m_collisionWire(pHitObject);

}


/**
 * @brief 壁との衝突処理
 *
 * @param[in] pHitObject    衝突したゲームオブジェクト
 * @param[in] pHitCollider  衝突したコライダ
 */
void Player::OnCollisionWithWall(GameObject* pHitObject, ICollider* pHitCollider)
{
	UNREFERENCED_PARAMETER(pHitObject);



	// 矩形にキャスト
	const Box2D* pHitBox = dynamic_cast<const Box2D*>(pHitCollider);


	// 押し出しの計算
	SimpleMath::Vector3 overlap = CalcOverlap(*pHitBox, *m_collider);

	SimpleMath::Vector3 overlapRaw = overlap;

	SimpleMath::Vector3 overlapDir = overlap;
	overlapDir.Normalize();


	// 現在の押し出しの単位ベクトル
	SimpleMath::Vector3 totalOverlapNormal = m_overlapTotal;
	totalOverlapNormal.Normalize();

	// 押し出し方向ベクトル（累積方向 + 今回の方向）
	SimpleMath::Vector3 combinedDirection = totalOverlapNormal + overlapDir;

	if (combinedDirection.LengthSquared() < PlayerParameter::SQUARED_ZERO_THRESHOLD)
		return;

	combinedDirection.Normalize();

	// それぞれの押し出しを、合成方向に投影して必要量を求める
	float push1 = std::max(0.0f, overlap.Dot(combinedDirection));
	float push2 = std::max(0.0f, m_overlapTotal.Dot(combinedDirection));

	// より大きな押し出し量を採用
	float projectedDepth = std::max(push1, push2);

	// 合成押し出しを更新
	m_overlapTotal += combinedDirection * projectedDepth;

	if (overlap.y > 0.0f)
	{
		m_isGround = true;
	}
}


void Player::OnCollisionWithBuilding(GameObject* pHitObject, ICollider* pHitCollider)
{
	UNREFERENCED_PARAMETER(pHitObject);
	if (State::STTEPPING == m_state /*|| State::WIRE_ACTION == m_state*/)
	{
		m_stateMachine->ChangeState<IdlePlayerState>();
	}
	// 矩形にキャスト
	const AABB* pHitBox = dynamic_cast<const AABB*>(pHitCollider);


	// 押し出しの計算
	SimpleMath::Vector3 overlap = CalcOverlap(*pHitBox, *m_collider);


	SimpleMath::Vector3 overlapDir = overlap;
	overlapDir.Normalize();


	// 現在の押し出しの単位ベクトル
	SimpleMath::Vector3 totalOverlapNormal = m_overlapTotal;
	totalOverlapNormal.Normalize();

	// 押し出し方向ベクトル（累積方向 + 今回の方向）
	SimpleMath::Vector3 combinedDirection = totalOverlapNormal + overlapDir;

	if (combinedDirection.LengthSquared() < PlayerParameter::SQUARED_ZERO_THRESHOLD)
		return;

	combinedDirection.Normalize();

	// それぞれの押し出しを、合成方向に投影して必要量を求める
	float push1 = std::max(0.0f, overlap.Dot(combinedDirection));
	float push2 = std::max(0.0f, m_overlapTotal.Dot(combinedDirection));

	// より大きな押し出し量を採用
	float projectedDepth = std::max(push1, push2);

	// 合成押し出しを更新
	m_overlapTotal += combinedDirection * projectedDepth;
}
void Player::PreCollision()
{
	m_isGround = false;
}
/**
 * @brief 衝突をした直後に呼ばれる
 *
 */
void Player::PostCollision()
{
	if (std::abs(m_overlapTotal.LengthSquared()) > PlayerParameter::SQUARED_ZERO_THRESHOLD)
	{
		SimpleMath::Vector3 overlap = m_overlapTotal;

		// **** 押し出しとバウンド ****
		ResolvePushOutAndBounce(overlap, 0.0f);

		m_overlapTotal = SimpleMath::Vector3::Zero;

		if (overlap.y > 0.0f)
		{
			m_isGround = true;
		}
	}
}
/**
 * @brief ワイヤーが掴んだ時に呼ばれる
 *
 * @param[in] pGrabGameObject　掴まれたオブジェクト
 */
void Player::OnWireGrabbed(GameObject* pGrabGameObject)
{
	UNREFERENCED_PARAMETER(pGrabGameObject);

	//RequestChangeState(Player::State::WIRE_GRABBING);


}

/**
 * @brief イベントメッセージを受け取る
 *
 * @param[in] eventID　イベントID
 */
void Player::OnGameFlowEvent(GameFlowEventID eventID)
{
	switch (eventID)
	{
	case GameFlowEventID::GAME_START:
	{
		// 宝の取得
		auto treasure = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::TREASURE);
		if (treasure == nullptr) { break; }

		// 対象座標の取得
		m_targetPosition = treasure->GetTransform()->GetPosition();
		m_targetGuideColor = Colors::LightGreen;
		break;
	}
	case GameFlowEventID::SPAWN_HELICOPTER:
	{
		// ヘリコプターの取得
		auto helicopter = GameObjectRegistry::GetInstance()->GetGameObject(GameObjectTag::ESCAPE_HELICOPTER);
		if (helicopter == nullptr) { break; }

		// 対象座標の更新
		m_targetPosition = helicopter->GetTransform()->GetPosition();
		m_targetGuideColor = Colors::Purple;

		break;
	}
	case GameFlowEventID::PLAYER_DIE:
		break;
	case GameFlowEventID::ESCAPE_SUCCESS:
		break;
	default:
		break;
	}
}

/**
 * @brief 移動の設定
 *
 * @param[in] moveDirection
 *
  * @returns true  成功
 * @returns false 失敗
 */
bool Player::RequestedMovement(DirectX::SimpleMath::Vector3 moveDirection)
{

	// 正規化
	if (moveDirection.LengthSquared() > 0.0f) {
		moveDirection.Normalize();
	}

	m_requestedMove = moveDirection;
	return true;
}

/**
 * @brief 移動の適用
 *
 * @param[in] deltaTime　経過時間
 */
void Player::Move(const float& deltaTime)
{

	SimpleMath::Quaternion rotate = GetTransform()->GetRotation();


	// **** 座標の更新 ****
	// 座標の算出
	SimpleMath::Vector3 position = GetTransform()->GetPosition() + GetVelocity() * deltaTime;
	GetTransform()->SetPosition(position);

	// コライダの更新処理
	m_collider->Transform(position);
}

/**
 * @brief シミュレータの更新処理
 *
 * @param[in] deltaTime　経過時間
 */
void Player::ApplyWireSimulator(const float& deltaTime)
{
	m_wire->ApplyWireSimulator(deltaTime);

}

/**
 * @brief 移動要求されているかどうか
 *
 * @returns true  要求されている
 * @returns false 要求されていない
 */
bool Player::IsMovingRequested()
{
	return (m_requestedMove.LengthSquared() > 0.0f);
}

/**
 * @brief 物理的な移動
 *
 * @param[in] deltaTime
 */
void Player::ApplyPhysic(const float& deltaTime)
{
	// 重力を加える
	ApplyGravity(deltaTime);

	if (IsGround())
	{
		// 摩擦を加える
		ApplyFriction(deltaTime);
	}

	// 空気抵抗の適用
	ApplyDrag(deltaTime);

	// 回転する
	RotateForMoveDirection(deltaTime);

}

/**
 * @brief 摩擦を加える
 */
void Player::ApplyFriction(float deltaTime)
{
	// 摩擦を加える
	SimpleMath::Vector3 velocity = PhysicsHelper::CalculateFrictionVelocity(GetVelocity(), deltaTime, PlayerParameter::FRICTION, PhysicsParameter::GRAVITY_ACCELERATION.Length());

	SetVelocity(velocity);
}

/**
 * @brief 空気抵抗の適用
 */
void Player::ApplyDrag(float deltaTime)
{
	// 適用
	auto velocity = PhysicsHelper::CalculateDragVelocity(GetVelocity(), deltaTime, PlayerParameter::AIR_RESISTANCE);

	SetVelocity(velocity);
}



/**
 * @brief 移動方向へ回転する
 *
 */
void Player::RotateForMoveDirection(const float& deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);


	GetTransform()->SetRotation(MovementHelper::RotateForMoveDirection(
		deltaTime,
		GetTransform()->GetRotation(),
		GetTransform()->GetForward(),
		GetVelocity(),
		PlayerParameter::ROTATE_SPEED_FACTOR));
}

/**
 * @brief ワイヤーを離す
 */
void Player::ReleaseWire()
{
	if (m_wire->IsActive())
		SetVelocity(m_wire->GetStartVelocity() * PlayerParameter::WIRE_RELEASE_VELOCITY_MULTIPLIER);
	m_wire->Reset();

}

/**
 * @brief 重力を適用
 *
 * @param[in] deltaTime　経過時間
 */
void Player::ApplyGravity(const float& deltaTime)
{
	SimpleMath::Vector3 accel = SimpleMath::Vector3::Zero;

	accel = PhysicsParameter::GRAVITY_ACCELERATION * deltaTime;

	AddForceToVelocity(accel);
}

/**
 * @brief 移動入力の適用
 *
 * @param[in] deltaTime
 */
void Player::ApplyMoveInput(const float& deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	using namespace SimpleMath;

	float maxMoveSpeed = (IsGround()) ? PlayerParameter::MAX_MOVE_SPEED : PlayerParameter::MAX_FLYING_MOVE_SPEED;
	// XZ平面の速度をクランプした移動要求に基づいて更新
	auto newVelocity = MovementHelper::ClampedMovement(
		GetVelocity() * Vector3(1.0f, 0.0f, 1.0f),
		m_requestedMove,
		deltaTime,
		PlayerParameter::ACCELERATION,
		PlayerParameter::DECELERATION,
		maxMoveSpeed);
	// Y軸の速度はそのまま保持
	AddForceToVelocity(newVelocity);

	// 移動要求の解除
	m_requestedMove = SimpleMath::Vector3::Zero;
}


DirectX::SimpleMath::Vector3 Player::CalcGrabbingPosition() const
{

	using namespace SimpleMath;
	float length = PlayerParameter::GRABBING_DISTANCE;


	return GetTransform()->GetPosition() + -GetTransform()->GetForward() * length;
}

bool Player::CanShootWire() const
{
	return m_wireTargetFinder->IsFindTarget();
}

/**
 * @brief ワイヤー衝突時に呼ぶ関数を登録する
 *
 * @param[in] wireCollision　衝突関数
 */
void Player::SetWireCollisionFunction(std::function<void(const GameObject*)> wireCollision)
{
	m_collisionWire = wireCollision;
}

void Player::ChangeAnimation(const std::string& animationFilePath)
{

	std::wstring fileString = std::wstring(animationFilePath.begin(), animationFilePath.end());
	const wchar_t* name = fileString.c_str();

	// アニメーションをロードする
	DX::ThrowIfFailed(m_animation.Load(name));

	// アニメーションをモデルにバインドする
	m_animation.Bind(m_model);

	// アニメーションの開始時間を設定する
	m_animation.SetStartTime(0.0);


}


/**
 * @brief ワイヤーアクション挙動
 *
 * @param[in] 経過時間
 *
 */
void Player::BehaviourWireAction(const float& deltaTime, const float& speed)
{
	m_canStep = true;
	// ワイヤーシミュレータの適用
	ApplyWireSimulator(deltaTime * speed);


	if (m_wire->IsActive())
	{
		GetTransform()->SetPosition(m_wire->GetEndPosition());
	}
}

/**
 * @brief ワイヤーを飛ばす
 *
 */
void Player::ShootWire(const DirectX::SimpleMath::Vector3& targetPosition)
{
	auto eyePos = m_pPlayerCamera->GetEye();

	// テスト ------------------------------------------------------

	using namespace SimpleMath;
	MyLib::Ray ray;

	// レイの算出
	MyLib::CalcScreenToWorldRay(
		m_cursorPos.x, m_cursorPos.y,
		Screen::Get()->GetWidthF(), Screen::Get()->GetHeightF(),
		GetCamera()->GetEye(), GetCamera()->GetViewMatrix(), GetCamera()->GetProjectionMatrix(),
		&ray.direction, &ray.origin);
	Vector3 maxPos = ray.origin + ray.direction * PlayerParameter::MAX_TARGETING_RAY_DISTANCE;

	//Vector3 wireLaunchDirection = maxPos - GetPosition();
	Vector3 wireLaunchDirection = targetPosition - GetTransform()->GetPosition();
	wireLaunchDirection.Normalize();

	m_wire->ShootWireToTarget(GetTransform()->GetPosition(), m_wireTargetFinder->GetTargetPosition(), PlayerParameter::SHOOT_WIRE_INITIAL_SPEED);

}

/**
 * @brief 押し出しと反射の解消
 *
 * @param[in] overlap      押し出しベクトル
 * @param[in] restitution  反発係数（バウンドする強さ）　0.0 = 全くバウンドしない、1.0 = 完全反射
 */
void Player::ResolvePushOutAndBounce(DirectX::SimpleMath::Vector3 overlap, const float& restitution)
{
	PushOut(overlap);

	SimpleMath::Vector3 overlapRaw = overlap;
	SimpleMath::Vector3 overlapDir = overlap;
	overlapDir.Normalize();

	float dot = GetVelocity().Dot(overlapDir);

	// 速度の反転（バウンド成分）
	SimpleMath::Vector3 bounce = -overlapDir * dot * restitution;

	// 壁に沿った滑り成分
	SimpleMath::Vector3 slide = GetVelocity() - overlapDir * dot;

	// 最終的な速度
	SetVelocity(slide + bounce);
}

/**
 * @brief ジャンプ
 */
void Player::Jump(float deltaTime)
{
	// SEの再生
	SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_PLAYER_JUMP), false);


	AddForceToVelocityY(PlayerParameter::JUMPING_POWER * deltaTime);
}

/**
 * @brief ジャンプに挑戦
 *
 * @returns true  成功
 * @returns false 失敗
 */
bool Player::RequestJump()
{
	if (!m_isGround) { return false; }

	if (m_state == State::WIRE_ACTION ||
		m_state == State::WIRE_GRABBING ||
		m_state == State::WIRE_THROWING ||
		m_state == State::JUMPING)
	{
		return false;
	}

	m_stateMachine->ChangeState<JumpingPlayerState>();
	return true;

}

/**
 * @brief ステップ要求
 *
 * @returns true  成功
 * @returns false 失敗
 */
bool Player::RequestStep()
{
	if (m_canStep && !m_isGround)
	{
		m_stateMachine->ChangeState<SteppingPlayerState>();
		m_canStep = false;
		return true;
	}

	return false;
}

/**
 * @brief ワイヤーを離すよう要求
 *
 * @returns true  成功
 * @returns false 失敗
 */
bool Player::RequestReleaseWire(bool isReleased)
{
	m_releaseWireRequested = isReleased;

	return true;
}

/**
 * @brief ワイヤーを飛ばすように要求
 * 
 * @returns true  成功
 * @returns false 失敗 
 */
bool Player::RequestShootWire(bool isShooting)
{
	m_shootWireRequested = isShooting;

	return true;
}

/**
 * @brief 押し出しをする
 *
 * @param[in] overlap
 */
void Player::PushOut(DirectX::SimpleMath::Vector3 overlap)
{
	// 押し出す
	GetTransform()->SetPosition(GetTransform()->GetPosition() + overlap * 0.99f);

	m_collider->Transform(GetTransform()->GetPosition());
}

/**
 * @brief ワイヤー発射方向の取得
 * 
 * @return 
 */
MyLib::Ray Player::GetWireShootingRay() const
{
	MyLib::Ray ray;

	// レイの算出
	MyLib::CalcScreenToWorldRay(
		m_cursorPos.x, m_cursorPos.y,
		Screen::Get()->GetWidthF(), Screen::Get()->GetHeightF(),
		GetCamera()->GetEye(), GetCamera()->GetViewMatrix(), GetCamera()->GetProjectionMatrix(),
		&ray.direction, &ray.origin);


	SimpleMath::Vector3 maxPos = ray.origin + ray.direction * PlayerParameter::MAX_TARGETING_RAY_DISTANCE;

	SimpleMath::Vector3 wireLaunchDirection = maxPos - GetTransform()->GetPosition();
	wireLaunchDirection.Normalize();

	MyLib::Ray resultRay(GetTransform()->GetPosition(), wireLaunchDirection);

	return resultRay;
}

