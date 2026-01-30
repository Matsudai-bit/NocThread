/*****************************************************************//**
 * @file    InputActionMap.h
 * @brief   入力アクションマップに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/01/30
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include <string>
#include <functional>



// クラスの前方宣言 ===================================================
namespace Input
{
	enum class Option;
	enum class MouseButtons;
	enum class GamePadButtons;
}


struct InputData;
struct InputOptionData;
struct InputEventData;
struct ActionData;



// クラスの定義 ===============================================================
/**
 * @brief 入力アクションマップ
 */
class InputActionMap
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:



// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	InputActionMap();

	// デストラクタ
	~InputActionMap();

// 操作
public:
	std::string m_mapName;

	std::unordered_map<std::string, ActionData> m_inputMap;	///< 入力情報

public:
	void AddAction(std::string actionName, InputData  inputData);

	void BindInputEvent(std::string actionName, void* owner, std::function<void(InputEventData)> callback);

	// 自分が登録したアクションをすべて解除する
	void UnBindAllInputEvent(const std::string& actionName, void* owner);
	
	std::unordered_map<std::string, ActionData>& GetActionMap() ;

	const std::string& GetActionMapName() const { return m_mapName; }
	void SetActionMapName(const std::string& name) { m_mapName = name; }

};

namespace Input
{

	enum class Option
	{
		DOWN,		// 押している
		PRESSED,	// 押した瞬間
		RELEASED,	// 離した瞬間
		UP			// 押していない
	};

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
}



struct InputData
{
	std::vector<DirectX::Keyboard::Keys> keys; // 判定キー
	std::vector<Input::MouseButtons> mouseButtons;
	std::vector<Input::GamePadButtons> gamePadButtons;
};

struct InputOptionData
{
	bool pressed;
	bool released;
};

struct InputEventData
{
	std::string actionMapName;	// アクションマップの名前
	std::string actionName;		// アクションの名前
	InputOptionData inputOption;
};

struct CallbackEntry 
{
	void* owner; // 誰が登録したか (thisポインタ)
	std::function<void(InputEventData)> callback;

	CallbackEntry(void* owner, std::function<void(InputEventData)> callback)
		: owner{ owner }
		, callback{ callback }
	{	}
};

struct ActionData
{
	bool isInput;
	InputOptionData inputOption;

	std::vector<InputData> inputs;
	std::vector<CallbackEntry> callbacks;
};