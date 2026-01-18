/*****************************************************************//**
 * @file    GameFlowEventID.h
 * @brief   ゲーム進行イベントの識別子に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once

/**
 * @brief ゲーム進行イベントの識別子
 */
enum class GameFlowEventID
{

	GAME_SETUP_FINISH,	// ゲーム準備完了
	GAME_TRANSITION_FADING_START, // フェード中
	GAME_TRANSITION_FADING_FINISH,		// 無し
	GAME_START,			// ゲーム開始
	GAME_END,			// ゲーム終了

	STOLE_TREASURE,		// 宝物を盗んだ
	SPAWN_HELICOPTER,	// ヘリコプターの出現
	CHECKPOINT_PASSED,	// チェックポイントを通過した
	
	PLAYER_DIE,			// プレイヤーの死亡
	ESCAPE_SUCCESS,		// 脱出成功
};




