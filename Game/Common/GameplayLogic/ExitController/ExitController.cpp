/*****************************************************************//**
 * @file    ExitController.cpp
 * @brief   終了操作に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/01/23
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "ExitController.h"


// 静的メンバ変数の定義
bool ExitController::m_isExit = false;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ExitController::ExitController()
{

}



/**
 * @brief デストラクタ
 */
ExitController::~ExitController()
{

}

/**
 * @brief インスタンス取得
 * 
 * @return 
 */
ExitController& ExitController::GetInstance()
{
	static ExitController instance;
	return instance;
}

/**
 * @brief 終了するかどうか
 * 
 * @return 
 */
bool ExitController::IsExit()
{
	return m_isExit;
}

/**
 * @brief 終了する
 * 
 */
void ExitController::ExitGame()
{
	m_isExit = true;
}



