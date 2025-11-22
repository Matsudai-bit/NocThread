/*****************************************************************//**
 * @file    Minimap.h
 * @brief   ミニマップに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/11/23
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/TaskManager/TaskManager.h"

// クラスの前方宣言 ===================================================
class CommonResources; // 共通リソース
class Camera;

// クラスの定義 ===============================================================
/**
 * @brief ミニマップ
 */
class Minimap
	: public Task
{
// 構造体の宣言 -------------------------------------------------
private:
	struct ConstantBuffer
	{
		DirectX::SimpleMath::Vector2 position;
		DirectX::SimpleMath::Vector2 scale;
		DirectX::SimpleMath::Vector2 windowSize;

		DirectX::SimpleMath::Vector2 dummy;
	};

// クラス定数の宣言 -------------------------------------------------
public:

	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;
	static constexpr int VERTEX_NUM = 4;



// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_primitiveBatch; ///< プリミティブバッチ
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;		///< ベーシックエフェクト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;	///< インプットレイアウト

	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_ps;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vs;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_gs;

	const CommonResources* m_pCommonResources; ///< 共通リソース

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Minimap(const CommonResources* pCommonResources);

	// デストラクタ
	~Minimap();


// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 描画処理
	void DrawTask(const Camera& camera) override;

	// 終了処理
	void Finalize();


// 取得/設定
public:


// 内部実装
private:


};
