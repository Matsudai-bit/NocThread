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
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/GameObjects/Common/GameObject.h"

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

	enum class SHAPE_ID
	{
		RECTANGLE	= 0,
		CIRCLE		= 1,
		TRIANGLE	= 2,
	};

// クラス定数の宣言 -------------------------------------------------
public:

	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	static constexpr DirectX::SimpleMath::Vector2 RENDERING_SIZE{ 800.0f, 800.0f };
	static constexpr float MAP_SCALE = 0.2f;
	static constexpr DirectX::SimpleMath::Vector2 MAP_POSITION{ (RENDERING_SIZE.x / 2.0f) * MAP_SCALE + 10.0f , (RENDERING_SIZE.y / 2.0f)* MAP_SCALE + 10.0f };  // マップの中心座標(スクリーン座標）

	static constexpr int VERTEX_NUM = 4;

	static constexpr float SPACE_VALUE = 0.9f;  // マップ中心からの距離度合　1だとそのままの距離感
	static constexpr float SPACE_VALUE_1 = 1.0f;  // マップ中心からの距離度合　1だとそのままの距離感
	static constexpr float MARK_SIZE = 0.14f;  // マークのサイズ
	static constexpr float MARK_SIZE_1 = 0.025f;  // マークのサイズ

	static constexpr float PLAYER_MARKER_SCALE = 0.7f;  // マークのサイズ

	static constexpr DirectX::SimpleMath::Vector2 MAP_OFFSET{ 10.0f, 10.0f }; // マップのオフセット位置

	// マップに表示される色
	static constexpr DirectX::SimpleMath::Vector4 BUILDING_MARK_COLOR	{ 0.6f, 0.6f, 0.6f, 1.0f };	// 建物の色
	static constexpr DirectX::SimpleMath::Vector4 PLAYER_MARK_COLOR		{ 1.0f, 1.0f, 0.0f, 1.0f };	// プレイヤーの色
	static constexpr DirectX::SimpleMath::Vector4 TREASURE_MARK_COLOR	{ 0.0f, 1.0f, 0.5f, 1.0f };	// お宝の色
	static constexpr DirectX::SimpleMath::Vector4 CHECKPOINT_MARK_COLOR	{ 0.0f, 1.0f, 0.5f, 1.0f };	// チェックポイント
	static constexpr DirectX::SimpleMath::Vector4 ENEMY_MARK_COLOR		{ 1.0f, 0.0f, 0.0f, 1.0f };	// 敵の色
	static constexpr DirectX::SimpleMath::Vector4 HELICOPTER_MARK_COLOR	{ 1.0f, 0.0f, 1.0f, 1.0f };	// ヘリコプターの色


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
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_ps_Triangle;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vs;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_gs;

	// スプライト
	std::unique_ptr<Sprite> m_minimapSprite;	///< ミニマップスプライト
	std::unique_ptr<Sprite> m_playerMarkSprite; ///< プレイヤーの印スプライト

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

	struct  VertexData
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector2 texCoord;

	};

	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_PS_Minimap;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_minimapConstantBuffer;

	// インスタンシング描画用　-------------------------------------------------

public:
	struct  InstanceBuffer
	{
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Vector4 color;
		float shapeID;

	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBufferIns;		///< インスタンシング用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBufferIns;		///< インスタンシング用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBufferIns;	///< インスタンシング用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBufferIns;	///< 定数バッファ
	InstanceBuffer* m_pInstanceDataIns;							///< インスタンシング用バッファ

	// 頂点シェーダ
	Microsoft::WRL::ComPtr <ID3D11VertexShader> m_vsIns;
	Microsoft::WRL::ComPtr <ID3D11GeometryShader> m_gsIns;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> m_psIns;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayoutIns;	///< インプットレイアウト



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

	void SetUpInstancing();
	void DrawInstancing(const DirectX::SimpleMath::Vector2& mapSize);

	// ミニマップテクスチャの作成
	void CreateMinimapTexture(const DirectX::SimpleMath::Vector2& mapSize);

	void DrawPlayer(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawBuilding(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawTreasure(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawCheckpoint(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawEnemy(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);
	void DrawHelicopter(DirectX::VertexPositionColorTexture vertexes[VERTEX_NUM], const DirectX::SimpleMath::Vector2& mapSize);

	// ミニマップ上の座標の算出
	DirectX::SimpleMath::Vector2 CalcMinimapPosition(const DirectX::SimpleMath::Vector3& worldPosition, const DirectX::SimpleMath::Vector2& mapSize);

	// インスタンスバッファにゲームオブジェクトの情報をセット
	void SetInstanceBufferGameObject(InstanceBuffer* instanceData, int* currentIndex, const DirectX::SimpleMath::Vector2& mapSize, const GameObjectTag& tag, const DirectX::SimpleMath::Vector4& color, const SHAPE_ID& shapeID);

	InstanceBuffer GetInstanceData(const DirectX::SimpleMath::Vector3& worldPosition, const DirectX::SimpleMath::Quaternion& rotate, const float& scale, const DirectX::SimpleMath::Vector4& color, const SHAPE_ID& shapeID, const DirectX::SimpleMath::Vector2& mapSize);

	// 頂点データに渡すマップ上の位置の算出
	DirectX::SimpleMath::Vector3 CalcVertexMapPosition(const DirectX::SimpleMath::Vector3& worldPosition, const DirectX::SimpleMath::Vector2& mapSize);

	void DrawPlayerMarker(const DirectX::SimpleMath::Vector2& mapSize);

	DirectX::SimpleMath::Vector2 CalcMinimapPosition() const;
};
