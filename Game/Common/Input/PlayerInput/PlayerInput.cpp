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
{
	m_inputs[ActionID::STEPPING].buttons.emplace_back(MouseButtons::RIGHT_BUTTTON);

	m_inputs[ActionID::WIRE_SHOOTING].buttons.emplace_back(MouseButtons::LEFT_BUTTON);

	m_inputs[ActionID::JUMPING].keys.emplace_back(Keyboard::Space);


	m_inputs[ActionID::LEFT_MOVE].keys.emplace_back(Keyboard::Left);
	m_inputs[ActionID::LEFT_MOVE].keys.emplace_back(Keyboard::A);

	m_inputs[ActionID::RIGHT_MOVE].keys.emplace_back(Keyboard::Right);
	m_inputs[ActionID::RIGHT_MOVE].keys.emplace_back(Keyboard::D);

	m_inputs[ActionID::FRONT_MOVE].keys.emplace_back(Keyboard::Up);
	m_inputs[ActionID::FRONT_MOVE].keys.emplace_back(Keyboard::W);

	m_inputs[ActionID::BACK_MOVE].keys.emplace_back(Keyboard::Back);
	m_inputs[ActionID::BACK_MOVE].keys.emplace_back(Keyboard::S);



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
 */
void PlayerInput::Update(
	const DirectX::Keyboard::KeyboardStateTracker* pKeyboardStateTracker, 
	const DirectX::Mouse::ButtonStateTracker* pMouseStateTracker)
{
	m_pKeyboardStateTracker = pKeyboardStateTracker;
	m_pMouseStateTracker	= pMouseStateTracker;
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
	InputData inputData = m_inputs[actionID];

	// キーボードの確認
	for (auto& key : inputData.keys)
	{
		switch (inputOption)
		{
		// 押している時
		case PlayerInput::InputOption::DOWN:
			if (m_pKeyboardStateTracker->GetLastState().IsKeyDown(key)) { return true; }
			break;
		// 押された時
		case PlayerInput::InputOption::PRESSED:
			if (m_pKeyboardStateTracker->IsKeyPressed(key)) { return true; }
			break;
		// 離された時
		case PlayerInput::InputOption::RELEASED:
			if (m_pKeyboardStateTracker->IsKeyReleased(key)) { return true; }
			break;
		default:
			break;
		}
	}

	// マウスの確認
	for (auto& button : inputData.buttons)
	{
		auto checkInput = [&](Mouse::ButtonStateTracker::ButtonState buttonState) {
			switch (inputOption)
			{
				// 押している時
			case PlayerInput::InputOption::DOWN:
				if (m_pMouseStateTracker->leftButton == Mouse::ButtonStateTracker::ButtonState::HELD) { return true; }
				break;
				// 押された時
			case PlayerInput::InputOption::PRESSED:
				if (m_pMouseStateTracker->leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED) { return true; }
				break;
				// 離された時
			case PlayerInput::InputOption::RELEASED:
				if (m_pMouseStateTracker->leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED) { return true; }
				break;
			default:
				break;
			}
		};

		int buttonInt[] = { static_cast<int>(MouseButtons::LEFT_BUTTON),  static_cast<int>(MouseButtons::RIGHT_BUTTTON), static_cast<int>(MouseButtons::MIDDLE_BUTTTON) };

		for (int i = 0; i < 3; i++)
		{
			// ボタンの確認
			if (button == static_cast<MouseButtons>(buttonInt[i]))
			{
				if (checkInput(m_pMouseStateTracker->leftButton))
				{
					return true;
				}
			}
		}

		//// 左ボタンの確認
		//if (button == MouseButtons::LEFT_BUTTON)
		//{
		//	if (checkInput(m_pMouseStateTracker->leftButton))
		//	{
		//		return true;
		//	}
		//}
		//// 右ボタンの確認
		//if (button == MouseButtons::RIGHT_BUTTTON)
		//{
		//	if (checkInput(m_pMouseStateTracker->rightButton))
		//	{
		//		return true;
		//	}
		//}
		//// 間のボタンの確認
		//if (button == MouseButtons::MIDDLE_BUTTTON)
		//{
		//	if (checkInput(m_pMouseStateTracker->middleButton))
		//	{
		//		return true;
		//	}
		//}
		//
	}
	return false;
}


