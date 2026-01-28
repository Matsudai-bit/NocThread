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
	: m_pCamera{ nullptr }
	, m_playerInput{ nullptr }
{
}



/**
 * @brief デストラクタ
 */
PlayerController::~PlayerController()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PlayerController::Initialize(const Camera* pCamera, Player* pPlayer)
{
	m_pCamera = pCamera;

	// プレイヤー入力の作成
	m_playerInput = InputBindingFactory::PlayerInputFactory().Create(DefaultSpawnDesc());

	m_pPlayer = pPlayer;
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void PlayerController::Update(
	float deltaTime,
	Player* pPlayer,
	const Keyboard::KeyboardStateTracker* pKeyboardStateTracker,
	const Mouse::ButtonStateTracker* pMouseStateTracker, 
	const DirectX::GamePad::ButtonStateTracker* pGamePadStateTracker)
{
	UNREFERENCED_PARAMETER(deltaTime);

	m_playerInput->Update(pKeyboardStateTracker, pMouseStateTracker, pGamePadStateTracker);

	TryWalk();
	
	bool isJumpSuccess = false;
	// ジャンプ
	isJumpSuccess = TryJumping();
	
	// ステップ
	TryStep(isJumpSuccess);

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
	if (m_playerInput->IsInput(InputActionType::PlyayerActionID::STEPPING, InputSystem<InputActionType::PlyayerActionID>::InputOption::PRESSED)
		&& !isJumping)
	{
		return m_pPlayer->RequestStep();
	}
	return false;
}

bool PlayerController::TryWalk()
{
	// 移動方向
	SimpleMath::Vector3 movementDirection = SimpleMath::Vector3::Zero;

	// 奥へ
	if (m_playerInput->IsInput(InputActionType::PlyayerActionID::FRONT_MOVE)) { movementDirection += SimpleMath::Vector3::Forward; }
	// 手前へ
	if (m_playerInput->IsInput(InputActionType::PlyayerActionID::BACK_MOVE)) { movementDirection += SimpleMath::Vector3::Backward; }
	// 右へ
	if (m_playerInput->IsInput(InputActionType::PlyayerActionID::RIGHT_MOVE)) { movementDirection += SimpleMath::Vector3::Right; }
	// 左へ
	if (m_playerInput->IsInput(InputActionType::PlyayerActionID::LEFT_MOVE)) { movementDirection += SimpleMath::Vector3::Left; }

	// 入力がないときは移動しない
	if (movementDirection.LengthSquared() > 0.0f)
	{
		movementDirection.Normalize();

		// カメラの「正面方向（XZ平面）」を取得
		SimpleMath::Vector3 cameraForward = m_pCamera->GetTarget() - m_pCamera->GetEye();
		cameraForward.y = 0.0f;
		cameraForward.Normalize();

		// カメラの「右方向」を取得（カメラの向き × Y軸）
		SimpleMath::Vector3 cameraRight = cameraForward.Cross(SimpleMath::Vector3::Up);
		cameraRight.Normalize();

		// ローカル入力をカメラ空間に変換（X:右、Z:前）
		// Z方向が正面同士の時に-の乗算になり結果敵に反対方向に向いてしまうため-で補正
		SimpleMath::Vector3 worldMovementDir = -movementDirection.z * cameraForward + movementDirection.x * cameraRight;

		worldMovementDir.Normalize();

		// 移動方向の設定
		return m_pPlayer->RequestedMovement(worldMovementDir);
	}

	return false;
}

bool PlayerController::TryJumping()
{
	// ジャンプ
	if (m_playerInput->IsInput(InputActionType::PlyayerActionID::JUMPING, InputSystem<InputActionType::PlyayerActionID>::InputOption::PRESSED))
	{
		return m_pPlayer->RequestJump();
	}

	return false;
}
