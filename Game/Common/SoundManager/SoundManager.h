/*****************************************************************//**
 * @file    SoundManager.h
 * @brief   音管理に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <unordered_map>
#include <random>

// クラスの前方宣言 ===================================================
class ResourceManager;

// クラスの定義 ===============================================================
/**
 * @brief 音管理
 */
class SoundManager final
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	static std::unique_ptr<SoundManager> s_instance; ///< インスタンス

	ResourceManager* m_pResourceManager;

	std::unordered_map<int, std::unique_ptr<DirectX::SoundEffectInstance>> m_sounds; ///< 登録されているサウンドとIDのペア
	std::mt19937 m_idGenerator;										///< 識別子生成器
	

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
private:
	// コンストラクタ
	SoundManager();
public:
	// デストラクタ
	~SoundManager() ;
public:


// 操作
public:

	// 再生
	int Play(const std::string& filePath, bool isLoop = false, float volume = 0.3f);
	// 既存のIDで再生する場合
	int Play(int id,  bool isLoop, float volume);

	// 削除
	bool Remove(int id);
	// 全削除
	bool RemoveAll();

// 取得/設定
public:

	// インスタンスの取得
	static SoundManager* GetInstance();

	void SetResourceManager(ResourceManager* pResourceManager);

// 内部実装
private:


};
