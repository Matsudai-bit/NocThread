/*****************************************************************//**
 * @file    Line2D.h
 * @brief   2Dのラインに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/14
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================
namespace DX
{
	class DeviceResources;
}

// クラスの定義 ===============================================================
/**
 * @brief 2Dのライン
 */
class Line2D
{
// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr float DEFAULT_SIZE_X = 100.3f;
	static constexpr float DEFAULT_SIZE_Y = 100.1f;


// データメンバの宣言 -----------------------------------------------
private:


	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;	///< プリミティブバッチ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect; ///< ベーシックエフェクト

	DX::DeviceResources* m_pDeviceResources;

	float m_length;
	float m_thickness;
	float m_radianAngle;
	DirectX::SimpleMath::Vector2 m_centerPosition;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Line2D();

	// デストラクタ
	~Line2D();


// 操作
public:
	// 初期化処理
	void Initialize(DX::DeviceResources* pDeviceResources);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(DirectX::CommonStates* pStates, float windowWidth, float windowHeight);

	// 終了処理
	void Finalize();



// 取得/設定
public:
	// 中心座標
	void SetPosition(const DirectX::SimpleMath::Vector2& center);
		
	// 長さの設定
	void SetLength(const float& length);
	// 太さの設定
	void SetThickness(const float& thickness);
	// 角度の設定
	void SetAngle(const float& radian);

// 内部実装
private:


};
