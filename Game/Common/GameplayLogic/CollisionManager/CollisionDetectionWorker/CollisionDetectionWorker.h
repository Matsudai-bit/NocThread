/*****************************************************************//**
 * @file    CollisionDetectionWorker.h
 * @brief   衝突検知作業員クラスに関するヘッダーファイル
 * 
 *　スレッドを用いて衝突検知を行うクラス
 *
 * @author  松下大暉
 * @date    2026/02/21
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <thread>
#include "Game/Common/GameplayLogic/CollisionManager/CollisionManager.h"


// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 衝突検知作業員
 */
class CollisionDetectionWorker
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr< std::thread>		m_workerThread;		///< 作業スレッド
	std::vector<DetectedCollisonData>	m_detectionResults;	///< 衝突結果

	std::mutex m_mutex;

	std::atomic<bool>	m_isCalculating = true;	///< 計算中かどうか
	std::atomic<bool>	m_stopThread = false;	///< スレッドの停止

	std::atomic<bool> m_isReady;			///< スレッドの適用

	std::condition_variable m_cv;

	std::unique_ptr<std::vector<ThreadCollisionObjectProxy>> m_proxies;

	std::unique_ptr<CollisionMatrix> m_collisionMatrix;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CollisionDetectionWorker();

	// デストラクタ
	~CollisionDetectionWorker();


// 操作
public:
	void StartAsync(std::unique_ptr<std::vector<ThreadCollisionObjectProxy>> proxies);
	// 終了まで待機
	void WaitForEndCalculation();



// 取得/設定
public:
	// 衝突検知結果の取得
	std::vector<DetectedCollisonData> GetDetectionResults();


// 内部実装
private:

	void DetectionThreadLoop();

	void CheckDetectionPair(const ThreadCollisionObjectProxy& collisionDataA, const ThreadCollisionObjectProxy& collisionDataB, std::vector<DetectedCollisonData>* pOutResults);

	// 検知できるかどうか
	bool CanDetect(const ThreadCollisionObjectProxy& collisionDataA, const ThreadCollisionObjectProxy& collisionDataB);

};
