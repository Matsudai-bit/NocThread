#pragma once


// 操舵パラメータ
class SteeringParameters
{
public:
	// 最大移動速度(1 秒あたりのドット数)
	static const float MAX_SPEED;
	// 適用される最大力
	static const float MAX_FORCE;
	// 最大回転率（ラジアン/秒）
	static const float MAX_TURN_RATE;
	// 操舵力に掛ける調整用のパラメータ
	static const float STEERING_FORCE_TWEAKER;
	// 「到着」行動の調整用のウエイト
	static const float ARRIVE_WEIGHT;
	// 「追跡」行動の調整用のウエイト
	static const float PURSUIT_WEIGHT;
};

