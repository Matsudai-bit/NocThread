/*****************************************************************//**
 * @file    EnemyManager.h
 * @brief   敵管理クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/TaskManager/TaskManager.h"

// クラスの前方宣言 ===================================================
class IEnemy; // 敵のインターフェース
class Camera; // カメラ

// クラスの定義 ===============================================================
/**
 * @brief 敵管理クラス
 */
class EnemyManager
	: public Task
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	std::vector<IEnemy*> m_enemies; ///< 管理している敵


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EnemyManager();

	// デストラクタ
	~EnemyManager();


// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 描画処理
	void DrawTask(const Camera& camera) override;

	// 終了処理
	void Finalize();

	void AddEnemy(IEnemy* enemy);


// 取得/設定
public:


// 内部実装
private:


};
