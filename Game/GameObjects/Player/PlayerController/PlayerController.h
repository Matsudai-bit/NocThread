/*****************************************************************//**
 * @file    PlayerController.h
 * @brief   PlayerControllerに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/06/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================
class Player;	// プレイヤー
class Camera;	// カメラ

// クラスの定義 ===============================================================
/**
 * @brief プレイヤーのコントローラ
 */
class PlayerController
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	Player* m_pPlayer; // 操作対象
	Camera* m_pCamera; // カメラ

	 
	

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PlayerController(Player* pPlayer, Camera* pCamera);

	// デストラクタ
	~PlayerController();


// 操作
public:
	// 初期化処理
	void Initialize(Player* pPlayer, Camera* pCamera);

	// 更新処理
	void Update(float elapsedTime, const DirectX::Keyboard::KeyboardStateTracker* pKeyboardStateTracker, const DirectX::Mouse::ButtonStateTracker* pMouseStateTracker);

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();


// 取得/設定
public:


// 内部実装
private:


};
