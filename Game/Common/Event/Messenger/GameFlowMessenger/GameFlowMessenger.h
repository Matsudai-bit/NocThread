/*****************************************************************//**
 * @file    GameFlowMessenger.h
 * @brief   ゲーム進行のイベントのメッセージ送出クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>

#include "Game/Common/Event/Messenger/GameFlowMessenger/GameFlowEventID.h"

// クラスの前方宣言 ===================================================
class IGameFlowObserver;


// クラスの定義 ===============================================================
/**
 * @brief ゲーム進行のイベントのメッセージ送出クラス (シングルトン)
 */
class GameFlowMessenger final
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	
	// インスタンス
	static std::unique_ptr<GameFlowMessenger> s_instance;

	std::vector<IGameFlowObserver*> m_observers; ///< 監視者


// メンバ関数の宣言 -------------------------------------------------
private:
	// コンストラクタ
	GameFlowMessenger() = default;
public :
	// デストラクタ
	~GameFlowMessenger() = default;


// 操作
public:

	// 監視者の登録
	void RegistrObserver(IGameFlowObserver* observer);

	// 通知
	void Notify(GameFlowEventID eventID);

	// 全て削除
	void RemoveAllObserver();

// 取得/設定
public:

	// インスタンスの取得
	static  GameFlowMessenger* GetInstance();



// 内部実装
private:


};
