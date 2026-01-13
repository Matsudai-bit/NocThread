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
#include "Game/Common/SoundManager/SoundManager.h"




namespace SoundDatabase
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
	static const std::unordered_map<std::string, SoundManager::SoundClip> SOUND_CLIP_MAP =
	{
		// UI
		{ SE_DECIDE,					{ SoundManager::SoundType::SE, SE_DECIDE, 1.0f }},
		{ SE_CANCEL,					{ SoundManager::SoundType::SE, SE_CANCEL, 0.4f  }},
		{ SE_BUTTON_CLICK_GAMESTART,	{ SoundManager::SoundType::SE, SE_BUTTON_CLICK_GAMESTART, 1.0f }},
		{ SE_CURSOR_MOVING,				{ SoundManager::SoundType::SE, SE_CURSOR_MOVING, 1.0f }},

		// プレイヤー関連
		{ SE_PLAYER_JUMP,			{ SoundManager::SoundType::SE, SE_PLAYER_JUMP, 0.4f }},
		{ SE_PLAYER_PASSING,		{ SoundManager::SoundType::SE, SE_PLAYER_PASSING, 1.5f}},
		{ SE_PLAYER_SHOOTINGWIRE,	{ SoundManager::SoundType::SE, SE_PLAYER_SHOOTINGWIRE, 1.0f }},

		// BGM
		{ BGM_TITLE,	{ SoundManager::SoundType::BGM, BGM_TITLE, 0.3f }},
		{ BGM_INGAME,	{ SoundManager::SoundType::BGM, BGM_INGAME, 0.3f }},
		{ BGM_RESULT,	{ SoundManager::SoundType::BGM, BGM_RESULT, 0.3f }}

	};
}