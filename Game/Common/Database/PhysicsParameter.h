/*****************************************************************//**
 * @file    PhysicsParameter.h
 * @brief   物理パラメーター群
 * 
 * @author  松下大暉
 * @date    2025/1/27
 *********************************************************************/
#pragma once

#include <string>
#include <SimpleMath.h>



namespace PhysicsParameter
{
	static constexpr float GRAVITY_SCALE = 25.0f;						// 重力量(s)
	static constexpr DirectX::SimpleMath::Vector3 GRAVITY_ACCELERATION =
		DirectX::SimpleMath::Vector3(0.0f, -GRAVITY_SCALE, 0.0f);		// 重力加速度

    
}