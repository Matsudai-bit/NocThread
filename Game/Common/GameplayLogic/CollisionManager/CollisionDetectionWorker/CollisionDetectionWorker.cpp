/*****************************************************************//**
 * @file    CollisionDetectionWorker.cpp
 * @brief   衝突検知作業員クラスに関するソースファイル
 *
 *　スレッドを用いて衝突検知を行う
 *
 * @author  松下大暉
 * @date    2026/02/21
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CollisionDetectionWorker.h"
#include "Game/Common/GameplayLogic/CollisionManager/CollisionManager.h"
#include "Game/Common/GameplayLogic/CollisionMatrix/CollisionMatrix.h"


// ライブラリ

// ファクトリー関連

// データベース関連

// フレームワーク関連

// ゲームプレイロジック関連

// グラフィック関連


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CollisionDetectionWorker::CollisionDetectionWorker()
	: m_detectionResults{}
	, m_isCalculating{ false }
	, m_isReady{ false }
{
	m_collisionMatrix = std::make_unique<CollisionMatrix>();
}



/**
 * @brief デストラクタ
 */
CollisionDetectionWorker::~CollisionDetectionWorker()
{
	if (m_workerThread)
	{
		m_stopThread = true;
		m_cv.notify_one();
		m_workerThread->join();
	}
}

void CollisionDetectionWorker::StartAsync(std::unique_ptr<std::vector<ThreadCollisionObjectProxy>> proxies)
{
	if (!m_workerThread)
	{
		m_workerThread = std::make_unique<std::thread>(&CollisionDetectionWorker::DetectionThreadLoop, this);
	}

	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_detectionResults.clear();
		m_proxies = std::move(proxies);
		m_isReady = true;       // 仕事の準備完了
		m_isCalculating = true; // 計算中にする
	}
	m_cv.notify_one(); // 待機中のスレッドを叩き起こす
}

void CollisionDetectionWorker::WaitForEndCalculation()
{
	std::unique_lock<std::mutex> lock(m_mutex);

	if (m_workerThread)
	{
		m_cv.wait(lock, [this] { return !m_isCalculating; });

	}
}

std::vector<DetectedCollisonData> CollisionDetectionWorker::GetDetectionResults()
{
	std::unique_lock<std::mutex> lock(m_mutex);

	return m_detectionResults;
}

void CollisionDetectionWorker::SetCollisionMatrix(const CollisionMatrix& matrix)
{
	*m_collisionMatrix = matrix;
}

void CollisionDetectionWorker::DetectionThreadLoop()
{
	// サブスレッド内だけで使う「完全なローカル」スタック
	std::vector<DetectedCollisonData> localBuffer;
	localBuffer.reserve(256); // あらかじめある程度確保しておく
	while (!m_stopThread)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			// 「仕事が来る(m_isReady)」か「終了する(m_stopThread)」までスリープ
			m_cv.wait(lock, [this] { return m_isReady || m_stopThread; });

#ifdef COLLISIONMANAGER_DEBUG	
			OutputDebugString(L"============ 仕事を受け取った ============\n");
#endif 

			if (m_stopThread) break;

			// 判定に使うデータをローカルにコピー（メインスレッドとの干渉を防ぐ）
			m_isCalculating = true;
			m_isReady = false; // 受付完了
		}

		if (m_proxies->size() >= 2)
		{
			for (size_t i = 0; i < m_proxies->size() - 1; i++)
			{
				for (size_t j = i + 1; j < m_proxies->size(); j++)
				{
					// 1. ロックを一切気にせず、ローカルバッファにどんどん溜める
					CheckDetectionPair((*m_proxies)[i], (*m_proxies)[j], &m_detectionResults);

						m_cv.notify_one();
					
				}
			}
		}

		

		m_isCalculating = false;

#ifdef COLLISIONMANAGER_DEBUG	
		OutputDebugString(L"!!!! 衝突処理の終了 !!!! \n");
#endif 
		m_cv.notify_one(); // 完了通知
	}
}

/**
 * @brief ペアの衝突チェック
 *
 * @param[in] collisionDataA	衝突データA
 * @param[in] collisionDataB	衝突データB
 *
 * @param[out] pOutResults 検知された衝突を格納する
 */
void CollisionDetectionWorker::CheckDetectionPair(
    const ThreadCollisionObjectProxy& collisionDataA, 
    const ThreadCollisionObjectProxy& collisionDataB, 
	std::vector<DetectedCollisonData>* pOutResultStack)
{
	if (collisionDataA.id == collisionDataB.id) return;


	// 活動していなければ飛ばす
	if (collisionDataA.isActive == false) return;
	//活動していなければ飛ばす
	if (collisionDataB.isActive == false) return;



	// 衝突検知をするかどうか
	if (!CanDetect(collisionDataA, collisionDataB))
	{
		return;
	}

	// 衝突しているかどうか
	if (CollisionManager::DetectCollision(collisionDataA.collider.get(), collisionDataB.collider.get()))
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			pOutResultStack->push_back({ collisionDataA.id, collisionDataB.id });
		}

		// 子を持っている場合その子も検知する
		for (auto& child : collisionDataA.children)
		{
			CheckDetectionPair(child, collisionDataB, pOutResultStack);
		}
		for (auto& child : collisionDataB.children)
		{
			CheckDetectionPair(child, collisionDataA, pOutResultStack);
		}
	}
}
/**
 * @brief 衝突検知できるかどうか
 *
 * @param[in] collisionDataA　衝突データA
 * @param[in] collisionDataB　衝突データB
 *
 * @returns true 可能
 * @returns false 不可能
 */
bool CollisionDetectionWorker::CanDetect(
    const ThreadCollisionObjectProxy& collisionDataA, 
    const ThreadCollisionObjectProxy& collisionDataB)
{
    // ゲームオブジェクトタグのビットインデックス
    uint32_t gameObjectTagIndexA = collisionDataA.tagBitIndex;
    // 衝突検知をするかどうか
    return m_collisionMatrix && m_collisionMatrix->ShouldCollide(gameObjectTagIndexA, collisionDataB.tag);
}
