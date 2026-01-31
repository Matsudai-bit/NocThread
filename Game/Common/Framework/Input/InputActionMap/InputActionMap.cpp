/*****************************************************************//**
 * @file    InputActionMap.cpp
 * @brief   入力アクションマップに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/01/30
 *********************************************************************/
// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "InputActionMap.h"



// ライブラリ

// ファクトリー関連

// データベース関連

// フレームワーク関連

// ゲームプレイロジック関連

// グラフィック関連


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
InputActionMap::InputActionMap()
{

}



/**
 * @brief デストラクタ
 */
InputActionMap::~InputActionMap()
{

}

void InputActionMap::AddAction(std::string actionName, InputData inputData)
{
	m_inputMap[actionName].inputs.push_back(inputData);
}

void InputActionMap::BindInputEvent(std::string actionName, void* owner, std::function<void(InputEventData)> callback)
{

	m_inputMap[actionName].callbacks.push_back(CallbackEntry(owner, callback));

}


void InputActionMap::UnBindAllInputEvent(const std::string& actionName, void* owner)
{
	auto& callbacks = m_inputMap[actionName].callbacks;
	callbacks.erase(
		std::remove_if(
			callbacks.begin(), callbacks.end(), [&](const CallbackEntry& entryData)  {return entryData.owner == owner; }), 
		callbacks.end());
}

std::unordered_map<std::string, ActionData>& InputActionMap::GetActionMap()
{
	return m_inputMap;
}
