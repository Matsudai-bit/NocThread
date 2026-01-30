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

namespace InputActionID {

	/**
	 * @brief プレイヤーアクションの識別ID
	 */
	struct Player {
		static constexpr const char* MAP_NAME = "player";

		static constexpr const char* JUMPING		= "jumping";
		static constexpr const char* FRONT_MOVE		= "frontMove";
		static constexpr const char* RIGHT_MOVE		= "rightMove";
		static constexpr const char* LEFT_MOVE		= "leftMove";
		static constexpr const char* BACK_MOVE		= "backMove";
		static constexpr const char* WIRE_SHOOTING	= "wireShooting";
		static constexpr const char* RELEASE_WIRE	= "releaseWire";
		static constexpr const char* STEPPING		= "stepping";
	};

	/**
	 * @brief UI操作の識別ID
	 */
	struct UI {
		static constexpr const char* MAP_NAME = "ui";


		static constexpr const char* CONFIRM	= "confirm";
		static constexpr const char* CANCEL		= "cancel";
		static constexpr const char* UP_MOVE	= "upMove";
		static constexpr const char* DOWN_MOVE	= "downMove";
		static constexpr const char* LEFT_MOVE	= "leftMove";
		static constexpr const char* RIGHT_MOVE = "rightMove";
	};

	/**
	 * @brief システム操作の識別ID
	 */
	struct System {
		static constexpr const char* MAP_NAME = "system";


		static constexpr const char* PAUSE				= "pause";
		static constexpr const char* CHANGE_DEBUG_MODE	= "changeDebugMode";
	};

}

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
