/*****************************************************************//**
 * @file    InputSystem.h
 * @brief   インプットシステムに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// クラスの前方宣言 ===================================================


// クラスの定義 ===============================================================
/**
 * @brief インプットシステム
 */
template<typename InputActionType>
class InputSystem
{
public:
	/**
* @brief 入力オプション
*/
	enum class InputOption
	{
		DOWN,		// 押している
		PRESSED,	// 押した瞬間
		RELEASED,	// 離した瞬間
		UP			// 押していない
	};
// 列挙型 -------------------------------------------------
public:


	/**
	 * @brief マウスボタン
	 */
	enum class MouseButtons
	{
		LEFT_BUTTON,
		RIGHT_BUTTON,
		MIDDLE_BUTTON,
	};

	/**
	 * @brief ゲームパッドボタン
	 */
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



// 構造体-------------------------------------------------
public:

	struct InputData
	{
		std::vector<DirectX::Keyboard::Keys> keys; // 判定キー
		std::vector<MouseButtons> mouseButtons;
		std::vector<GamePadButtons> gamePadButtons;
	};

	std::unordered_map<InputActionType, InputData> m_inputs; ///< 入力情報と動作の紐づけ群

	std::unordered_map < InputActionType, std::unordered_map<InputOption, bool>> m_currentInputState; ///< 現在の入力状況

	const DirectX::Keyboard::KeyboardStateTracker* m_pKeyboardStateTracker;
	const DirectX::Mouse::ButtonStateTracker* m_pMouseStateTracker;
	const DirectX::GamePad::ButtonStateTracker* m_pGamePadStateTracker;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:

	// メンバ関数の定義 ===========================================================
	/**
	 * @brief コンストラクタ
	 *
	 * @param[in] なし
	 */
	InputSystem()
		: m_pKeyboardStateTracker{ nullptr }
		, m_pMouseStateTracker{ nullptr }
		, m_pGamePadStateTracker{ nullptr }{}



	/**
	 * @brief デストラクタ
	 */
	~InputSystem()
	{}

	// 入力の登録 ---------------------------------------------------------------------------------------------
	public:

	/**
	 * @brief キーボード入力の登録
	 * 
	 * @param[in] actionID　動作ID
	 * @param[in] keyCode	キーコード
	 */
		void RegisterKeyboardInput(InputActionType actionID, DirectX::Keyboard::Keys keyCode)
	{
		// キーボード入力の登録
		m_inputs[actionID].keys.emplace_back(keyCode);
	}

	/**
	 * @brief マウス入力の登録
	 *
	 * @param[in] actionID　	動作ID
	 * @param[in] mouseButton	マウスボタン
	 */
	void RegisterMouseInput(InputActionType actionID,MouseButtons button)
	{
		// マウス入力の登録
		m_inputs[actionID].mouseButtons.emplace_back(button);
	}


	/**
	 * @brief ゲームパッド入力の登録
	 *
	 * @param[in] actionID　	動作ID
	 * @param[in] gamepadButton	ゲームパッドボタン
	 */
	void RegisterGamePadInput(InputActionType actionID, GamePadButtons button)
	{
		// ゲームパッド入力の登録
		m_inputs[actionID].gamePadButtons.emplace_back(button);
	}

	// 操作 ---------------------------------------------------------------------------------------------
	public :
	/**
	 * @brief 更新処理
	 *
	 * @param[in] pKeyboardStateTracker　キーボード情報
	 * @param[in] pMouseStateTracker	 マウス情報
	 * @param[in] pGamePadStateTracker	 ゲームパッド情報
	 */
	void Update(
		const DirectX::Keyboard::KeyboardStateTracker* pKeyboardStateTracker,
		const DirectX::Mouse::ButtonStateTracker* pMouseStateTracker,
		const DirectX::GamePad::ButtonStateTracker* pGamePadStateTracker)
	{
		// リセット
		m_currentInputState.clear();

		// それぞれのトラッカーの取得
		m_pKeyboardStateTracker = pKeyboardStateTracker;
		m_pMouseStateTracker = pMouseStateTracker;
		m_pGamePadStateTracker = pGamePadStateTracker;
	}

	/**
	 * @brief 入力されたかどうか
	 *
	 * @param[in] actionID　動作ID
	 * @param[in] inputOptionオプション [デフォルトはDOWN]
	 *
	 * @returns true  入力された
	 * @returns false 入力されていない
	 */
	bool IsInput(const InputActionType& actionID, const InputOption& inputOption = InputOption::DOWN)
	{
		using namespace DirectX;

		// 既にデータに入っているかどうか
		if (m_currentInputState.count(actionID)
			&& m_currentInputState[actionID].count(inputOption))
		{
			// あればそれを送る
			return m_currentInputState[actionID][inputOption];
		}

		// 指定されたアクションの入力データの取得
		InputData inputData = m_inputs[actionID];

		// 結果
		bool result = false;

		if (!result)
		{
			result = CheckInputKeyboard(inputData, inputOption);
		}

		if (!result)
		{
			result = CheckInputMouse(inputData, inputOption);

		}

		if (!result)
		{
			result = CheckInputGamePad(inputData, inputOption);
		}

		// 結果の登録
		m_currentInputState[actionID][inputOption] = result;

		return result;
	}

	// 内部実装 ------------------------------------------------------------------------
	private:

	bool CheckInputKeyboard(const InputData& inputData, const InputOption& inputOption)
	{
		using namespace DirectX;
		bool result = false;
		if (!m_pKeyboardStateTracker->GetLastState().IsKeyDown(Keyboard::None))
		{
			// キーボードの確認
			for (auto& key : inputData.keys)
			{
				if (result) { return true; }

				switch (inputOption)
				{
					// 押している時
				case InputSystem::InputOption::DOWN:
					result = m_pKeyboardStateTracker->GetLastState().IsKeyDown(key);
					break;
					// 押された時
				case InputSystem::InputOption::PRESSED:
					result = m_pKeyboardStateTracker->IsKeyPressed(key);
					break;
					// 離された時
				case InputSystem::InputOption::RELEASED:
					result = m_pKeyboardStateTracker->IsKeyReleased(key);
					break;
					// 押していない時
				case InputOption::UP:
					result = m_pKeyboardStateTracker->GetLastState().IsKeyUp(key);
					break;
				default:
					break;
				}
			}
		}
		// もし押していない状態を見ていれば
		else if (InputOption::UP == inputOption)
		{
			result = true;
		}

		return result;
	}

	bool CheckInputMouse(const InputData& inputData, const InputOption& inputOption)
	{
		using namespace DirectX;

		bool result = false;

		auto checkInput = [&](Mouse::ButtonStateTracker::ButtonState buttonState) {
			switch (inputOption)
			{
				// 押している時
			case InputOption::DOWN:
				if (buttonState == Mouse::ButtonStateTracker::ButtonState::HELD) { return true; }
				break;
				// 押された時
			case InputOption::PRESSED:
				if (buttonState == Mouse::ButtonStateTracker::ButtonState::PRESSED) { return true; }
				break;
				// 離された時
			case InputOption::RELEASED:
				if (buttonState == Mouse::ButtonStateTracker::ButtonState::RELEASED) { return true; }
				break;
				// 押していない時
			case InputOption::UP:
				if (buttonState == Mouse::ButtonStateTracker::ButtonState::UP) { return true; }
				break;
			default:
				break;
			}
			return false;
			};

		std::unordered_map<MouseButtons, Mouse::ButtonStateTracker::ButtonState> buttonStateAccessor
		{
			{MouseButtons::LEFT_BUTTON,		m_pMouseStateTracker->leftButton},
			{MouseButtons::RIGHT_BUTTON,	m_pMouseStateTracker->rightButton},
			{MouseButtons::MIDDLE_BUTTON,	m_pMouseStateTracker->middleButton}
		};



		// マウスの確認
		for (auto& button : inputData.mouseButtons)
		{
			if (result) { break; }


			for (auto& buttonState : buttonStateAccessor)
			{
				if (result) { break; }
				// ボタンの確認
				if (button == buttonState.first)
				{
					result = checkInput(buttonState.second);

				}
			}

		}
		return result;
	}

	bool CheckInputGamePad(const InputData& inputData, const InputOption& inputOption)
	{
		using namespace DirectX;

		bool result = false;

		auto checkInput = [&](GamePad::ButtonStateTracker::ButtonState buttonState) {
			switch (inputOption)
			{
				// 押している時
			case InputOption::DOWN:
				if (buttonState == GamePad::ButtonStateTracker::ButtonState::HELD) { return true; }
				break;
				// 押された時
			case InputOption::PRESSED:
				if (buttonState == GamePad::ButtonStateTracker::ButtonState::PRESSED) { return true; }
				break;
				// 離された時
			case InputOption::RELEASED:
				if (buttonState == GamePad::ButtonStateTracker::ButtonState::RELEASED) { return true; }
				break;
				// 押していない時
			case InputOption::UP:
				if (buttonState == GamePad::ButtonStateTracker::ButtonState::UP) { return true; }
				break;
			default:
				break;
			}
			return false;
			};

		auto state = m_pGamePadStateTracker;
		std::unordered_map<GamePadButtons, GamePad::ButtonStateTracker::ButtonState> buttonStateAccessor
		{
			{GamePadButtons::A, state->a},
			{GamePadButtons::B, state->b},
			{GamePadButtons::X, state->x},
			{GamePadButtons::Y, state->y},

			{GamePadButtons::LEFT_STICK,	state->leftStick},
			{GamePadButtons::RIGHT_STICK,	state->rightStick},

			{GamePadButtons::LEFT_SHOULDER, state->leftShoulder},
			{GamePadButtons::RIGHT_SHOULDER,state->rightShoulder},

			{GamePadButtons::LEFT_TRIGGER,	state->leftTrigger},
			{GamePadButtons::RIGHT_TRIGGER, state->rightTrigger},

			{GamePadButtons::START,			state->start},
			{GamePadButtons::MENU,			state->menu},

			{GamePadButtons::D_PAD_UP,		state->dpadUp},
			{GamePadButtons::D_PAD_DOWN,	state->dpadDown},
			{GamePadButtons::D_PAD_LEFT,	state->dpadLeft},
			{GamePadButtons::D_PAD_RIGHT,	state->dpadRight},

			{GamePadButtons::LEFT_STICK_UP,		state->leftStickUp},
			{GamePadButtons::LEFT_STICK_DOWN,	state->leftStickDown},
			{GamePadButtons::LEFT_STICK_LEFT,	state->leftStickLeft},
			{GamePadButtons::LEFT_STICK_RIGHT,	state->leftStickRight},

			{GamePadButtons::RIGHT_STICK_UP,	state->rightStickUp},
			{GamePadButtons::RIGHT_STICK_DOWN,	state->rightStickDown},
			{GamePadButtons::RIGHT_STICK_LEFT,	state->rightStickLeft},
			{GamePadButtons::RIGHT_STICK_RIGHT, state->rightStickRight},
		};


		// マウスの確認
		for (auto& button : inputData.gamePadButtons)
		{
			if (result) { break; }


			for (auto& buttonState : buttonStateAccessor)
			{
				if (result) { break; }
				// ボタンの確認
				if (button == buttonState.first)
				{
					result = checkInput(buttonState.second);

				}
			}

		}

		return result;
	}



};
