/*****************************************************************//**
 * @file    GameEffectManager.h
 * @brief   ゲームエフェクト管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/11
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GameEffectManager.h"
#include "Game/Common/Camera/Camera.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GameEffectManager::GameEffectManager()
{

}



/**
 * @brief デストラクタ
 */
GameEffectManager::~GameEffectManager()
{

}



/**
 * @brief エフェクトの再生
 * 
 * @param[in] effect　	エフェクト
 * @param[in] clip		クリップ
 * 
 * @returns 1以上 識別番号
 * @returns 0未満 エラー
 */
int GameEffectManager::PlayEffect(std::unique_ptr<IGameEffect> effect, EffectClip clip)
{
	if (effect.get() == nullptr) { return -1; };

	// 識別番号の生成
	unsigned int newID = m_idGenerator();

	// マップに追加
	m_effectHandles[newID].clip = clip;
	m_effectHandles[newID].effect = std::move(effect);
	m_effectHandles[newID].isEnd = false;

	// 再生
	m_effectHandles[newID].effect->Play();

	return newID;
}

/**
 * @brief エフェクトの停止
 * 
 * @param[in] id 識別番号
 */
void GameEffectManager::RequestEffectStop(unsigned int id)
{
	// 存在するかどうか
	if (m_effectHandles.count(id) > 0)
	{
		// 再生終了フラグを立てる
		m_effectHandles[id].isEnd = true;
	}

}


/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void GameEffectManager::Update(float deltaTime)
{
	for (auto it = m_effectHandles.begin(); it != m_effectHandles.end();)
	{
		auto& effectHandle = *it;
		std::unique_ptr<IGameEffect>& effect = effectHandle.second.effect;
		EffectClip					  clip = effectHandle.second.clip;
		bool						isEnd = effectHandle.second.isEnd;

		// 再生中の場合
		if (effect->IsPlaying())
		{
			// 更新処理
			effect->Update(deltaTime, isEnd);

		}

		// 再生が終了している場合
		else
		{
			// 再生が終了している場合 
			if (isEnd || !clip.isLoop)
			{
				// 再生を停止する
				// 識別番号の返却
				m_idGenerator.RecycleID(it->first);
				// 削除
				it = m_effectHandles.erase(it);
				
				continue;
			}

			// ループする場合
			else
			{
				effect->Play();
			}
		}
		++it;
		
	}

}



/**
 * @brief 描画処理
 *
 * @param[in] view　ビュー行列
 * @param[in] proj  射影行列
 */
void GameEffectManager::Draw(const Camera& camera)
{
	for (auto& effectHandle : m_effectHandles)
	{
		std::unique_ptr<IGameEffect>& effect = effectHandle.second.effect;

		// 描画処理
		effect->Draw(camera);
	}
}
/**
 * @brief マップからエフェクトを削除する
 *
 * @param[in] id　識別番号
 */
void GameEffectManager::EraseEffectToMap(unsigned int id)
{
	// 存在するかどうか
	if (m_effectHandles.count(id) > 0)
	{
		// 削除
		m_effectHandles.erase(id);
		// 識別番号の返却
		m_idGenerator.RecycleID(id);
	}
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameEffectManager::Finalize()
{

}

/**
 * @brief タスクの更新処理
 * 
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @returns true タスクを継続する
 * @returns false タスクを削除する
 */
bool GameEffectManager::UpdateTask(float deltaTime)
{
	// 更新処理
	Update(deltaTime);

	return true;
}

/**
 * @brief 描画処理
 * 
 * @param[in] camera
 */
void GameEffectManager::DrawTask(const Camera& camera)
{
	// 描画処理
	Draw(camera);
}


/**
 * @brief コンストラクタ
 * 
 */
GameEffectManager::IdGenerator::IdGenerator()
{
	m_nextID = 1;

	m_freeIDs.clear();
}

/**
 * @brief IDを返却する
 * 
 * @param[in] id　識別番号
 */
void GameEffectManager::IdGenerator::RecycleID(unsigned int id)
{
	if (id > 0)
	{
		// 破棄されたIDをキューの末尾に追加
		m_freeIDs.push_back(id);
	}
}



