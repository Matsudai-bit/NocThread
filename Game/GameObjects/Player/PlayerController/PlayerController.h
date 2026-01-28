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
#include <memory>
#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"

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
	const Camera* m_pCamera; // カメラ

	std::unique_ptr<InputSystem<InputActionType::PlyayerActionID>> m_playerInput;
	
	Player* m_pPlayer;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PlayerController();

	// デストラクタ
	~PlayerController();


// 操作
public:
	// 初期化処理
	void Initialize(const Camera* pCamera, Player* pPlayer);

	// 更新処理
	void Update(
		float deltaTime,
		const DirectX::Keyboard::KeyboardStateTracker* pKeyboardStateTracker,
		const DirectX::Mouse::ButtonStateTracker* pMouseStateTracker, const DirectX::GamePad::ButtonStateTracker* pGamePadStateTracker);

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();

	bool TryStep(bool isJumping);

	bool TryWalk();

	bool TryJumping();


// 取得/設定
public:


// 内部実装
private:


};
