/*****************************************************************//**
 * @file    SoundManager.h
 * @brief   サウンド管理に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SoundManager.h"

#include "Game/Common/ResourceManager/ResourceManager.h"

using namespace DirectX;

std::unique_ptr<SoundManager> SoundManager::s_instance = nullptr;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
SoundManager::SoundManager()
	: m_pResourceManager{ nullptr }
{
	std::random_device seedGenerator;
	m_idGenerator.seed(seedGenerator());
}

/**
 * @brief デストラクタ
 */
SoundManager::~SoundManager()
{
	m_sounds.clear();
}


/**
 * @brief 再生
 * 
 * @param[in] filePath　ファイルパス
 * @param[in] isLoop	ループするかどうか
 * 
 * @returns -1	エラー
 * @returns 0> 識別子 */
int SoundManager::Play(const std::string& filePath, bool isLoop, float volume)
{
	std::unique_ptr<DirectX::SoundEffectInstance> soundInstance = m_pResourceManager->CreateSoundEffect(filePath)->CreateInstance();
	soundInstance->Play(isLoop);

	int newId = m_idGenerator();

	// 存在しないキーになるまで続ける
	while (m_sounds.find(newId) != m_sounds.end())
	{
		newId = m_idGenerator();
	}

	// 登録する
	m_sounds.insert({ newId, std::move(soundInstance) });

	return Play(newId, isLoop, volume);
}

/**
 * @brief 指定した音を鳴らす
 * 
 * @param[in] id　		識別子
 * @param[in] isLoop	ループさせるかどうか
 * 
 * @returns -1	エラー
 * @returns 0> 識別子
 * 
 */
int SoundManager::Play(int id, bool isLoop, float volume)
{
	if (m_sounds.find(id) == m_sounds.end()){return -1;}

	m_sounds[id]->Play(isLoop);

	m_sounds[id]->SetVolume(volume);

	return id;
}

/**
 * @brief 削除
 * 
 * @param[in] id　識別子
 * 
 * @returns true 成功
 * @returns false 失敗
 */
bool SoundManager::Remove(int id)
{
	if (m_sounds.find(id) == m_sounds.end()) { return false; }

	m_sounds[id]->Stop(true);

	m_sounds.erase(id);

	return true;
}
/**
 * @brief 削除
 *
 * @returns true 成功
 * @returns false 失敗
 */
bool SoundManager::RemoveAll()
{
	for (auto& pair : m_sounds)
	{
		pair.second->Stop(true);
	}

	m_sounds.clear();
	return true;
}

/**
 * @brief インスタンスの取得
 * 
 * @return インスタンスのポインタ
 */
SoundManager* SoundManager::GetInstance()
{
	if (s_instance.get() == nullptr)
	{
		s_instance.reset(new SoundManager());
	}

	return s_instance.get();
}

/**
 * @brief リソース管理の設定
 * 
 * @param[in] pResourceManager　リソース管理
 */
void SoundManager::SetResourceManager(ResourceManager* pResourceManager)
{
	m_pResourceManager = pResourceManager;
}


