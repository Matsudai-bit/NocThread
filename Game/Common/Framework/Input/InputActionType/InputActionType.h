/*****************************************************************//**
 * @file    InputActionType.h
 * @brief   入力アクションの種類に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/22
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

namespace InputActionType
{
	/**
	 * @brief プレイヤーアクションの識別番号
	 */
	enum class PlyayerActionID
	{
		JUMPING,
		FRONT_MOVE,
		RIGHT_MOVE,
		LEFT_MOVE,
		BACK_MOVE,
		WIRE_SHOOTING,
		RELEASE_WIRE,
		STEPPING,
	};

	/**
	 * @brief UIへの動作の識別番号
	 */
	enum class UIActionID
	{
		CONFIRM,	// 決定
		CANCEL,		// キャンセル
		UP_MOVE,	// 移動 (上)
		DOWN_MOVE,	// 移動 (下)
		LEFT_MOVE,	// 移動 (左)
		RIGHT_MOVE,	// 移動 (右)
	};

	/**
	 * @brief ゲームシステムへの動作の識別番号
	 */
	enum class SystemActionID
	{
		PAUSE,					// ポーズ画面
		CHANGE_DEBUG_MODE,		// デバックモード
	
	};
}
