/*****************************************************************//**
 * @file    SoundPaths.h
 * @brief   サウンドパス群
 * 
 * @author  松下大暉
 * @date    2025/9/15
 *********************************************************************/
#pragma once

#include <string>

namespace SoundPaths
{
    // 効果音（SFX）

    // UI
	static const std::string SE_DECIDE                  = "SE/se_ui_decide.wav";
	static const std::string SE_CANCEL                  = "SE/se_ui_cancel.wav";
	static const std::string SE_BUTTON_CLICK_GAMESTART  = "SE/se_ui_gamestart.wav";
	static const std::string SE_CURSOR_MOVING           = "SE/se_ui_cursor_move.wav";

	// プレイヤー関連
	static const std::string SE_PLAYER_JUMP = "SE/se_player_jumping.wav";
	static const std::string SE_PLAYER_PASSING = "SE/se_player_passing.wav";
	static const std::string SE_PLAYER_SHOOTINGWIRE = "SE/se_player_shootingWire.wav";

    // BGM
    static const std::string BGM_TITLE = "BGM/bgm_title.wav";
    static const std::string BGM_INGAME = "BGM/bgm_gameplay.wav";
    static const std::string BGM_RESULT = "BGM/bgm_result.wav";

    // その他
}
