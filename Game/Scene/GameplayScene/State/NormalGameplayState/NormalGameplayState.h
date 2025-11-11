/*****************************************************************//**
 * @file    NormalGameplayState.h
 * @brief   ゲームプレイシーンの通常状態に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include "Game/Common/StateMachine/StateBase/StateBase.h"
#include "Game/Common/Input/InputSystem/InputSystem.h"
#include "Game/Common/Input/InputActionType/InputActionType.h"

// クラスの前方宣言 ===================================================
class GameplayScene; // ゲームプレイシーン
class Sprite;
// クラスの定義 ===============================================================
/**
 * @brief ゲームプレイシーンの通常状態
 */
class NormalGameplayState
	: public StateBase<GameplayScene>
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	std::unique_ptr<Sprite>	m_manualSprite;				///< 操作ガイドスプライト

	std::unique_ptr < InputSystem<InputActionType::SystemActionID>> m_systemInput; ///< ゲームシステムの入力判断

	bool m_isPrevConnectedGamepad; ///< ゲームパッドが接続されているかどうか

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	NormalGameplayState();

	// デストラクタ
	~NormalGameplayState();


// 操作
public:
	// 開始処理
	void OnStartState() override;
	void OnExitState() override;

	// 更新処理
	void OnUpdate(float deltaTime) override;

	// 描画処理
	void OnDraw() override;




// 取得/設定
public:


// 内部実装
private:

	// 現在のガイドガイドUIの変更を試みる
	bool TryChangeCurrentGuideUI();

};
