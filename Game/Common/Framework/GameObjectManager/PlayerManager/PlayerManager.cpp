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

// ファクトリー関連
#include "Game/Common/Factory/InputBindingFactory/InputBindingFactory.h"

// フレームワーク関連
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"
#include "Game/Common/Framework/CommonResources/CommonResources.h"

// グラフィック関連
#include "Game/Common/Graphics/Camera/PlayerCamera/PlayerCamera.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Player/Player.h"
#include "Game/GameObjects/Player/PlayerController/PlayerController.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PlayerManager::PlayerManager()
	: m_pCommonResources{ nullptr }
	, m_isStoppingUpdate{ false }
	, m_pPlayerCamera	{ nullptr }
{
	GameFlowMessenger::GetInstance()->RegistryObserver(this);
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
void PlayerManager::Initialize(const CommonResources* pCommonResources, const PlayerCamera* pPlayerCamera)
{
	m_pCommonResources = pCommonResources;

	// プレイヤーインプットの作成
	m_playerInput = InputBindingFactory::PlayerInputFactory().Create(DefaultSpawnDesc());

	// プレイヤーの影の作成
	m_playerShadow = std::make_unique<CircularShadow>();
	m_playerShadow->Initialize(m_pCommonResources->GetDeviceResources(), 0.01f);

	// プレイヤーコントローラの作成
	m_playerController = std::make_unique<PlayerController>();

	// プレイヤーカメラの取得
	m_pPlayerCamera = pPlayerCamera;
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool PlayerManager::UpdateTask(float deltaTime)
{
	if (m_isStoppingUpdate || m_player.get() == nullptr) { return true; }

	// プレイヤーの入力
	m_playerInput->Update(m_pCommonResources->GetKeyboardTracker(), m_pCommonResources->GetMouseTracker(), m_pCommonResources->GetGamePadTracker());

	// プレイヤーの更新処理
	m_player->Update(deltaTime);

	m_playerController->Update(deltaTime,m_player.get(), m_pCommonResources->GetKeyboardTracker(), m_pCommonResources->GetMouseTracker(), m_pCommonResources->GetGamePadTracker());

	return true;
}



/**
 * @brief 描画処理
 *
 * @param[in] camera カメラ
 *
 * @return なし
 */
void PlayerManager::DrawTask(const Camera& camera)
{
	// プレイヤーの描画
	m_player->Draw(camera);

	// プレイヤーの影
	m_playerShadow->Draw(camera, m_pCommonResources->GetCommonStates(), m_player->GetTransform()->GetPosition());

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

void PlayerManager::OnGameFlowEvent(GameFlowEventID eventID)
{
	switch (eventID)
	{
	case GameFlowEventID::GAME_TRANSITION_FADING_START:
		// プレイヤーのアクティブ化
		m_isStoppingUpdate = true;
		break;
	case GameFlowEventID::GAME_START:
		// プレイヤーのアクティブ化
		m_isStoppingUpdate = false;

		// プレイヤーコントローラの初期化処理
		m_playerController->Initialize(m_pPlayerCamera, m_player.get());
		m_player->RequestStep();

		break;
	}
}

/**
 * @brief プレイヤーの設定
 * 
 * @param[in] player
 */
void PlayerManager::SetPlayer(std::unique_ptr<Player> player)
{
	m_player = std::move(player);
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
