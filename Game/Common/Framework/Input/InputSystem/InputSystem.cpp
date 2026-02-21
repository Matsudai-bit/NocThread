/*****************************************************************//**
 * @file    InputSystem.h
 * @brief   入力システムに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/01/30
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputSystem.h"



// ライブラリ

// ファクトリー関連

// データベース関連

// フレームワーク関連
#include "Game/Common/Framework/Input/InputActionMap/InputActionMap.h"

// ゲームプレイロジック関連

// グラフィック関連


// メンバ関数の定義 ===========================================================



/**
 * @brief コンストラクタ
 * 
 * @param[in] pKeyboardStateTracker　キーボード情報
 * @param[in] pMouseStateTracker	 マウス情報
 * @param[in] pGamePadStateTracker	 ゲームパッド情報
 */
InputSystem::InputSystem(
	const DirectX::Keyboard::KeyboardStateTracker*	pKeyboardStateTracker,
	const DirectX::Mouse::ButtonStateTracker*		pMouseStateTracker, 
	const DirectX::GamePad::ButtonStateTracker*		pGamePadStateTracker)
	: m_pKeyboardStateTracker	{ pKeyboardStateTracker }
	, m_pMouseStateTracker		{ pMouseStateTracker }
	, m_pGamePadStateTracker	{ pGamePadStateTracker }
{
}

/**
 * @brief デストラクタ
 */
InputSystem::~InputSystem()
{

}

/**
 * @brief 更新処理
 */
void InputSystem::Update()
{
	// アクションマップごとに入力の確認
	for (auto& actionMap : m_inputActionMaps)
	{
		CheckInputActionMap(actionMap.second.get());
	}

}

/**
 * @brief アクションマップの追加
 * 
 * @param[in] inputActionMap 追加するアクションマップ
 */
void InputSystem::AddActionMap(std::unique_ptr<InputActionMap> inputActionMap)
{
	m_inputActionMaps[inputActionMap->GetActionMapName()] = std::move(inputActionMap);
}

/**
 * @brief アクションマップの入力確認
 * 
 * @param[in] pInputActionMap　アクションマップのポインタ
 */
void InputSystem::CheckInputActionMap(InputActionMap* pInputActionMap)
{
	// マップデータのデータ毎に処理する
	for (auto& data : pInputActionMap->GetActionMap())
	{
		// アクションデータの抽出
		ActionData* pActionData = &data.second;

		// 初期化する
		pActionData->inputOption = InputOptionData();
		pActionData->isInput = false;
		
		// 入力データの確認
		for (InputData& inputData : pActionData->inputs)
		{
			// 入力オプション
			InputOptionData inputOption{};

			if (!CheckInputData(inputData, &inputOption)) { continue; }

			// アクションデータに結果を登録
			pActionData->inputOption = inputOption;
			pActionData->isInput = true;

			// イベントデータの作成
			InputEventData eventData{};
			eventData.actionMapName = pInputActionMap->GetActionMapName();
			eventData.actionName = data.first;
			eventData.inputOption = inputOption;

			// コールバックを呼び出す
			for (auto it = pActionData->callbacks.begin(); it != pActionData->callbacks.end();)
			{
				if (!(it->owner || it->callback))
				{
					it = pActionData->callbacks.erase(it);
				}
				else
				{
					it->callback(eventData);
					it++;
				}
			}

		}
		
	}
}

/**
 * @brief 入力情報を元に入力されたかどうかの確認
 * 
 * @param[in]  inputData 入力データ
 * @param[out] pOption	 オプション
 * 
 * @returns true 　入力された
 * @returns false  入力されていない
 */
bool InputSystem::CheckInputData(const InputData& inputData, InputOptionData* pOption)
{
	// 結果
	bool result = false;

	// キーボードの確認
	if (!result && inputData.keys.size() > 0){result = CheckInputKeyboard(inputData, pOption);	}
	// マウスの確認
	if (!result && inputData.mouseButtons.size() > 0){result = CheckInputMouse(inputData, pOption);		}
	// ゲームパッドの確認
	if (!result && inputData.gamePadButtons.size() > 0){result = CheckInputGamePad(inputData, pOption);	}

	return result;
}

/**
 * @brief 入力情報を元にキーボード入力されたかどうかの確認
 *
 * @param[in]  inputData 入力データ
 * @param[out] pOption	 オプション
 *
 * @returns true 　入力された
 * @returns false  入力されていない
 */
bool InputSystem::CheckInputKeyboard(const InputData& inputData, InputOptionData* pInputOption)
{
	bool result = true;

	for (auto& key : inputData.keys)
	{
		// 押された時
		pInputOption->pressed = m_pKeyboardStateTracker->IsKeyPressed(key);
		// 離された時
		pInputOption->released = m_pKeyboardStateTracker->IsKeyReleased(key);
		// 押していない時
		if (!pInputOption->released && m_pKeyboardStateTracker->GetLastState().IsKeyUp(key))
		{
			result = false;
			pInputOption->pressed = false;
			pInputOption->released = false;
			break;
		}

	}

	return result;
}

/**
 * @brief 入力情報を元にマウス入力されたかどうかの確認
 *
 * @param[in]  inputData 入力データ
 * @param[out] pOption	 オプション
 *
 * @returns true 　入力された
 * @returns false  入力されていない
 */
bool InputSystem::CheckInputMouse(const InputData& inputData, InputOptionData* pInputOption)
{
	using namespace DirectX;

	bool result = false;

	// 入力確認用関数
	auto checkInput = [&](Mouse::ButtonStateTracker::ButtonState buttonState) {

			pInputOption->pressed = (buttonState == Mouse::ButtonStateTracker::ButtonState::PRESSED);
			pInputOption->released = (buttonState == Mouse::ButtonStateTracker::ButtonState::RELEASED);

			// 押していなければ
			if (!pInputOption->released && buttonState == Mouse::ButtonStateTracker::ButtonState::UP){	return false; }

			return true;
		};

	// DirectXTKのマウスボタンとの対応付け
	std::unordered_map<Input::MouseButtons, Mouse::ButtonStateTracker::ButtonState> buttonStateAccessor
	{
		{Input::MouseButtons::LEFT_BUTTON,		m_pMouseStateTracker->leftButton},
		{Input::MouseButtons::RIGHT_BUTTON,	m_pMouseStateTracker->rightButton},
		{Input::MouseButtons::MIDDLE_BUTTON,	m_pMouseStateTracker->middleButton}
	};


	// マウスの確認
	for (auto& button : inputData.mouseButtons)
	{

		for (auto& buttonState : buttonStateAccessor)
		{
			// ボタンの確認
			if (button == buttonState.first)
			{
				result = checkInput(buttonState.second);

				if (!result)
				{
					pInputOption->pressed = false;
					pInputOption->released = false;
					return false;
				}
			}
		}

	}
	return result;
}

/**
 * @brief 入力情報を元にゲームパッド入力されたかどうかの確認
 *
 * @param[in]  inputData 入力データ
 * @param[out] pOption	 オプション
 *
 * @returns true 　入力された
 * @returns false  入力されていない
 */
bool InputSystem::CheckInputGamePad(const InputData& inputData, InputOptionData* pInputOption)
{
	using namespace DirectX;

	bool result = false;

	// 入力確認用関数

	auto checkInput = [&](GamePad::ButtonStateTracker::ButtonState buttonState) {
			pInputOption->pressed = (buttonState == GamePad::ButtonStateTracker::ButtonState::PRESSED);
			pInputOption->released = (buttonState == GamePad::ButtonStateTracker::ButtonState::RELEASED);
			// 押していなければ
			if (!pInputOption->released && buttonState == GamePad::ButtonStateTracker::ButtonState::UP){return false;	}

			return true;
		};

	auto state = m_pGamePadStateTracker;
	// DirectXTKのゲームパッドボタンとの対応付け
	std::unordered_map<Input::GamePadButtons, GamePad::ButtonStateTracker::ButtonState> buttonStateAccessor
	{
		{Input::GamePadButtons::A, state->a},
		{Input::GamePadButtons::B, state->b},
		{Input::GamePadButtons::X, state->x},
		{Input::GamePadButtons::Y, state->y},

		{Input::GamePadButtons::LEFT_STICK,	state->leftStick},
		{Input::GamePadButtons::RIGHT_STICK,	state->rightStick},

		{Input::GamePadButtons::LEFT_SHOULDER, state->leftShoulder},
		{Input::GamePadButtons::RIGHT_SHOULDER,state->rightShoulder},

		{Input::GamePadButtons::LEFT_TRIGGER,	state->leftTrigger},
		{Input::GamePadButtons::RIGHT_TRIGGER, state->rightTrigger},

		{Input::GamePadButtons::START,			state->start},
		{Input::GamePadButtons::MENU,			state->menu},

		{Input::GamePadButtons::D_PAD_UP,		state->dpadUp},
		{Input::GamePadButtons::D_PAD_DOWN,	state->dpadDown},
		{Input::GamePadButtons::D_PAD_LEFT,	state->dpadLeft},
		{Input::GamePadButtons::D_PAD_RIGHT,	state->dpadRight},

		{Input::GamePadButtons::LEFT_STICK_UP,		state->leftStickUp},
		{Input::GamePadButtons::LEFT_STICK_DOWN,	state->leftStickDown},
		{Input::GamePadButtons::LEFT_STICK_LEFT,	state->leftStickLeft},
		{Input::GamePadButtons::LEFT_STICK_RIGHT,	state->leftStickRight},

		{Input::GamePadButtons::RIGHT_STICK_UP,	state->rightStickUp},
		{Input::GamePadButtons::RIGHT_STICK_DOWN,	state->rightStickDown},
		{Input::GamePadButtons::RIGHT_STICK_LEFT,	state->rightStickLeft},
		{Input::GamePadButtons::RIGHT_STICK_RIGHT, state->rightStickRight},
	};


	// ゲームパッドの確認
	for (auto& button : inputData.gamePadButtons)
	{

		for (auto& buttonState : buttonStateAccessor)
		{
			// ボタンの確認
			if (button == buttonState.first)
			{
				result = checkInput(buttonState.second);

				if (!result)
				{
					pInputOption->pressed = false;
					pInputOption->released = false;
					return false;
				}
			}
		}

	}
	return result;
}
