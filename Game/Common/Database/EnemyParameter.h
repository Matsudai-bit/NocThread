/*****************************************************************//**
 * @file    EnemyParameter.h
 * @brief   敵パラメーター群
 * 
 * @author  松下大暉
 * @date    2025/01/27
 *********************************************************************/
#pragma once

#include <string>

namespace EnemyParameter
{
	// 敵モデルのファイル名
	static const constexpr char* PLAYER_MODEL_FILE_NAME = "player.sdkmesh";

	static constexpr float FRICTION = 3.0f;			// 摩擦係数
	static constexpr float AIR_RESISTANCE = 0.05f;	// 空気抵抗係数
	static constexpr float ROTATION_SPEED = 1.0f;	// 回転速度(s)
}