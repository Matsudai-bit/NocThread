/*****************************************************************//**
 * @file    InputBindingFactory.h
 * @brief   入力と動作の紐づけを作成するファクトリーに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/22
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include "Game/Common/Input/InputSystem/InputSystem.h"
#include "Game/Common/Input/InputActionType/InputActionType.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 入力と動作の紐づけを作成するファクトリー
 */
class InputBindingFactory
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:



// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	InputBindingFactory();

	// デストラクタ
	~InputBindingFactory();


// 操作
public:

  static std::unique_ptr<InputSystem<InputActionType::PlyayerActionID>> CreatePlayerInput();

  static std::unique_ptr<InputSystem<InputActionType::UIActionID>> CreateUIInput();

  static std::unique_ptr<InputSystem<InputActionType::SystemActionID>> CreateSystemInput();
	

};
