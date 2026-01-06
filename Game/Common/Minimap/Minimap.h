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
#include "Library/DirectXFramework/RenderTexture.h"
#include "Library/MyLib/DirectXMyToolKit/DepthStencil/DepthStencil.h"
#include "Library/MyLib/DirectXMyToolKit/OffscreenRendering/OffscreenRendering.h"

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

	static constexpr DirectX::SimpleMath::Vector2 MAP_POSITION_TOP_LEFT{ 10.0f, 10.0f,  };  // マップの中心座標(スクリーン座標）
	static constexpr float MAP_SCALE = 0.15f;

	static constexpr int VERTEX_NUM = 4;

	static constexpr float SPACE_VALUE = 1.0f;  // マップ中心からの距離度合　1だとそのままの距離感
	static constexpr float MARK_SIZE = 0.025f;  // マークのサイズ

	// マップに表示される色
	static constexpr DirectX::SimpleMath::Vector4 BUILDING_MARK_COLOR	{ 0.6f, 0.6f, 0.6f, 1.0f };	// 建物の色
	static constexpr DirectX::SimpleMath::Vector4 PLAYER_MARK_COLOR		{ 1.0f, 1.0f, 0.0f, 1.0f };	// プレイヤーの色
	static constexpr DirectX::SimpleMath::Vector4 TREASURE_MARK_COLOR	{ 0.0f, 1.0f, 0.5f, 1.0f };	// お宝の色
	static constexpr DirectX::SimpleMath::Vector4 CHECKPOINT_MARK_COLOR	{ 0.0f, 1.0f, 0.5f, 1.0f };	// チェックポイント
	static constexpr DirectX::SimpleMath::Vector4 ENEMY_MARK_COLOR		{ 1.0f, 0.0f, 0.0f, 1.0f };	// 敵の色
	static constexpr DirectX::SimpleMath::Vector4 HELICOPTER_MARK_COLOR	{ 1.0f, 0.0f, 1.0f, 1.0f };	// ヘリコプターの色

	static constexpr DirectX::SimpleMath::Vector2 RENDERING_SIZE{ 800.0f, 800.0f };

	static constexpr DirectX::SimpleMath::Vector4 MINIMAP_BACK_COLOR { 0.01f, 0.01f, 0.48f, 1.0f};
	
// データメンバの宣言 -----------------------------------------------
private:

	// 描画プリセット
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_primitiveBatch; ///< プリミティブバッチ
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;		///< ベーシックエフェクト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;	///< インプットレイアウト

	// シェーダ関連
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_ps_Circle;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_ps_Rectangle;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vs;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_gs;

	const CommonResources* m_pCommonResources; ///< 共通リソース

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

	// オフスクリーンレンダリング
	std::unique_ptr<MyLib::OffscreenRendering> m_offscreenRendering;

	DirectX::SimpleMath::Vector2 m_mapSize; ///< マップサイズ

	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	struct MinimapConstantBuffer
	{
		DirectX::SimpleMath::Vector4 transparentColor;
		DirectX::SimpleMath::Vector2 aspect;
		float padding[2];
	};

	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PS_Minimap;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_minimapConstantBuffer;
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

	// ミニマップテクスチャの作成
	void CreateMinimapTexture();

	void DrawPlayer(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawBuilding(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawTreasure(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawCheckpoint(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawEnemy(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawHelicopter(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);

	// ミニマップ上の座標の算出
	DirectX::SimpleMath::Vector2 CalcMinimapPosition(const DirectX::SimpleMath::Vector3& worldPosition, const DirectX::SimpleMath::Vector2& mapSize);
};
