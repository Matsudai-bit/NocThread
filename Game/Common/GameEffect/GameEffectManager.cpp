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

	// 再生
	m_effectHandles[newID].effect->Play();

	return newID;
}

/**
 * @brief エフェクトの停止
 * 
 * @param[in] id 識別番号
 */
void GameEffectManager::StopEffect(unsigned int id)
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
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void GameEffectManager::Update(float elapsedTime)
{
	for (auto& effectHandle : m_effectHandles)
	{
		std::unique_ptr<IGameEffect>& effect	= effectHandle.second.effect;
		EffectClip					  clip		= effectHandle.second.clip;

		// 再生中の場合
		if (effect->IsPlaying())
		{
			// 更新処理
			effect->Update(elapsedTime);
		}

		// 再生が終了している場合
		else
		{
			// ループする場合
			if (clip.isLoop)
			{
				effect->Play();
			}
			else
			{
				// 再生を停止する
				StopEffect(effectHandle.first);
			}
		}
	}
}



/**
 * @brief 描画処理
 * 
 * @param[in] view　ビュー行列
 * @param[in] proj  射影行列
 */
void GameEffectManager::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	for (auto& effectHandle : m_effectHandles)
	{
		std::unique_ptr<IGameEffect>& effect = effectHandle.second.effect;

		// 描画処理
		effect->Draw(view, proj);
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



