/*****************************************************************//**
 * @file    GameProgressDataManager.h
 * @brief   ゲーム中の進捗データ管理クラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/12/17
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GameProgressData.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GameProgressDataManager::GameProgressDataManager()
{

}



/**
 * @brief デストラクタ
 */
GameProgressDataManager::~GameProgressDataManager()
{

}

/**
 * @brief 通過したチェックポイント数を増やす
 */
void GameProgressDataManager::IncrementPassedCheckpoints()
{
	m_gameProgressData->passedCheckpointNum++;
}

/**
 * @brief データをリセットする
 */
void GameProgressDataManager::ResetData()
{
	m_gameProgressData.reset(new GameProgressData());
}

/**
 * @brief 進捗データの取得
 * 
 * @return 進捗データ
 */
GameProgressData GameProgressDataManager::GetProgressData() const
{
	return *m_gameProgressData;
}
