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
 * @brief 組み立てる
 * 
 * @param[in] instance インスタンスのポインタ
 * @param[in] desc 　　生成設定データ
 */
void InputBindingFactory::PlayerInputFactory::Assemble(InputSystem<InputActionType::PlyayerActionID>* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);

	// 動作の型
	using ActionID = InputActionType::PlyayerActionID;


	// *** 入力の登録 ****
	// ステップ
	instance->RegisterMouseInput(ActionID::STEPPING, InputSystem<ActionID>::MouseButtons::RIGHT_BUTTON);
	instance->RegisterGamePadInput(ActionID::STEPPING, InputSystem<ActionID>::GamePadButtons::A);

	// ワイヤー発射
	instance->RegisterMouseInput(ActionID::WIRE_SHOOTING, InputSystem<ActionID>::MouseButtons::LEFT_BUTTON);
	instance->RegisterGamePadInput(ActionID::WIRE_SHOOTING, InputSystem<ActionID>::GamePadButtons::RIGHT_TRIGGER);

	// ワイヤー発射の終了
	instance->RegisterMouseInput(ActionID::RELEASE_WIRE, InputSystem<ActionID>::MouseButtons::LEFT_BUTTON);
	instance->RegisterGamePadInput(ActionID::RELEASE_WIRE, InputSystem<ActionID>::GamePadButtons::RIGHT_TRIGGER);

	// ジャンプ
	instance->RegisterKeyboardInput(ActionID::JUMPING, Keyboard::Space);
	instance->RegisterGamePadInput(ActionID::JUMPING, InputSystem<ActionID>::GamePadButtons::A);


	// 左移動
	instance->RegisterKeyboardInput(ActionID::LEFT_MOVE, Keyboard::Left);
	instance->RegisterKeyboardInput(ActionID::LEFT_MOVE, Keyboard::A);
	instance->RegisterGamePadInput(ActionID::LEFT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_LEFT);

	// 右移動
	instance->RegisterKeyboardInput(ActionID::RIGHT_MOVE, Keyboard::Right);
	instance->RegisterKeyboardInput(ActionID::RIGHT_MOVE, Keyboard::D);
	instance->RegisterGamePadInput(ActionID::RIGHT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_RIGHT);

	// 正面移動
	instance->RegisterKeyboardInput(ActionID::FRONT_MOVE, Keyboard::Up);
	instance->RegisterKeyboardInput(ActionID::FRONT_MOVE, Keyboard::W);
	instance->RegisterGamePadInput(ActionID::FRONT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_UP);


	// 後ろに移動
	instance->RegisterKeyboardInput(ActionID::BACK_MOVE, Keyboard::Back);
	instance->RegisterKeyboardInput(ActionID::BACK_MOVE, Keyboard::S);
	instance->RegisterGamePadInput(ActionID::BACK_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_DOWN);
}

/**
 * @brief 組み立てる
 *
 * @param[in] instance インスタンスのポインタ
 * @param[in] desc 　　生成設定データ
 */
void InputBindingFactory::UIInputFactory::Assemble(InputSystem<InputActionType::UIActionID>* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);
	// 動作の型
	using ActionID = InputActionType::UIActionID;
	// *** 入力の登録 ****
	// 決定
	instance->RegisterKeyboardInput	(ActionID::CONFIRM, Keyboard::Keys::Space);
	instance->RegisterKeyboardInput	(ActionID::CONFIRM, Keyboard::Keys::Enter);
	instance->RegisterGamePadInput	(ActionID::CONFIRM, InputSystem<ActionID>::GamePadButtons::A);
	instance->RegisterMouseInput	(ActionID::CONFIRM, InputSystem<ActionID>::MouseButtons::LEFT_BUTTON);
	// キャンセル
	instance->RegisterKeyboardInput(ActionID::CANCEL, Keyboard::Keys::Escape);
	instance->RegisterKeyboardInput(ActionID::CANCEL, Keyboard::Keys::C);
	instance->RegisterGamePadInput(ActionID::CANCEL, InputSystem<ActionID>::GamePadButtons::B);
	// 左移動
	instance->RegisterKeyboardInput(ActionID::LEFT_MOVE, Keyboard::Keys::Left);
	instance->RegisterKeyboardInput(ActionID::LEFT_MOVE, Keyboard::Keys::A);
	instance->RegisterGamePadInput(ActionID::LEFT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_LEFT);
	instance->RegisterGamePadInput(ActionID::LEFT_MOVE, InputSystem<ActionID>::GamePadButtons::D_PAD_LEFT);
	// 右移動
	instance->RegisterKeyboardInput(ActionID::RIGHT_MOVE, Keyboard::Keys::Right);
	instance->RegisterKeyboardInput(ActionID::RIGHT_MOVE, Keyboard::Keys::D);
	instance->RegisterGamePadInput(ActionID::RIGHT_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_RIGHT);
	instance->RegisterGamePadInput(ActionID::RIGHT_MOVE, InputSystem<ActionID>::GamePadButtons::D_PAD_RIGHT);
	// 上移動
	instance->RegisterKeyboardInput(ActionID::UP_MOVE, Keyboard::Keys::Up);
	instance->RegisterKeyboardInput(ActionID::UP_MOVE, Keyboard::Keys::W);
	instance->RegisterGamePadInput(ActionID::UP_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_UP);
	instance->RegisterGamePadInput(ActionID::UP_MOVE, InputSystem<ActionID>::GamePadButtons::D_PAD_UP);
	// 下移動
	instance->RegisterKeyboardInput(ActionID::DOWN_MOVE, Keyboard::Keys::Down);
	instance->RegisterKeyboardInput(ActionID::DOWN_MOVE, Keyboard::Keys::S);
	instance->RegisterGamePadInput(ActionID::DOWN_MOVE, InputSystem<ActionID>::GamePadButtons::LEFT_STICK_DOWN);
	instance->RegisterGamePadInput(ActionID::DOWN_MOVE, InputSystem<ActionID>::GamePadButtons::D_PAD_DOWN);

}

/**
 * @brief 組み立てる
 *
 * @param[in] instance インスタンスのポインタ
 * @param[in] desc 　　生成設定データ
 */
void InputBindingFactory::SystemInputFactory::Assemble(InputSystem<InputActionType::SystemActionID>* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);
	// 動作の型
	using ActionID = InputActionType::SystemActionID;

	// ポーズ画面
	instance->RegisterKeyboardInput(ActionID::PAUSE, Keyboard::Keys::Escape);
	instance->RegisterGamePadInput(ActionID::PAUSE, InputSystem<ActionID>::GamePadButtons::START);

}

void InputActionMapFactory::PlayerInputMapFactory::Assemble(InputActionMap* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);

	using namespace InputActionID;
	using namespace Input;

	// マップ名の設定
	instance->SetActionMapName(Player::MAP_NAME);
	
	// ステップ
	{
		instance->AddAction(Player::STEPPING, { {}, {}, {GamePadButtons::A} });
		instance->AddAction(Player::STEPPING, { {}, {MouseButtons::RIGHT_BUTTON}, {} });
	}
	// ワイヤー発射
	{
		instance->AddAction(Player::WIRE_SHOOTING , { {}, {MouseButtons::LEFT_BUTTON}, {} });
		instance->AddAction(Player::WIRE_SHOOTING, { {}, {}, {GamePadButtons::RIGHT_TRIGGER} });
	}
	// ワイヤー発射の終了
	{
		instance->AddAction(Player::RELEASE_WIRE, { {}, {MouseButtons::LEFT_BUTTON}, {} });
		instance->AddAction(Player::RELEASE_WIRE, { {}, {}, {GamePadButtons::RIGHT_TRIGGER} });
	}
	// ジャンプ
	{
		instance->AddAction(Player::JUMPING, { {Keyboard::Space}, {}, {} });
		instance->AddAction(Player::JUMPING, { {}, {}, {GamePadButtons::A} });
	}
	// 左移動
	{
		instance->AddAction(Player::LEFT_MOVE, { {Keyboard::Left}, {}, {} });
		instance->AddAction(Player::LEFT_MOVE, { {Keyboard::A}, {}, {} });
		instance->AddAction(Player::LEFT_MOVE, { {}, {}, {GamePadButtons::LEFT_STICK_LEFT} });
	}

	// 右移動
	{
		instance->AddAction(Player::RIGHT_MOVE, { {Keyboard::Right}, {}, {} });
		instance->AddAction(Player::RIGHT_MOVE, { {Keyboard::D}, {}, {} });
		instance->AddAction(Player::RIGHT_MOVE, { {}, {}, {GamePadButtons::LEFT_STICK_RIGHT} });
	}

	// 正面移動
	{
		instance->AddAction(Player::FRONT_MOVE, { {Keyboard::Up}, {}, {} });
		instance->AddAction(Player::FRONT_MOVE, { {Keyboard::W}, {}, {} });
		instance->AddAction(Player::FRONT_MOVE, { {}, {}, {GamePadButtons::LEFT_STICK_UP} });
	}

	// 後ろに移動
	{
		instance->AddAction(Player::BACK_MOVE, { {Keyboard::Back}, {}, {} });
		instance->AddAction(Player::BACK_MOVE, { {Keyboard::S}, {}, {} });
		instance->AddAction(Player::BACK_MOVE, { {}, {}, {GamePadButtons::LEFT_STICK_DOWN} });
	}
}

void InputActionMapFactory::UIInputMapFactory::Assemble(InputActionMap* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);

	using namespace InputActionID;
	using namespace Input;

	// マップ名の設定
	instance->SetActionMapName(UI::MAP_NAME);

	// 決定
	{
		instance->AddAction(UI::CONFIRM, { {Keyboard::Keys::Space}, {}, {} });
		instance->AddAction(UI::CONFIRM, { {Keyboard::Keys::Enter}, {}, {} });
		instance->AddAction(UI::CONFIRM, { {}, {}, {GamePadButtons::A} });
		instance->AddAction(UI::CONFIRM, { {}, {MouseButtons::LEFT_BUTTON}, {} });
	}

	// キャンセル
	{
		instance->AddAction(UI::CANCEL, { {Keyboard::Keys::Escape}, {}, {} });
		instance->AddAction(UI::CANCEL, { {Keyboard::Keys::C}, {}, {} });
		instance->AddAction(UI::CANCEL, { {}, {}, {GamePadButtons::B} });
	}

	// 左移動
	{
		instance->AddAction(UI::LEFT_MOVE, { {Keyboard::Keys::Left}, {}, {} });
		instance->AddAction(UI::LEFT_MOVE, { {Keyboard::Keys::A}, {}, {} });
		instance->AddAction(UI::LEFT_MOVE, { {}, {}, {GamePadButtons::LEFT_STICK_LEFT} });
		instance->AddAction(UI::LEFT_MOVE, { {}, {}, {GamePadButtons::D_PAD_LEFT} });
	}

	// 右移動
	{
		instance->AddAction(UI::RIGHT_MOVE, { {Keyboard::Keys::Right}, {}, {} });
		instance->AddAction(UI::RIGHT_MOVE, { {Keyboard::Keys::D}, {}, {} });
		instance->AddAction(UI::RIGHT_MOVE, { {}, {}, {GamePadButtons::LEFT_STICK_RIGHT} });
		instance->AddAction(UI::RIGHT_MOVE, { {}, {}, {GamePadButtons::D_PAD_RIGHT} });
	}

	// 上移動
	{
		instance->AddAction(UI::UP_MOVE, { {Keyboard::Keys::Up}, {}, {} });
		instance->AddAction(UI::UP_MOVE, { {Keyboard::Keys::W}, {}, {} });
		instance->AddAction(UI::UP_MOVE, { {}, {}, {GamePadButtons::LEFT_STICK_UP} });
		instance->AddAction(UI::UP_MOVE, { {}, {}, {GamePadButtons::D_PAD_UP} });
	}

	// 下移動
	{
		instance->AddAction(UI::DOWN_MOVE, { {Keyboard::Keys::Down}, {}, {} });
		instance->AddAction(UI::DOWN_MOVE, { {Keyboard::Keys::S}, {}, {} });
		instance->AddAction(UI::DOWN_MOVE, { {}, {}, {GamePadButtons::LEFT_STICK_DOWN} });
		instance->AddAction(UI::DOWN_MOVE, { {}, {}, {GamePadButtons::D_PAD_DOWN} });
	}
}

void InputActionMapFactory::SystemInputMapFactory::Assemble(InputActionMap* instance, const DefaultSpawnDesc& desc)
{
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(desc);

	using namespace InputActionID;
	using namespace Input;

	// マップ名の設定
	instance->SetActionMapName(System::MAP_NAME);

	// ポーズ画面
	{
		instance->AddAction(System::PAUSE, { {Keyboard::Keys::Escape}, {}, {} });
		instance->AddAction(System::PAUSE, { {}, {}, {GamePadButtons::START} });
	}

	// （参考）デバッグモード切替なども同様に追加可能です
	{
		// instance->AddAction(System::CHANGE_DEBUG_MODE, { {Keyboard::Keys::F1}, {}, {} });
	}
}
