/*****************************************************************//**
 * @file    PlayerInput.h
 * @brief   プレイヤー入力に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <unordered_map>

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief プレイヤー入力
 */
class PlayerInput
{
// 列挙型 -------------------------------------------------
public:
	enum class ActionID
	{
		JUMPING,
		FRONT_MOVE,
		RIGHT_MOVE,
		LEFT_MOVE,
		BACK_MOVE,
		WIRE_SHOOTING,
		RELEASE_WIRE,
		STEPPING,
	};

	enum class MouseButtons
	{
		LEFT_BUTTON,
		RIGHT_BUTTON,
		MIDDLE_BUTTON,
	};

	enum class GamePadButtons
	{
		A,
		B,
		X,
		Y,

		LEFT_STICK,
		RIGHT_STICK,

		LEFT_SHOULDER,		// 左上ボタン (LB)
		RIGHT_SHOULDER,		// 右上ボタン (RB)

		LEFT_TRIGGER,		// 左トリガー (LT)
		RIGHT_TRIGGER,		// 右トリガー (RT)

		START,				// スタートボタン
		MENU,				// メニューボタン

		D_PAD_UP,			// 方向ボタン (上)
		D_PAD_DOWN,			// 方向ボタン (下)
		D_PAD_LEFT,			// 方向ボタン (左)
		D_PAD_RIGHT,		// 方向ボタン (右)

		LEFT_STICK_UP,		// 左スティック (上)
		LEFT_STICK_DOWN,	// 左スティック (下)
		LEFT_STICK_LEFT,	// 左スティック (左)
		LEFT_STICK_RIGHT,	// 左スティック (右)

		RIGHT_STICK_UP,		// 右スティック (上)
		RIGHT_STICK_DOWN,	// 右スティック (下)
		RIGHT_STICK_LEFT,	// 右スティック (左)
		RIGHT_STICK_RIGHT,	// 右スティック (右)
	};

	enum class InputOption
	{
		DOWN,
		PRESSED,
		RELEASED
	};

// 構造体-------------------------------------------------
public:

	struct InputData
	{
		std::vector<DirectX::Keyboard::Keys> keys; // 判定キー
		std::vector<MouseButtons> mouseButtons;
		std::vector<GamePadButtons> gamePadButtons;
	};

	std::unordered_map<ActionID, InputData> m_inputs; ///< 入力情報と動作の紐づけ群

	std::unordered_map < ActionID, std::unordered_map<InputOption, bool>> m_currentInputState; ///< 現在の入力状況

	const DirectX::Keyboard::KeyboardStateTracker* m_pKeyboardStateTracker;
	const DirectX::Mouse::ButtonStateTracker* m_pMouseStateTracker;
	const DirectX::GamePad::ButtonStateTracker * m_pGamePadStateTracker;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PlayerInput();

	// デストラクタ
	~PlayerInput();


// 操作
public:

	// 更新処理
	void Update(
		const DirectX::Keyboard::KeyboardStateTracker*  pKeyboardStateTracker,
		const DirectX::Mouse::ButtonStateTracker*		pMouseStateTracker,
		const DirectX::GamePad::ButtonStateTracker* pGamePadStateTracker);

	// 入力されたかどうか
	bool IsInput(const ActionID& actionID, const InputOption& inputOption = InputOption::DOWN);


// 取得/設定
public:


// 内部実装
private:

	bool CheckInputMouse(const InputData& inputData, const InputOption& inputOption);
	bool CheckInputGamePad(const InputData& inputData, const InputOption& inputOption);

};
