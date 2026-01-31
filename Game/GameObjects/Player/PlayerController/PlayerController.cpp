/*****************************************************************//**
 * @file    PlayerController.h
 * @brief   プレイヤーのコントローラに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PlayerController.h"

// ファクトリー関連
#include "Game/Common/Factory/InputBindingFactory/InputBindingFactory.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Player/Player.h"

// フレームワーク関連
#include "Game/Common/Framework/Input/InputManager/InputManager.h"
#include "Game/Common/Framework/Input/InputActionMap/InputActionMap.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"

// グラフィック関連
#include "Game/Common/Graphics/Camera/Camera.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PlayerController::PlayerController()
	: m_pCamera			 { nullptr }
	, m_pPlayer			 { nullptr }
	, m_pInputManager	 { nullptr }
	, m_isSteppingRequest{ false }
	, m_isJumpingRequest { false }
	, m_isReleaseWireRequested{ false }
{
}



/**
 * @brief デストラクタ
 */
PlayerController::~PlayerController()
{
	UnBindCallbackToInput();
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PlayerController::Initialize(const Camera* pCamera, Player* pPlayer, InputManager* pInputManager)
{
	m_pCamera = pCamera;

	m_pPlayer = pPlayer;

	m_pInputManager = pInputManager;
	RegisterBindCallbackToInput();

}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void PlayerController::Update(
	float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);


	TryWalk();
	
	bool isJumpSuccess = false;
	// ジャンプ
	isJumpSuccess = TryJumping();
	
	// ステップ
	TryStep(isJumpSuccess);

	m_pPlayer->RequestReleaseWire(m_isReleaseWireRequested);
	m_isReleaseWireRequested = false;

	m_pPlayer->RequestShootWire(m_isShootWireRequested);
	m_isShootWireRequested = false;

	m_movementDirection = SimpleMath::Vector3::Zero;
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PlayerController::Draw()
{

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PlayerController::Finalize()
{

}

bool PlayerController::TryStep(bool isJumping)
{
	if (m_isSteppingRequest	&& !isJumping)
	{
		m_isSteppingRequest = false;
		return m_pPlayer->RequestStep();
	}
	return false;
}

bool PlayerController::TryWalk()
{
	// 移動方向

	// 入力がないときは移動しない
	if (m_movementDirection.LengthSquared() > 0.0f)
	{
		m_movementDirection.Normalize();

		// カメラの「正面方向（XZ平面）」を取得
		SimpleMath::Vector3 cameraForward = m_pCamera->GetTarget() - m_pCamera->GetEye();
		cameraForward.y = 0.0f;
		cameraForward.Normalize();

		// カメラの「右方向」を取得（カメラの向き × Y軸）
		SimpleMath::Vector3 cameraRight = cameraForward.Cross(SimpleMath::Vector3::Up);
		cameraRight.Normalize();

		// ローカル入力をカメラ空間に変換（X:右、Z:前）
		// Z方向が正面同士の時に-の乗算になり結果敵に反対方向に向いてしまうため-で補正
		SimpleMath::Vector3 worldMovementDir = -m_movementDirection.z * cameraForward + m_movementDirection.x * cameraRight;

		worldMovementDir.Normalize();

		// 移動方向の設定
		return m_pPlayer->RequestedMovement(worldMovementDir);
	}

	return false;
}

bool PlayerController::TryJumping()
{
	// ジャンプ
	if (m_isJumpingRequest)
	{
		m_isJumpingRequest = false;
		return m_pPlayer->RequestJump();
	}

	return false;
}

/**
 * @brief 入力のコールバックの紐づけを登録
 */
void PlayerController::RegisterBindCallbackToInput()
{
	using namespace SimpleMath;

	// 入力管理の取得
	// アクションマップの取得
	auto playerActionMap = m_pInputManager->GetInputActionMap(InputActionID::Player::MAP_NAME);
	// 上入力
	playerActionMap->BindInputEvent(InputActionID::Player::FRONT_MOVE, this,
		[this](const InputEventData& data) { UNREFERENCED_PARAMETER(data);  AddMoveDirection(Vector3::Forward); });
	// 下入力
	playerActionMap->BindInputEvent(InputActionID::Player::BACK_MOVE, this,
		[this](const InputEventData& data) { UNREFERENCED_PARAMETER(data);  AddMoveDirection(Vector3::Backward); });
	// 左入力
	playerActionMap->BindInputEvent(InputActionID::Player::LEFT_MOVE, this,
		[this](const InputEventData& data) { UNREFERENCED_PARAMETER(data); AddMoveDirection(Vector3::Left); });
	// 右入力
	playerActionMap->BindInputEvent(InputActionID::Player::RIGHT_MOVE, this,
		[this](const InputEventData& data) { UNREFERENCED_PARAMETER(data);  AddMoveDirection(Vector3::Right); });
	// ステップ入力
	playerActionMap->BindInputEvent(InputActionID::Player::STEPPING, this,
		[this](const InputEventData& data) { if (data.inputOption.pressed) m_isSteppingRequest = true; });
	// ジャンプ入力
	playerActionMap->BindInputEvent(InputActionID::Player::JUMPING, this,
		[this](const InputEventData& data) { if (data.inputOption.pressed) m_isJumpingRequest = true; });
	// ワイヤーを離す入力
	playerActionMap->BindInputEvent(InputActionID::Player::RELEASE_WIRE, this,
		[this](const InputEventData& data) { if (data.inputOption.released) m_isReleaseWireRequested = true; });	
	// ワイヤーを発射入力
	playerActionMap->BindInputEvent(InputActionID::Player::WIRE_SHOOTING, this,
		[this](const InputEventData& data) { if (!data.inputOption.released) m_isShootWireRequested = true; });


}


/**
 * @brief 入力のコールバックの紐づけを解除
 */
void PlayerController::UnBindCallbackToInput()
{
	// 入力管理の取得
	// アクションマップの取得
	auto playerActionMap = m_pInputManager->GetInputActionMap(InputActionID::Player::MAP_NAME);
	playerActionMap->UnBindAllInputEvent(InputActionID::Player::FRONT_MOVE, this);
	playerActionMap->UnBindAllInputEvent(InputActionID::Player::BACK_MOVE,	this);
	playerActionMap->UnBindAllInputEvent(InputActionID::Player::LEFT_MOVE,	this);
	playerActionMap->UnBindAllInputEvent(InputActionID::Player::RIGHT_MOVE, this);
	playerActionMap->UnBindAllInputEvent(InputActionID::Player::STEPPING,	this);
	playerActionMap->UnBindAllInputEvent(InputActionID::Player::JUMPING,	this);
	playerActionMap->UnBindAllInputEvent(InputActionID::Player::RELEASE_WIRE,this);
}



/**
 * @brief 移動方向に加算する
 * 
 * @param[in] addDirection 加算方向
 */
void PlayerController::AddMoveDirection(const DirectX::SimpleMath::Vector3& addDirection)
{
	m_movementDirection += addDirection;
}
