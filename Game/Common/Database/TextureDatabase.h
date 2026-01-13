/*****************************************************************//**
 * @file    SoundDatabase.h
 * @brief   サウンドパス群
 * 
 * @author  松下大暉
 * @date    2025/9/15
 *********************************************************************/
#pragma once

#include <string>
#include <unordered_map>




namespace TextureDatabase
{
    enum class TextureID
    {
		// 共通
		UI_NUMBER_THORN,		// 数字
		UI_NUMBER_THORN_PUNCH,	// 「:」記号

        // 操作ガイド関連
		UI_GUIDE_INGAME_GAMEPAD,			// インゲームの操作ガイド（ゲームパッド）
		UI_GUIDE_INGAME_KEYBOARD,			// インゲームの操作ガイド（キーボード）
		UI_GUIDE_UI_GAMEPAD,				// UIの操作ガイド（ゲームパッド）
		UI_GUIDE_UI_KEYBOARD,				// UIの操作ガイド（キーボード）
		UI_GUIDE_GAME_OPERATING_GAMEPAD,	// ゲーム操作ガイド（ゲームパッド）
		UI_GUIDE_GAME_OPERATING_KEYBOARD,	// ゲーム操作ガイド（キーボード）

		// ポーズ関連
		BACKGROUND_PAUSE_ALPHA_MASK,	// ポーズ背景アルファマスク
		UI_PAUSE_FONT_PAUSE,			// 「PAUSE」フォント
		UI_PAUSE_FONT_CONTINUE,			// 「CONTINUE」フォント/
		UI_PAUSE_FONT_RETURNTILTE,		// 「RETURN TO TITLE」フォント
		UI_PAUSE_FONT_SETTING,			// 「SETTING」フォント
		UI_PAUSE_FONT_TUTORIAL,			// 「TUTORIAL」フォント

		UI_PAUSE_FONT_OPERATING, 		// 「操作方法」フォント

		// ゲームプレイ関連
		UI_INGAME_MINIMAP_PLAYER_ICON, 	// ミニマップのプレイヤーアイコン


		// タイトル関連
		UI_TITLE_FONT_PLAY,			// 「PLAY」フォント
		UI_TITLE_FONT_TUTORIAL,		// 「TUTORIAL」フォント
		UI_TITLE_FONT_SETTING,		// 「SETTING」フォント
		UI_TITLE_FONT_QUIT,			// 「QUIT」フォント
		BACKGROUND_TITLE_ALPHA_MASK,// タイトル背景アルファマスク
		BACKGROUND_TITLE,			// タイトル背景
		BACKGROUND_TITLE_LOGO,		// タイトルロゴ

		// リザルト関連
		BACKGROUND_RESULT_ALPHA_MASK,	// リザルト背景アルファマスク
		BACKGROUND_RESULT,				// リザルト背景
		UI_FONT_DESTROYED_ENEMIES,		// 「倒した敵数」数フォント
		UI_FONT_FAILURE_JAPANESE,		// 「失敗」フォント（日本語）
		UI_FONT_FAILURE_ENGLISH,		// 「FAILURE」フォント（英語）
		UI_FONT_SUCCESS_JAPANESE,		// 「成功」フォント（日本語）
		UI_FONT_SUCCESS_ENGLISH,		// 「SUCCESS」フォント（英語）
		UI_FONT_FAILURE_SURVIVAL_TIME,	// 「生存時間」数フォント（失敗時）
		UI_FONT_SUCCESS_CLEAR_TIME,		// 「クリアタイム」数フォント（成功時）

		// チュートリアルウィンドウ関連
		BACKGROUND_ALPHA_MASK,			// チュートリアル背景アルファマスク
		UI_TUTORIAL_ARROW,				// チュートリアル矢印
		// チュートリアルウィンドウ各種
		UI_TUTORIAL_WINDOW_1,
		UI_TUTORIAL_WINDOW_2,
		UI_TUTORIAL_WINDOW_3,
		UI_TUTORIAL_WINDOW_4,
		UI_TUTORIAL_WINDOW_5,
		UI_TUTORIAL_WINDOW_6,
		UI_TUTORIAL_WINDOW_7,
			
    };

	static const std::unordered_multimap<TextureID, std::string> TEXTURE_PATH_MAP =
	{

	};

    
}