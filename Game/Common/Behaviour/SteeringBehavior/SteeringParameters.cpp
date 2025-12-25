#include "pch.h"
#include "SteeringParameters.h"

// 最大移動速度(1 秒あたりのドット数)
const float SteeringParameters::MAX_SPEED = 100.0f;
// 操舵力に掛ける調整用のパラメータ
const float SteeringParameters::STEERING_FORCE_TWEAKER = 3.0f;
// 適用される最大力
const float SteeringParameters::MAX_FORCE = STEERING_FORCE_TWEAKER;
// 最大回転率（ラジアン/秒）
const float SteeringParameters::MAX_TURN_RATE = 1.3f;
// 「到着」行動の調整用ウエイト
const float SteeringParameters::ARRIVE_WEIGHT = STEERING_FORCE_TWEAKER * .01f;
// 「追跡」行動の調整用ウエイト
const float SteeringParameters::PURSUIT_WEIGHT = STEERING_FORCE_TWEAKER * 20.0f;


