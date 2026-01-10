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
#include "Library/DirectXFramework/DeviceResources.h"
#include "Game/Common/ElapsedTimeCounter/ElapsedTimeCounter.h"

// クラスの前方宣言 ===================================================
class CircularShadow;
class Camera;

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
		float		time = 0.0f;
		float		speed = 0.0f;

		bool isActive = false; // 再利用のため

		Particle()
			: time{ 0.0f }
			, speed{ 0.0f }
			, isActive{ false }
		{
		}
	};


	struct ConstantBuffer
	{
		DirectX::SimpleMath::Matrix matWorldTranspose;
		DirectX::SimpleMath::Matrix matView;
		DirectX::SimpleMath::Matrix matProj;

		DirectX::SimpleMath::Vector3 objectTarget;    // オブジェクトの注視点(ビルボード用)
		float scale;
	};

	// クラス定数の宣言 -------------------------------------------------
public:

	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	// データメンバの宣言 -----------------------------------------------
private:

	// 描画関連
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_primitiveBatch; ///< プリミティブバッチ
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;		///< ベーシックエフェクト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;	///< インプットレイアウト
	std::unique_ptr<DirectX::GeometricPrimitive> m_model;

	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_ps;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vs;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_gs;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;


	// パーティクル生成関連
	std::vector<Particle> m_particles;	///< パーティクル群
	ElapsedTimeCounter m_timeCounter;   ///< 時間計測器
	std::mt19937 m_randomGenerator;		///< ランダム生成器
	DirectX::SimpleMath::Vector3 m_position; ///< 中心座標

	// その他
	DirectX::CommonStates m_commonStates; ///< 共通状態
	DX::DeviceResources* m_pDeviceResources;
	const Camera* m_pCamera;					 ///< カメラ




// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SimpleParticle(DX::DeviceResources* pDeviceResources, DirectX::SimpleMath::Vector3 position, const Camera* pCamera);

	// デストラクタ
	~SimpleParticle();


// 操作
public:
	// 再生
	void Play() override;
	// 更新処理
	void Update(float deltaTime, const float& isEnd) override;
	// 描画処理
	void Draw(const Camera& camera) override;

// 取得/設定
public:

	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector3& position);

// 内部実装
private:

	void CreateParticle();

};
