/*****************************************************************//**
 * @file    TextureDatabase.h
 * @brief   テクスチャ群
 * 
 * @author  松下大暉
 * @date    2025/01/13
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
		TEXTURE_FLOOR,					// 床テクスチャ

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
		UI_TUTORIAL_WINDOW_8,

		// ロード中関連
		BACKGROUND_LOADING,	// ロード中背景
		UI_FONT_LOADING,	// ロード中フォント
		UI_ANIMATION_CHARACTER, // ロード中アニメーションキャラクター
			
    };

	static const std::unordered_map<TextureID, std::string> TEXTURE_PATH_MAP =
	{
		// 共通
		{ TextureID::UI_NUMBER_THORN,				"Number/number_thorn.dds" },
		{ TextureID::UI_NUMBER_THORN_PUNCH,			"Number/number_thorn_punch.dds" },
		// 操作ガイド関連
		{ TextureID::UI_GUIDE_INGAME_GAMEPAD,			"Guide/ui_guide_ingame_gamepad.dds" },
		{ TextureID::UI_GUIDE_INGAME_KEYBOARD,			"Guide/ui_guide_ingame_keyboard.dds" },
		{ TextureID::UI_GUIDE_UI_GAMEPAD,				"Guide/ui_guide_ui_gamepad.dds" },
		{ TextureID::UI_GUIDE_UI_KEYBOARD,				"Guide/ui_guide_ui_keyboard.dds" },
		{ TextureID::UI_GUIDE_GAME_OPERATING_GAMEPAD,	"Guide/ui_guide_game_operating_gamepad.dds" },
		{ TextureID::UI_GUIDE_GAME_OPERATING_KEYBOARD,	"Guide/ui_guide_game_operating_keyboard.dds" },
		// ポーズ関連
		{ TextureID::BACKGROUND_PAUSE_ALPHA_MASK,	"Pause/pause_background_alpha.dds" },
		{ TextureID::UI_PAUSE_FONT_PAUSE,			"Pause/pause_font_pause.dds" },
		{ TextureID::UI_PAUSE_FONT_CONTINUE,		"Pause/pause_font_continue.dds" },
		{ TextureID::UI_PAUSE_FONT_RETURNTILTE,		"Pause/pause_font_returnTitle.dds" },
		{ TextureID::UI_PAUSE_FONT_SETTING,			"Pause/pause_font_setting.dds" },
		{ TextureID::UI_PAUSE_FONT_TUTORIAL,		"Pause/pause_font_tutorial.dds" },
		{ TextureID::UI_PAUSE_FONT_OPERATING, 		"Pause/pause_font_operating.dds" },
		// ゲームプレイ関連
		{ TextureID::UI_INGAME_MINIMAP_PLAYER_ICON, 	"InGame/inGame_minimap_player_icon.dds" },
		{ TextureID::TEXTURE_FLOOR,					"InGame/texture_floor.dds" },
		// タイトル関連
		{ TextureID::UI_TITLE_FONT_PLAY,			"Title/title_font_play.dds" },
		{ TextureID::UI_TITLE_FONT_TUTORIAL,		"Title/title_font_tutorial.dds" },
		{ TextureID::UI_TITLE_FONT_SETTING,			"Title/title_font_setting.dds" },
		{ TextureID::UI_TITLE_FONT_QUIT,			"Title/title_font_quit.dds" },
		{ TextureID::BACKGROUND_TITLE_ALPHA_MASK,	"Title/title_background_alpha.dds" },
		{ TextureID::BACKGROUND_TITLE,				"Title/title_background.dds" },
		{ TextureID::BACKGROUND_TITLE_LOGO,			"Title/title_logo.dds" },
		// リザルト関連
		{ TextureID::BACKGROUND_RESULT_ALPHA_MASK,	"Result/result_background_alpha.dds" },
		{ TextureID::BACKGROUND_RESULT,				"Result/result_background.dds" },
		{ TextureID::UI_FONT_DESTROYED_ENEMIES,		"Result/result_font_destroyed_enemies.dds" },
		{ TextureID::UI_FONT_FAILURE_JAPANESE,		"Result/result_font_failure_japanese.dds" },
		{ TextureID::UI_FONT_FAILURE_ENGLISH,		"Result/result_font_failure_english.dds" },
		{ TextureID::UI_FONT_SUCCESS_JAPANESE,		"Result/result_font_success_japanese.dds" },
		{ TextureID::UI_FONT_SUCCESS_ENGLISH,		"Result/result_font_success_english.dds" },
		{ TextureID::UI_FONT_FAILURE_SURVIVAL_TIME,	"Result/result_font_failure_survivalTime.dds" },
		{ TextureID::UI_FONT_SUCCESS_CLEAR_TIME,	"Result/result_font_success_clearTime.dds" },
		// チュートリアルウィンドウ関連
		{ TextureID::BACKGROUND_ALPHA_MASK,			"Tutorial/tutorial_alpha.dds" },
		{ TextureID::UI_TUTORIAL_ARROW,				"Tutorial/tutorial_arrow.dds" },
		{ TextureID::UI_TUTORIAL_WINDOW_1,			"Tutorial/tutorial_gameFlow_1.dds" },
		{ TextureID::UI_TUTORIAL_WINDOW_2,			"Tutorial/tutorial_gameFlow_2.dds" },
		{ TextureID::UI_TUTORIAL_WINDOW_3,			"Tutorial/tutorial_gameFlow_3.dds" },
		{ TextureID::UI_TUTORIAL_WINDOW_4,			"Tutorial/tutorial_gameFlow_4.dds" },
		{ TextureID::UI_TUTORIAL_WINDOW_5,			"Tutorial/tutorial_gameFlow_5.dds" },
		{ TextureID::UI_TUTORIAL_WINDOW_6,			"Tutorial/tutorial_gameFlow_6.dds" },
		{ TextureID::UI_TUTORIAL_WINDOW_7,			"Tutorial/tutorial_gameFlow_7.dds" },
		{ TextureID::UI_TUTORIAL_WINDOW_8,			"Tutorial/tutorial_gameFlow_8.dds" },

		// ロード中関連
		{ TextureID::BACKGROUND_LOADING,	"Loading/loading_background.dds" },
		{ TextureID::UI_FONT_LOADING,	"Loading/loading_font_loading.dds" },
		{ TextureID::UI_ANIMATION_CHARACTER, "Loading/loading_animation_character.dds" },
		
	};

    
}