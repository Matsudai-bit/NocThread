/*****************************************************************//**
 * @file    GameEffectManager.h
 * @brief   ゲームエフェクト管理に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/11
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <unordered_map>
#include <vector>

#include "Game/Common/GameEffect/Base/IGameEffect.h"
#include "Game/Common/TaskManager/TaskManager.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief ゲームエフェクト管理
 */
class GameEffectManager
	: public Task
{
/// 構造体の宣言 ----------------------------------------------------
public:

	/**
	 * @brief エフェクトクリップ
	 */
	struct EffectClip
	{
		bool isLoop; // ループさせるかどうか

		EffectClip(bool loop = false)
			: isLoop{ loop }
		{
		}
	};

private:

	/**
	 * @brief エフェクトと操作用
	 */
	struct EffectHandle
	{
		EffectClip clip;
		std::unique_ptr<IGameEffect> effect;
		bool isEnd = false; // 終了フラグ
	};

	/**
	 * @brief 識別番号の生成器
	 */
	class IdGenerator
	{
	public:
		IdGenerator();

	private:
		int m_nextID;
		std::vector<int> m_freeIDs;

	public:
		unsigned int operator() ()
		{
			// 再利用可能なIDプールをチェック
			if (!m_freeIDs.empty())
			{
				unsigned int reusedID = m_freeIDs.back();
				m_freeIDs.pop_back();
				return reusedID;
			}

			// プールが空であれば、新しい連番IDを発行
			if (m_nextID == 0) 
			{
				m_nextID++; // 1から開始
			}
			int newID = m_nextID;

			m_nextID++;

			// プールに追加
			m_freeIDs.emplace_back(newID);

			return newID;
		}

		// ID を返却する
		void RecycleID(unsigned int id);

	
	};

// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::unordered_map<int, EffectHandle> m_effectHandles;	///< エフェクト群

	IdGenerator m_idGenerator; ///< 識別番号生成器

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GameEffectManager();
	// デストラクタ
	~GameEffectManager();


// 操作
public:

	// エフェクトの再生
	int PlayEffect(std::unique_ptr<IGameEffect> effect, EffectClip clip);
	// エフェクトの停止
	void RequestEffectStop(unsigned int id);

	// 終了処理
	void Finalize();

	// タスクの更新処理
	bool UpdateTask(float deltaTime) override;

	// 描画処理
	void DrawTask(const Camera& camera) override;

private:
	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(const Camera& camera);

	// 
	void EraseEffectToMap(unsigned int id);

	// 
	bool CanEraseEffect(unsigned int id, bool isEnd, EffectClip clip);


// 取得/設定
public:


// 内部実装
private:


};
