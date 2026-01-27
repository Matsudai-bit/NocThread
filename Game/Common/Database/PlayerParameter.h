/*****************************************************************//**
 * @file    PlayerParameter.h
 * @brief   プレイヤーパラメーター群
 * 
 * @author  松下大暉
 * @date    2025/9/15
 *********************************************************************/
#pragma once

#include <string>




namespace PlayerParameter
{
	// プレイヤーモデルのファイル名
	static const constexpr char* PLAYER_MODEL_FILE_NAME = "player.sdkmesh";

	// 物理パラメータ
	static constexpr float FRICTION			= 3.0f;		// 摩擦係数
	static constexpr float AIR_RESISTANCE	= 0.05f;	// 空気抵抗係数
	static constexpr float ACCELERATION		= 100.0f;	// 加速力（加速の強さ）
	static constexpr float MAX_MOVE_SPEED	= 20.0f;	// 最大速度
	static constexpr float MAX_FLYING_MOVE_SPEED = 15.0f;// 空中速度
	static constexpr float ROTATION_SPEED	= 1.0f;		// 回転速度(s)
	static constexpr float DECELERATION		= 60.0f;	// 減速力（ブレーキの強さ）
	static constexpr float JUMPING_POWER	= 1200.0f;	// ジャンプ力

	// ワイヤー関連
	static constexpr float WIRE_LENGTH = 65.0f;					// ワイヤーの長さ
	static constexpr float MAX_TARGETING_RAY_DISTANCE = 30.0f;	// ワイヤーの照準計算に使用する仮想的な最大距離（ワイヤーの実際の長さとは異なる）

	// アニメーション関連
	static const constexpr char* ANIM_WALKING		= "Resources/Models/player_walk.sdkmesh_anim";
	static const constexpr char* ANIM_IDLENG		= "Resources/Models/player_idle.sdkmesh_anim";
	static const constexpr char* ANIM_WIREACTION	= "Resources/Models/player_wireAction.sdkmesh_anim";

	// プレイヤーの初期設定値
	static constexpr float DEFAULT_MODEL_SCALE = 0.5f;              // プレイヤーモデルの標準的なスケール
	static constexpr float INITIAL_POS_Y = 29.0f;                   // プレイヤーの初期Y座標
	static constexpr float DEFAULT_COLLIDER_RADIUS_FACTOR = 1.3f;   // コライダー半径のスケール乗数
	static constexpr float DEFAULT_ROTATION_Y_DEGREE = 180.0f;      // デフォルト回転のY軸角度（度）

	// 衝突・物理演算に関する閾値
	static constexpr float SQUARED_ZERO_THRESHOLD = 1e-6f;          // ベクトル長二乗のゼロ判定閾値 (1e-6f)

	// ワイヤー関連の挙動調整値
	static constexpr int WIRE_SIMULATION_ITERATIONS = 1;             // XPBDシミュレーションの標準的な反復回数
	static constexpr float WIRE_TARGET_FINDER_RADIUS = 30.0f;        // WireTargetFinderに渡す初期半径（探索距離）
	static constexpr float WIRE_RELEASE_VELOCITY_MULTIPLIER = 3.0f;  // ワイヤー解放時の速度倍率
	static constexpr float SHOOT_WIRE_INITIAL_SPEED = 200.0f;        // ワイヤーの初期発射速度
	static constexpr float GRABBING_DISTANCE = 1.5f;                 // 掴み位置計算のための固定距離
	static constexpr float ROTATE_SPEED_FACTOR = 0.1f;               // 移動方向への回転速度（スムージング）係数

	// 描画に関するオフセット値
	static constexpr float CURSOR_Y_OFFSET_SCALE = 140.f;            // カーソルのY軸オフセットの画面スケール係数
	static constexpr float MODEL_DEFAULT_OFFSET_Y = -2.5f;           // モデルのデフォルトY軸オフセットのスケール係数
	static constexpr float WIRE_ACTION_OFFSET_X = 1.9f;              // ワイヤーアクション時のX軸オフセット
	static constexpr float WIRE_ACTION_OFFSET_Y = -1.7f;             // ワイヤーアクション時のY軸オフセット

    
}