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
		std::vector<MouseButtons> buttons;
	};

	std::unordered_map<ActionID, InputData> m_inputs; ///< 入力情報と動作の紐づけ群

	std::unordered_map < ActionID, std::unordered_map<InputOption, bool>> m_currentInputState; ///< 現在の入力状況

	const DirectX::Keyboard::KeyboardStateTracker* m_pKeyboardStateTracker;
	const DirectX::Mouse::ButtonStateTracker* m_pMouseStateTracker;

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
		const DirectX::Mouse::ButtonStateTracker*		pMouseStateTracker);

	// 入力されたかどうか
	bool IsInput(const ActionID& actionID, const InputOption& inputOption = InputOption::DOWN);


// 取得/設定
public:


// 内部実装
private:


};
