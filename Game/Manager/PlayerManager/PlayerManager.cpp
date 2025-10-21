/*****************************************************************//**
 * @file    PlayerManager.h
 * @brief   プレイヤー管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/20
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PlayerManager.h"

#include "Game/GameObjects/Player/Player.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/GameObjects/Player/PlayerController/PlayerController.h"
#include "Game/Common/Camera/PlayerCamera/PlayerCamera.h"

#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PlayerManager::PlayerManager()
	: m_pCommonResources{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
PlayerManager::~PlayerManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] pCommonResources	共通リソース
 * @param[in] pCollisionManager	衝突管理
 * @param[in] pPlayerCamera		プレイヤーカメラ
 */
void PlayerManager::Initialize(CommonResources* pCommonResources, CollisionManager* pCollisionManager, const PlayerCamera* pPlayerCamera)
{
	m_pCommonResources = pCommonResources;

	// プレイヤーインプットの作成
	m_playerInput = InputBindingFactory::CreatePlayerInput();

	// プレイヤーの作成
	m_player = std::make_unique<Player>();
	m_player->Initialize(pCommonResources, pCollisionManager, pPlayerCamera, m_playerInput.get());

	// プレイヤーの影の作成
	m_playerShadow = std::make_unique<CircularShadow>();
	m_playerShadow->Initialize(m_pCommonResources->GetDeviceResources(), 0.01f);

	// プレイヤーコントローラの作成
	m_playerController = std::make_unique<PlayerController>(m_player.get(), pPlayerCamera);
	m_playerController->Initialize(m_player.get(), pPlayerCamera);
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void PlayerManager::Update(float deltaTime, const DirectX::SimpleMath::Matrix& proj)
{
	// プレイヤーの入力
	m_playerInput->Update(m_pCommonResources->GetKeyboardTracker(), m_pCommonResources->GetMouseTracker(), m_pCommonResources->GetGamePadTracker());

	// プレイヤーの更新処理
	m_player->Update(deltaTime, proj);

	m_playerController->Update(deltaTime, m_pCommonResources->GetKeyboardTracker(), m_pCommonResources->GetMouseTracker(), m_pCommonResources->GetGamePadTracker());

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PlayerManager::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	// プレイヤーの描画
	m_player->Draw(view,projection);

	// プレイヤーの影
	m_playerShadow->Draw(view, projection, m_pCommonResources->GetCommonStates(), m_player->GetPosition());

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PlayerManager::Finalize()
{

}

/**
 * @brief プレイヤーの取得
 * 
 * @return プレイヤー
 */
const Player* PlayerManager::GetPlayer() const
{
	return m_player.get();
}
