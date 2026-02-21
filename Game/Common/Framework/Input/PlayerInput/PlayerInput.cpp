/*****************************************************************//**
 * @file    PlayerInput.h
 * @brief   プレイヤー入力に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PlayerInput.h"


using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PlayerInput::PlayerInput()
	: m_pKeyboardStateTracker	{ nullptr }
	, m_pMouseStateTracker		{ nullptr }
	, m_pGamePadStateTracker	{ nullptr }
{
	// ステップ
	m_inputs[ActionID::STEPPING].mouseButtons.emplace_back(MouseButtons::RIGHT_BUTTON);
	m_inputs[ActionID::STEPPING].gamePadButtons.emplace_back(GamePadButtons::B);

	// ワイヤー発射
	m_inputs[ActionID::WIRE_SHOOTING].mouseButtons.emplace_back(MouseButtons::LEFT_BUTTON);
	m_inputs[ActionID::WIRE_SHOOTING].gamePadButtons.emplace_back(GamePadButtons::RIGHT_TRIGGER);

	// ワイヤー発射の終了
	m_inputs[ActionID::RELEASE_WIRE].mouseButtons.emplace_back(MouseButtons::LEFT_BUTTON);
	m_inputs[ActionID::RELEASE_WIRE].gamePadButtons.emplace_back(GamePadButtons::RIGHT_TRIGGER);

	// ジャンプ
	m_inputs[ActionID::JUMPING].keys.emplace_back(Keyboard::Space);
	m_inputs[ActionID::JUMPING].gamePadButtons.emplace_back(GamePadButtons::A);


	// 左移動
	m_inputs[ActionID::LEFT_MOVE].keys.emplace_back(Keyboard::Left);
	m_inputs[ActionID::LEFT_MOVE].keys.emplace_back(Keyboard::A);
	m_inputs[ActionID::LEFT_MOVE].gamePadButtons.emplace_back(GamePadButtons::LEFT_STICK_LEFT);

	// 右移動
	m_inputs[ActionID::RIGHT_MOVE].keys.emplace_back(Keyboard::Right);
	m_inputs[ActionID::RIGHT_MOVE].keys.emplace_back(Keyboard::D);
	m_inputs[ActionID::RIGHT_MOVE].gamePadButtons.emplace_back(GamePadButtons::LEFT_STICK_RIGHT);

	// 正面移動
	m_inputs[ActionID::FRONT_MOVE].keys.emplace_back(Keyboard::Up);
	m_inputs[ActionID::FRONT_MOVE].keys.emplace_back(Keyboard::W);
	m_inputs[ActionID::FRONT_MOVE].gamePadButtons.emplace_back(GamePadButtons::LEFT_STICK_UP);


	// 後ろに移動
	m_inputs[ActionID::BACK_MOVE].keys.emplace_back(Keyboard::Back);
	m_inputs[ActionID::BACK_MOVE].keys.emplace_back(Keyboard::S);
	m_inputs[ActionID::BACK_MOVE].gamePadButtons.emplace_back(GamePadButtons::LEFT_STICK_DOWN);



}



/**
 * @brief デストラクタ
 */
PlayerInput::~PlayerInput()
{

}

/**
 * @brief 更新処理
 * 
 * @param[in] pKeyboardStateTracker　キーボード情報
 * @param[in] pMouseStateTracker	 マウス情報
 * @param[in] pGamePadStateTracker	 ゲームパッド情報
 */
void PlayerInput::Update(
	const DirectX::Keyboard::KeyboardStateTracker* pKeyboardStateTracker, 
	const DirectX::Mouse::ButtonStateTracker* pMouseStateTracker,
	const DirectX::GamePad::ButtonStateTracker* pGamePadStateTracker)
{
	// リセット
	m_currentInputState.clear();

	m_pKeyboardStateTracker = pKeyboardStateTracker;
	m_pMouseStateTracker	= pMouseStateTracker;
	m_pGamePadStateTracker	= pGamePadStateTracker;
}

/**
 * @brief 入力されたかどうか
 * 
 * @param[in] actitonID　動作ID
 * @param[in] inputOptionオプション [デフォルトはDOWN]
 * 
 * @returns true  入力された
 * @returns false 入力されていない
 */
bool PlayerInput::IsInput(const ActionID& actionID, const InputOption& inputOption)
{
	// 既にデータに入っているかどうか
	if (m_currentInputState.count(actionID)
		&& m_currentInputState[actionID].count(inputOption))
	{
		// あればそれを送る
		return m_currentInputState[actionID][inputOption];
	}

	InputData inputData = m_inputs[actionID];

	bool result = false;

	if (!m_pKeyboardStateTracker->GetLastState().IsKeyDown(Keyboard::None))
	{
		// キーボードの確認
		for (auto& key : inputData.keys)
		{
			if (result) { break; }

			switch (inputOption)
			{
				// 押している時
			case PlayerInput::InputOption::DOWN:
				result = m_pKeyboardStateTracker->GetLastState().IsKeyDown(key);
				break;
				// 押された時
			case PlayerInput::InputOption::PRESSED:
				result = m_pKeyboardStateTracker->IsKeyPressed(key);
				break;
				// 離された時
			case PlayerInput::InputOption::RELEASED:
				result = m_pKeyboardStateTracker->IsKeyReleased(key);
				break;
			default:
				break;
			}
		}
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

bool PlayerInput::CheckInputMouse(const InputData& inputData, const InputOption& inputOption)
{
	bool result = false;

	auto checkInput = [&](Mouse::ButtonStateTracker::ButtonState buttonState) {
		switch (inputOption)
		{
			// 押している時
		case PlayerInput::InputOption::DOWN:
			if (buttonState == Mouse::ButtonStateTracker::ButtonState::HELD) { return true; }
			break;
			// 押された時
		case PlayerInput::InputOption::PRESSED:
			if (buttonState == Mouse::ButtonStateTracker::ButtonState::PRESSED) { return true; }
			break;
			// 離された時
		case PlayerInput::InputOption::RELEASED:
			if (buttonState == Mouse::ButtonStateTracker::ButtonState::RELEASED) { return true; }
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

bool PlayerInput::CheckInputGamePad(const InputData& inputData, const InputOption& inputOption)
{
	bool result = false;

	auto checkInput = [&](GamePad::ButtonStateTracker::ButtonState buttonState) {
		switch (inputOption)
		{
			// 押している時
		case PlayerInput::InputOption::DOWN:
			if (buttonState == GamePad::ButtonStateTracker::ButtonState::HELD) { return true; }
			break;
			// 押された時
		case PlayerInput::InputOption::PRESSED:
			if (buttonState == GamePad::ButtonStateTracker::ButtonState::PRESSED) { return true; }
			break;
			// 離された時
		case PlayerInput::InputOption::RELEASED:
			if (buttonState == GamePad::ButtonStateTracker::ButtonState::RELEASED) { return true; }
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


