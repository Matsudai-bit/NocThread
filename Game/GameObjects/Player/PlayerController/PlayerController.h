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
class Player;		// プレイヤー
class Camera;		// カメラ
class InputManager; // 入力管理
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
	
	Player* m_pPlayer;

	InputManager* m_pInputManager; ///< 入力管理

	DirectX::SimpleMath::Vector3 m_movementDirection;

	bool m_isSteppingRequest;		///< ステップ要求
	bool m_isJumpingRequest;		///< ジャンプ要求
	bool m_isReleaseWireRequested;  ///< ワイヤーを離す要求
	bool m_isShootWireRequested;	///< ワイヤー発射要求

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
	void Initialize(const Camera* pCamera, Player* pPlayer, InputManager* pInputManager);

	// 更新処理
	void Update(float deltaTime);

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

	// 入力のコールバックの登録
	void RegisterBindCallbackToInput();
	// 紐づけの解除をする
	void UnBindCallbackToInput();


	// 移動方向に加算する
	void AddMoveDirection(const DirectX::SimpleMath::Vector3& addDirection);


};
