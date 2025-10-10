/*****************************************************************//**
 * @file    GameEffectController.h
 * @brief   ゲームエフェクトコントローラに関するヘッダーファイル
 * 
 * 登録されたゲームエフェクト管理クラスを使用してエフェクトの再生、停止をする
 *
 * @author  松下大暉
 * @date    2025/10/11
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include "Game/Common/GameEffect/GameEffectManager.h"

// クラスの前方宣言 ===================================================
class IGameEffect;
// クラスの定義 ===============================================================
/**
 * @brief ゲームエフェクトコントローラ　シングルトン
 */
class GameEffectController final
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	static std::unique_ptr<GameEffectController> s_instance; ///< インスタンス

	GameEffectManager* m_pGameEffectManager;	///< ゲームエフェクト管理

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
private:
	// コンストラクタ
	GameEffectController();

public:
	// デストラクタ
	~GameEffectController();


// 操作
public:

	// エフェクトの再生
	int PlayEffect(std::unique_ptr<IGameEffect> effect, GameEffectManager::EffectClip clip);
	// エフェクトの停止
	void StopEffect(unsigned int id);


// 取得/設定
public:

	// インスタンスの取得
	static GameEffectController* GetInstance();

	// ゲームエフェクト管理の設定
	void SetGameEffectManager(GameEffectManager* pGameEffectManager);


// 内部実装
private:


};
