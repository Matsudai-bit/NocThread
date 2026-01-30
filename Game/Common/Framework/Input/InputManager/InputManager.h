/*****************************************************************//**
 * @file    InputManager.h
 * @brief   入力管理に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/01/30
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>


// クラスの前方宣言 ===================================================
class InputActionMap;
class InputData;
class InputOptionData;
/**
 * @brief 入力管理
 */
class InputManager
{

// データメンバ変数
private:
	std::unordered_map<std::string, std::unique_ptr<InputActionMap>> m_inputActionMaps;	///< アクションマップ群（キー：マップ名, 値：アクションマップ）

	const DirectX::Keyboard::KeyboardStateTracker*	m_pKeyboardStateTracker;	///< キーボード情報
	const DirectX::Mouse::ButtonStateTracker*		m_pMouseStateTracker;		///< マウス情報
	const DirectX::GamePad::ButtonStateTracker*		m_pGamePadStateTracker;		///< ゲームパッド情報

// コンストラクタ・デストラクタ
public:

	// コンストラクタ
	InputManager(
		const DirectX::Keyboard::KeyboardStateTracker* pKeyboardStateTracker,
		const DirectX::Mouse::ButtonStateTracker* pMouseStateTracker,
		const DirectX::GamePad::ButtonStateTracker* pGamePadStateTracker);

	// デストラクタ
	~InputManager();

// 操作
public:
	// 更新処理
	void Update();
	// アクションマップの追加
	void AddActionMap(std::unique_ptr<InputActionMap> inputActionMap);

	// アクションマップの取得
	InputActionMap* GetInputActionMap(std::string actionMapName) const { return m_inputActionMaps.at(actionMapName).get(); }

// 内部実装 ------------------------------------------------------------------------
	private:

		// アクションマップの入力確認
		void CheckInputActionMap(InputActionMap* pInputActionMap);
		// インプットデータを元に入力の確認
		bool CheckInputData(const InputData& inputData, InputOptionData* pOption);

		// キーボード入力の確認
		bool CheckInputKeyboard(const InputData& inputData, InputOptionData* pInputOption);
		// マウス入力の確認
		bool CheckInputMouse(const InputData& inputData, InputOptionData* pInputOption);
		// ゲームパッド入力の確認
		bool CheckInputGamePad(const InputData& inputData, InputOptionData* pInputOption);
		


};
