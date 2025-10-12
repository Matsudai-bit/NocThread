/*****************************************************************//**
 * @file    SimpleParticle.h
 * @brief   シンプルなパーティクルエフェクトに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/12
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>
#include <random>

#include "Game/Common/GameEffect/Base/GameEffectBase.h"
#include "Game/Common/DeviceResources.h"
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"
// クラスの前方宣言 ===================================================
class CircularShadow;

// クラスの定義 ===============================================================
/**
 * @brief シンプルなパーティクルエフェクト
 */
class SimpleParticle
	: public GameEffectBase
{

// 構造体の宣言 ---------------------------------------------------------
public:

	struct Particle
	{
		DirectX::SimpleMath::Vector3 position;
		float		time;
		float		speed;
	
		bool isActive; // 再利用のため
	};
	
	// クラス定数の宣言 -------------------------------------------------
public:



	// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<CircularShadow> m_circleShadow;

	DirectX::SimpleMath::Vector3 m_position;

	DirectX::CommonStates m_commonStates;

	std::vector<Particle> m_particles;

	ElapsedTimeCounter m_timeCounter;

	std::mt19937 m_randomGenerator;

	std::unique_ptr<DirectX::GeometricPrimitive> m_model;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SimpleParticle(DX::DeviceResources* pDeviceResources, DirectX::SimpleMath::Vector3 position);

	// デストラクタ
	~SimpleParticle();


// 操作
public:
	// 再生
	void Play() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

	// 再生してるかどうか
	bool IsPlaying() const override;

// 取得/設定
public:

	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector3& position);

// 内部実装
private:


};
