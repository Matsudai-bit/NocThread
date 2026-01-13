/*****************************************************************//**
 * @file    InputBindingFactory.h
 * @brief   入力と動作の紐づけを作成するファクトリーに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/22
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputBindingFactory.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
InputBindingFactory::InputBindingFactory()
{

}



/**
 * @brief デストラクタ
 */
InputBindingFactory::~InputBindingFactory()
{

}

std::unique_ptr<InputSystem<InputActionType::PlyayerActionID>> InputBindingFactory::CreatePlayerInput()
{
	// 動作の型
	using ActionID = InputActionType::PlyayerActionID;

	// 作成するインプットシステム
	auto playerInput = std::make_unique<InputSystem<ActionID>>();

	// *** 入力の登録 ****
	// ステップ
	playerInput->RegisterMouseInput(ActionID::STEPPING, InputSystem<ActionID>::MouseButtons::RIGHT_BUTTON);
	playerInput->RegisterGamePadInput(ActionID::STEPPING, InputSystem<ActionID>::GamePadButtons::A);

	// ワイヤー発射
	playerInput->RegisterMouseInput(ActionID::WIRE_SHOOTING,InputSystem<ActionID>::MouseButtons::LEFT_BUTTON);
	playerInput->RegisterGamePadInput(ActionID::WIRE_SHOOTING, InputSystem<ActionID>::GamePadButtons::RIGHT_TRIGGER);

	// ワイヤー発射の終了
	playerInput->RegisterMouseInput(ActionID::RELEASE_WIRE, InputSystem<ActionID>::MouseButtons::LEFT_BUTTON);
	playerInput->RegisterGamePadInput(ActionID::RELEASE_WIRE, InputSystem<ActionID>::GamePadButtons::RIGHT_TRIGGER);

	// ジャンプ
	playerInput->RegisterKeyboardInput(ActionID::JUMPING, Keyboard::Space);
	playerInput->RegisterGamePadInput(ActionID::JUMPING, InputSystem<ActionID>::GamePadButtons::A);
	

	// 左移動
	playerInput->RegisterKeyboardInput(ActionID::LEFT_MOVE, Keyboard::Left);
	playerInput->RegisterKeyboardInput(ActionID::LEFT_MOVE, Keyboard::A);
	playerInput->RegisterGamePadInput(ActionID::LEFT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_LEFT);

	// 右移動
	playerInput->RegisterKeyboardInput(ActionID::RIGHT_MOVE, Keyboard::Right);
	playerInput->RegisterKeyboardInput(ActionID::RIGHT_MOVE, Keyboard::D);
	playerInput->RegisterGamePadInput(ActionID::RIGHT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_RIGHT);

	// 正面移動
	playerInput->RegisterKeyboardInput(ActionID::FRONT_MOVE, Keyboard::Up);
	playerInput->RegisterKeyboardInput(ActionID::FRONT_MOVE, Keyboard::W);
	playerInput->RegisterGamePadInput(ActionID::FRONT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_UP);


	// 後ろに移動
	playerInput->RegisterKeyboardInput(ActionID::BACK_MOVE, Keyboard::Back);
	playerInput->RegisterKeyboardInput(ActionID::BACK_MOVE, Keyboard::S);
	playerInput->RegisterGamePadInput(ActionID::BACK_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_DOWN);



	return std::move(playerInput);
}


std::unique_ptr<InputSystem<InputActionType::UIActionID>> InputBindingFactory::CreateUIInput()
{
	// 動作の型
	using ActionID = InputActionType::UIActionID;

	// 作成するインプットシステム
	auto uiInput = std::make_unique<InputSystem<ActionID>>();

	// *** 入力の登録 ****
	
	// 決定
	uiInput->RegisterKeyboardInput(ActionID::CONFIRM, Keyboard::Keys::Space);
	uiInput->RegisterKeyboardInput(ActionID::CONFIRM, Keyboard::Keys::Enter);
	uiInput->RegisterGamePadInput(ActionID::CONFIRM, InputSystem<ActionID>::GamePadButtons::A);

	// キャンセル
	uiInput->RegisterKeyboardInput(ActionID::CANCEL, Keyboard::Keys::Escape);
	uiInput->RegisterGamePadInput(ActionID::CANCEL, InputSystem<ActionID>::GamePadButtons::B);

	// 左移動
	uiInput->RegisterKeyboardInput(ActionID::LEFT_MOVE	, Keyboard::Keys::Left);
	uiInput->RegisterKeyboardInput(ActionID::LEFT_MOVE	, Keyboard::Keys::A);
	uiInput->RegisterGamePadInput(ActionID::LEFT_MOVE	, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_LEFT);
	uiInput->RegisterGamePadInput(ActionID::LEFT_MOVE	, InputSystem<ActionID>::GamePadButtons::D_PAD_LEFT);

	// 右移動
	uiInput->RegisterKeyboardInput(ActionID::RIGHT_MOVE, Keyboard::Keys::Right);
	uiInput->RegisterKeyboardInput(ActionID::RIGHT_MOVE, Keyboard::Keys::D);
	uiInput->RegisterGamePadInput(ActionID::RIGHT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_RIGHT);
	uiInput->RegisterGamePadInput(ActionID::RIGHT_MOVE, InputSystem<ActionID>::GamePadButtons::D_PAD_RIGHT);


	// 上移動
	uiInput->RegisterKeyboardInput(ActionID::UP_MOVE, Keyboard::Keys::Up);
	uiInput->RegisterKeyboardInput(ActionID::UP_MOVE, Keyboard::Keys::W);
	uiInput->RegisterGamePadInput(ActionID::UP_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_UP);
	uiInput->RegisterGamePadInput(ActionID::UP_MOVE, InputSystem<ActionID>::GamePadButtons::D_PAD_UP);


	// 下移動
	uiInput->RegisterKeyboardInput(ActionID::DOWN_MOVE, Keyboard::Keys::Down);
	uiInput->RegisterKeyboardInput(ActionID::DOWN_MOVE, Keyboard::Keys::S);
	uiInput->RegisterGamePadInput(ActionID::DOWN_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_DOWN);
	uiInput->RegisterGamePadInput(ActionID::DOWN_MOVE, InputSystem<ActionID>::GamePadButtons::D_PAD_DOWN);


	return std::move(uiInput);
}


std::unique_ptr<InputSystem<InputActionType::SystemActionID>> InputBindingFactory::CreateSystemInput()
{
	// 動作の型
	using ActionID = InputActionType::SystemActionID;

	// 作成するインプットシステム
	auto systemInput = std::make_unique<InputSystem<ActionID>>();

	// ポーズ画面
	systemInput->RegisterKeyboardInput(ActionID::PAUSE, Keyboard::Keys::Escape);
	systemInput->RegisterGamePadInput(ActionID::PAUSE, InputSystem<ActionID>::GamePadButtons::START);

	// デバック
	// まだ未設定

	return systemInput;
}


