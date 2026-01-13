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

	// 列挙型の宣言 -------------------------------------------------
private:
	enum class SHAPE_ID
	{
		RECTANGLE = 0,
		CIRCLE = 1,
		TRIANGLE = 2,
	};


// 構造体の宣言 -------------------------------------------------
private:

	// 定数バッファ用構造体
	struct ConstantBuffer
	{
		DirectX::SimpleMath::Vector2 position;
		DirectX::SimpleMath::Vector2 scale;
		DirectX::SimpleMath::Vector2 windowSize;

		DirectX::SimpleMath::Vector2 dummy;
	};

	// ミニマップ用定数バッファ
	struct MinimapConstantBuffer
	{
		DirectX::SimpleMath::Vector4 transparentColor;
		DirectX::SimpleMath::Vector2 aspect;
		float padding[2];
	};

	// 頂点データ構造体
	struct  VertexData
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector2 texCoord;

	};


public:

	// インスタンスデータ構造体
	struct  InstanceBuffer
	{
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Vector4 color;
		float shapeID;

		InstanceBuffer()
			: world{ DirectX::SimpleMath::Matrix::Identity }
			, color{ DirectX::SimpleMath::Vector4::Zero }
			, shapeID{ 0.0f }
		{
		}

	};
	

// クラス定数の宣言 -------------------------------------------------
public:

	// --- システム・描画設定 ---
	static const std::vector<D3D11_INPUT_ELEMENT_DESC>	INPUT_LAYOUT;		// インプットレイアウト定義
	static constexpr int								VERTEX_NUM = 4;		// 頂点数
	static constexpr int								MAX_INSTANCE = 1000;// 最大インスタンス数

	// --- 画面配置・スケール ---
	static constexpr DirectX::SimpleMath::Vector2	RENDERING_SIZE{ 800.0f, 800.0f };	// レンダリングサイズ
	static constexpr float							MAP_SCALE = 0.2f;					// マップのスケール
	static constexpr DirectX::SimpleMath::Vector2	MAP_OFFSET{ 10.0f, 10.0f };			// マップのオフセット位置
	static constexpr DirectX::SimpleMath::Vector2	MAP_POSITION{						// マップの中心座標(スクリーン座標）
		(RENDERING_SIZE.x / 2.0f) * MAP_SCALE + 10.0f ,
		(RENDERING_SIZE.y / 2.0f) * MAP_SCALE + 10.0f };  

	// --- マーク・表示調整 ---
	static constexpr float SPACE_VALUE			= 0.9f;	// マップ中心からの距離度合　1だとそのままの距離感
	static constexpr float MARK_SIZE			= 0.14f;// マークのサイズ
	static constexpr float PLAYER_MARKER_SCALE	= 0.7f; // マークのサイズ

	// --- マップカラー設定 ---
	static constexpr DirectX::SimpleMath::Vector4 BUILDING_MARK_COLOR	{ 0.6f, 0.6f, 0.6f, 1.0f };		// 建物の色
	static constexpr DirectX::SimpleMath::Vector4 PLAYER_MARK_COLOR		{ 1.0f, 1.0f, 0.0f, 1.0f };		// プレイヤーの色
	static constexpr DirectX::SimpleMath::Vector4 TREASURE_MARK_COLOR	{ 0.0f, 1.0f, 0.5f, 1.0f };		// お宝の色
	static constexpr DirectX::SimpleMath::Vector4 CHECKPOINT_MARK_COLOR	{ 0.0f, 1.0f, 0.5f, 1.0f };		// チェックポイント
	static constexpr DirectX::SimpleMath::Vector4 ENEMY_MARK_COLOR		{ 1.0f, 0.0f, 0.0f, 1.0f };		// 敵の色
	static constexpr DirectX::SimpleMath::Vector4 HELICOPTER_MARK_COLOR	{ 1.0f, 0.0f, 1.0f, 1.0f };		// ヘリコプターの色
	static constexpr DirectX::SimpleMath::Vector4 MINIMAP_BACK_COLOR	{ 0.01f, 0.01f, 0.48f, 1.0f };	// ミニマップ背景色
	
	
private:
	

	// データメンバの宣言 -----------------------------------------------

private:
	// シェーダー・レイアウト
	Microsoft::WRL::ComPtr<ID3D11VertexShader>   m_vsIns;              ///< インスタンシング描画用頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader>    m_psIns;              ///< インスタンシング描画用ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader>    m_PS_Minimap;         ///< ミニマップ背景・マスク処理用ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11InputLayout>    m_inputLayoutIns;     ///< インスタンシング用入力レイアウト

	// バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBufferIns;            ///< 図形用頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBufferIns;             ///< 図形用インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBufferIns;          ///< 行列・色・IDを格納するインスタンスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBufferIns;          ///< 窓サイズを格納する定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_minimapConstantBuffer;      ///< 透過色やアスペクト比用のミニマップ定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;             ///< その他の汎用定数バッファ

	// 外部参照・リソース
	const CommonResources* m_pCommonResources;                          ///< 共通リソースへの参照
	std::unique_ptr<MyLib::OffscreenRendering> m_offscreenRendering;    ///< オフスクリーンレンダリング制御クラス
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;                ///< 2Dスプライト一括描画用
	std::unique_ptr<Sprite> m_minimapSprite;                            ///< ミニマップ本体のスプライト
	std::unique_ptr<Sprite> m_playerMarkSprite;                         ///< プレイヤー専用アイコンのスプライト

	// 状態変数
	DirectX::SimpleMath::Vector2 m_mapSize;                             ///< ミニマップの表示領域サイズ


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Minimap(const CommonResources* pCommonResources);

	// デストラクタ
	~Minimap();


// 操作
public:


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

	// 初期化処理
	void Initialize();

	// 描画フロー管理 -------------------------------------------------
	// ミニマップ全体の描画フロー（オフスクリーンテクスチャの生成）を管理する
	void CreateMinimapTexture(const DirectX::SimpleMath::Vector2& mapSize);

	// ミニマップテクスチャをスクリーンに描画する
	void DrawTextureMinimap(const DirectX::SimpleMath::Vector2& mapSize);


	// インスタンシングシステム-------------------------------------------------
	//  シェーダやレイアウト、共通定数バッファのバインドを行う
	void SetUpInstancing();

	// インスタンスバッファの内容を元に描画命令（DrawIndexedInstanced）を発行する
	void DrawInstancing(const DirectX::SimpleMath::Vector2& mapSize);

	// 特定タグのオブジェクト群を検索し、インスタンスバッファ用の配列へ一括設定する
	void SetInstanceBufferGameObject(
		InstanceBuffer* instanceData,
		int* currentIndex,
		const DirectX::SimpleMath::Vector2& mapSize,
		const GameObjectTag& tag,
		const DirectX::SimpleMath::Vector4& color,
		const SHAPE_ID& shapeID);

	//  個別のオブジェクト情報から、歪みのない行列を含む単一のインスタンスデータを作成する
	InstanceBuffer CreateInstanceData(
		const DirectX::SimpleMath::Vector3& worldPosition,
		const float& scale,
		const DirectX::SimpleMath::Vector4& color,
		const SHAPE_ID& shapeID,
		const DirectX::SimpleMath::Vector2& mapSize);


	//  座標変換・計算 -------------------------------------------------

	// ワールド座標からミニマップ上の 2D ピクセル座標を算出する
	DirectX::SimpleMath::Vector2 CalcMinimapPosition(
		const DirectX::SimpleMath::Vector3& worldPosition,
		const DirectX::SimpleMath::Vector2& mapSize);

	// ミニマップ自体のスクリーン上での配置座標（左上など）を算出する
	DirectX::SimpleMath::Vector2 CalcMinimapPosition() const;

	// 頂点データとして渡すためのミニマップ空間上の 3D 位置を算出する
	DirectX::SimpleMath::Vector3 CalcVertexMapPosition(
		const DirectX::SimpleMath::Vector3& worldPosition,
		const DirectX::SimpleMath::Vector2& mapSize);


	// 特殊要素の描画 -------------------------------------------------

	// プレイヤー自身のアイコンを専用のスプライトまたは手法で描画する
	void DrawPlayerMarker(const DirectX::SimpleMath::Vector2& mapSize);
};
