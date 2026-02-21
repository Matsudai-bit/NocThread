/*****************************************************************//**
 * @file    EnemyManager.h
 * @brief   敵管理クラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EnemyManager.h"


// フレームワーク関連
#include "Game/Common/Framework/Event/Messenger/GameFlowMessenger/GameFlowMessenger.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Enemy/IEnemy.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
EnemyManager::EnemyManager()
{
	GameFlowMessenger::GetInstance()->RegistryObserver(this);
}



/**
 * @brief デストラクタ
 */
EnemyManager::~EnemyManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EnemyManager::Initialize()
{

}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool EnemyManager::UpdateTask(float deltaTime)
{
	for (auto& enemy : m_enemies)
	{
		enemy->Update(deltaTime);
	}

	return true;
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EnemyManager::DrawTask(const Camera& camera)
{
	if (m_isStoppingUpdate || m_enemies.empty()) { return; }
	for (auto& enemy : m_enemies)
	{
		enemy->Draw(camera);
	}
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EnemyManager::Finalize()
{

}

/**
 * @brief 管理する敵の追加
 * 
 * @param[in] enemy
 */
void EnemyManager::AddEnemy(IEnemy* enemy)
{
	m_enemies.push_back(enemy);
}

/**
 * @brief ゲームフローイベントの受信
 * 
 * @param[in] eventID
 */
void EnemyManager::OnGameFlowEvent(GameFlowEventID eventID)
{
	switch (eventID)
	{

	case GameFlowEventID::GAME_TRANSITION_FADING_START:
		m_isStoppingUpdate = true;
		break;
	case GameFlowEventID::GAME_START:
		m_isStoppingUpdate = false;
		break;

	}
}
