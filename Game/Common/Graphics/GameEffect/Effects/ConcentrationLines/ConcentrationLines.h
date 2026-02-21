/*****************************************************************//**
 * @file    ConcentrationLines.h
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

// ライブラリ関連
#include "Library/DirectXFramework/DeviceResources.h"

// グラフィック関連
#include "Game/Common/Graphics/GameEffect/Base/GameEffectBase.h"

// ユーティリティ関連
#include "Game/Common/Utillities/ElapsedTimeCounter/ElapsedTimeCounter.h"

// クラスの前方宣言 ===================================================
class CircularShadow;
class Camera;

// クラスの定義 ===============================================================
/**
 * @brief シンプルなパーティクルエフェクト
 */
class ConcentrationLines
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
		DirectX::SimpleMath::Vector3  lineColor;
		float time;
		float speed;

		float noiseScale;
		DirectX::SimpleMath::Vector2 uvOffset;
		float lineRegion;
		float lineRate;
		float padding[2];
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

	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_ps;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vs;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_gs;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;


	// その他
	DirectX::CommonStates m_commonStates; ///< 共通状態
	DX::DeviceResources* m_pDeviceResources;

	ElapsedTimeCounter m_elapsedTimeCounter; ///< 経過時間計測

	// パラメータ
	float m_lineLengthRate ;	///< 線の長さ
	float m_speed ;			///< 線の流れる速さ


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ConcentrationLines(DX::DeviceResources* pDeviceResources, const float& speed, const float& lineLengthRate);

	// デストラクタ
	~ConcentrationLines();


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
	
	// 線の長さの倍率設定
	void SetLineLengthRate(float lineLengthRate) { m_lineLengthRate =  std::min(lineLengthRate, 1.2f); }
	// 速度設定
	void SetSpeed(float speed) { m_speed = speed; }

// 内部実装
private:


};
