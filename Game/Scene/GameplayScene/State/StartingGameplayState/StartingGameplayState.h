/*****************************************************************//**
 * @file    StartingGameplayState.h
 * @brief   ゲームプレイシーンの開始状態に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/01/27
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// グラフィック関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"

// 状態関連
#include "Game/Common/Framework/StateMachine/StateBase/StateBase.h"

// ユーティリティ関連
#include "Game/Common/Utillities/ElapsedTimeCounter/ElapsedTimeCounter.h"
#include "Game/Common/Utillities/DxTween/DxTween.h"

// クラスの前方宣言 ===================================================
class GameplayScene;		// ゲームプレイシーン（所有者）
class Canvas;				// キャンバス
class Sprite;				// スプライト
class PauseNavigator;	// ポーズナビゲーター
struct InputEventData;	// 入力イベント時のデータ
// クラスの定義 ===============================================================
/**
 * @brief ゲームプレイシーンの開始状態
 */
class StartingGameplayState
	: public StateBase<GameplayScene>
{
// 列挙型の宣言		-------------------------------------------------
private:
	enum  class SpriteID : UINT
	{
		BACKGROUND_APLPHA = 0,
		PUSHGUIDE_KEYBOARD,
		PUSHGUIDE_GAMEPAD,
		ITEM,

		NUM
	};


// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr UINT SPRITE_NUM = 4;

// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<Sprite> m_sprites[static_cast<UINT>(SpriteID::NUM)]; ///< スプライト

	ElapsedTimeCounter m_elapsedTimeCounter;
	DxTween<DirectX::SimpleMath::Vector2> m_backgroundTween;
	DxTween<float> m_itemAlphaTween;
	DxTween<float> m_guideAlphaTween;

	std::unique_ptr<PauseNavigator> m_pauseNavigator;	///< ポーズナビゲーター


	DirectX::SimpleMath::Vector2 m_startPosition;
// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StartingGameplayState();

	// デストラクタ
	~StartingGameplayState();


// 操作
public:

	// 状態の開始
	void OnStartState();

	// 更新処理
	void OnUpdate(float deltaTime) override;

	// 描画処理
	void OnDraw() override;

	// 状態の終了処理
	void OnExitState() override;

	


// 取得/設定
public:


// 内部実装
private:
	// 入力のコールバックの登録
	void RegisterBindCallbackToInput();
	// 紐づけの解除をする
	void UnBindCallbackToInput();
	// ナビゲーターの作成
	void CreatePauseNavigator();

	// ポーズ画面を開く処理
	void OnOpenPause(const InputEventData& data);

	// ゲームの開始
	void OnStartGame(const InputEventData& data);
};
