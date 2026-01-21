/*****************************************************************//**
 * @file    GameObjectRegistry.h
 * @brief   ゲームオブジェクト登録帳に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GameObjectRegistry.h"



// 静的変数の初期化
std::unique_ptr<GameObjectRegistry> GameObjectRegistry::s_instance = nullptr;

// メンバ関数の定義 ===========================================================

/**
 * @brief 登録
 * 
 * @param[in] pGameObject　登録するゲームオブジェクト
 */
void GameObjectRegistry::Registr(const GameObject* pGameObject)
{
	if (std::find(m_gameObjects.begin(), m_gameObjects.end(), pGameObject) == m_gameObjects.end())
	{
		m_gameObjects.push_back(pGameObject);
	}
}

/**
 * @brief ゲームオブジェクトの取得
 * 
 * @param[in] tag　取得するゲームオブジェクトのタグ
 * 
 * @return ゲームオブジェクト
 */
const GameObject* GameObjectRegistry::GetGameObject(GameObjectTag tag)
{
	auto gameObject = std::find_if( m_gameObjects.begin(), m_gameObjects.end(),
		[&](const GameObject* gameObject) 
		{
			if (gameObject)
			{
				return gameObject->GetTag() == tag; 
			}
			return false;
		}
	);

	if (gameObject == m_gameObjects.end())
	{
		return nullptr;
	}
	
	return *gameObject;
}

/**
 * @brief ゲームオブジェクトを取得する
 * 
 * @param[in] tag　取得するゲームオブジェクトのタグ
 *
 * @return ゲームオブジェクト
 */
std::vector<const GameObject*> GameObjectRegistry::GetGameObjects(GameObjectTag tag)
{

	std::vector<const GameObject*> result;
	std::copy_if(m_gameObjects.begin(), m_gameObjects.end(), std::back_inserter(result),
		[tag](const GameObject* gameObject)
		{
			return gameObject->GetTag() == tag;
		});



	return result;
}

/**
 * @brief クリア
 * 
 */
void GameObjectRegistry::Clear()
{
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i] = nullptr;
	}
	m_gameObjects.clear();

}

/**
 * @brief インスタンスの取得
 * 
 * @return インスタンス
 */
GameObjectRegistry* GameObjectRegistry::GetInstance()
{
	if (s_instance.get() == nullptr)
	{
		s_instance.reset(new GameObjectRegistry());
	}

	return s_instance.get();
}
