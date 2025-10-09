/*****************************************************************//**
 * @file    GameObjectRegistry.h
 * @brief   ゲームオブジェクト登録帳クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>
#include "Game/GameObjects/Common/GameObject.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief ゲームオブジェクト登録帳
 */
class GameObjectRegistry final
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	// インスタンス
	static std::unique_ptr<GameObjectRegistry> s_instance; 

	// ゲームオブジェクト
	std::vector<const GameObject*>  m_gameObjects;

// メンバ関数の宣言 -------------------------------------------------

private:
	// コンストラクタ
	GameObjectRegistry() = default;

public:
	// デストラクタ
	~GameObjectRegistry() = default;


// 操作
public:

	// 登録
	void Registr(const GameObject* pGameObject);

	// ゲームオブジェクトを取得する
	const GameObject* GetGameObject(GameObjectTag tag);

	// ゲームオブジェクトを取得する
	std::vector<const GameObject*> GetGameObjects(GameObjectTag tag);

	// クリア
	void Clear();
	
// 取得/設定
public:
	// インスタンスの登録
	static GameObjectRegistry* GetInstance() ;

// 内部実装
private:


};
