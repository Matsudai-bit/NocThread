/*****************************************************************//**
 * @file    CircularShadow.h
 * @brief   丸影に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/08
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>

#include "Game/Common/DeviceResources.h"

// クラスの前方宣言 ===================================================
class Camera; // カメラ

// クラスの定義 ===============================================================
/**
 * @brief 丸影
 */
class CircularShadow
{
// クラス定数の宣言 -------------------------------------------------
public:

	static constexpr int VERTEX_NUM = 4;

	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

// 構造体の宣言
private: 
	
	struct ConstantBuffer
	{
		DirectX::SimpleMath::Matrix matWorldTranspose;
		DirectX::SimpleMath::Matrix matView;
		DirectX::SimpleMath::Matrix matProj;
		DirectX::SimpleMath::Vector4 alphaValue;
	};

// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_primitiveBatch; ///< プリミティブバッチ
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;		///< ベーシックエフェクト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;	///< インプットレイアウト

	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_ps;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vs;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_gs;

	DX::DeviceResources* m_pDeviceResources;

	float m_castPositionY;	///< 影を落とすY座標

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CircularShadow();

	// デストラクタ
	~CircularShadow();


// 操作
public:
	// 初期化処理
	void Initialize(DX::DeviceResources* pDeviceResources,const float& castShadowPositionY);

	// 描画処理
	void Draw( const Camera& camera, DirectX::CommonStates* states, const DirectX::SimpleMath::Vector3& shadowOwnerPosition);



// 取得/設定
public:


// 内部実装
private:


};
