/*****************************************************************//**
 * @file    GameProgressDataManager.h
 * @brief   ゲーム中の進捗データ管理クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/12/17
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include <memory>

// クラスの前方宣言 ===================================================

/**
 * @brief ゲーム中の進捗データ
 */
struct GameProgressData
{
	int passedCheckpointNum;///< 通過したチェックポイント数

	float gameTime;			///< ゲーム時間
};

// クラスの定義 ===============================================================
/**
 * @brief ゲーム中の進捗データ管理
 */
class GameProgressDataManager
{
// クラス定数の宣言 -------------------------------------------------
public:

	std::unique_ptr<GameProgressData> m_gameProgressData; ///< ゲーム進捗データ

// データメンバの宣言 -----------------------------------------------
private:



// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GameProgressDataManager();

	// デストラクタ
	~GameProgressDataManager();


// 操作
public:

	// 通過したチェックポイント数を増やす
	void IncrementPassedCheckpoints();

	// データをリセットする
	void ResetData();

	// 進捗データの取得
	GameProgressData GetProgressData() const;


// 取得/設定
public:


// 内部実装
private:


};
