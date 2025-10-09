/*****************************************************************//**
 * @file    Sprite.h
 * @brief   スプライトクラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/04/29
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/UserInterfaceTool/Sprite/ISprite2D.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief スプライト
 */
class Sprite : public ISprite2D
{
// クラス定数の宣言 -------------------------------------------------
public:

// 構造体の宣言 -------------------------------------------------
public:

	/**
	 * @brief 領域
	 */
	struct Region
	{
		int topLeftX;  ///< 領域の左上のX座標
		int topLeftY;  ///< 領域の左上のY座標
		int width;     ///< 領域の幅
		int height;    ///< 領域の高さ


	};



// データメンバの宣言 -----------------------------------------------
private:

	ID3D11ShaderResourceView* m_texture;	///< テクスチャ
	RECT	m_sourceRegion;					///< テクスチャ上の描画領域

	float  m_scale;							///< 拡大率
	float  m_rotationAngle;					///< 回転角[ラジアン]
	float  m_opacity;						///< 不透明度

	DirectX::SimpleMath::Vector2 m_centerPos;	///< テクスチャの中心座標
	DirectX::SimpleMath::Vector2 m_position;	///< 描画座標

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Sprite();

	// デストラクタ
	~Sprite();


// 操作
public:

	// 初期化処理 (領域自動設定)
	void Initialize(ID3D11ShaderResourceView* texture);

	// 初期化処理 (領域の設定)
	void Initialize(ID3D11ShaderResourceView* texture, Region region);


	// 描画処理
	void DrawSprite(DirectX::SpriteBatch* spriteBatch) override ;

	// 終了処理
	void Finalize();

// 取得/設定
public:

	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector2& pos) { m_position = pos; }
	// 座標の取得
	DirectX::SimpleMath::Vector2 GetPosition() const {return m_position;}


	// スプライトの横幅の取得
	int GetSpriteWidth() const { return static_cast<int>((static_cast<float>(m_sourceRegion.right - m_sourceRegion.left)) * m_scale); }

	// スプライトの縦幅の取得
	int GetSpriteHeight() const { return static_cast<int>((static_cast<float>(m_sourceRegion.bottom - m_sourceRegion.top)) * m_scale); }

	// 描画領域の設定
	void SetRegion(Region region);
	// 描画領域の設定
	void SetRegion(RECT rect);

	//　領域の取得
	RECT GetRegion() const { return m_sourceRegion; }

	// 拡大率の設定
	void SetScale(const float& scale) { m_scale = scale; }
	// 拡大率の取得
	float GetScale() const { return m_scale; };

	// 不透明の設定
	void SetOpacity(const float& opacity) { m_opacity = opacity; }
	// 不透明度の取得
	float GetOpacity() const { return m_opacity; }
	
	//　回転の設定
	void SetAngle(const float& angle) { m_rotationAngle = angle; }
	// 回転の取得
	float GetAngle() const { return m_rotationAngle; }


// 内部実装
private:

	// テクスチャのサイズとを取得する
	DirectX::SimpleMath::Vector2 GetTextureSize(ID3D11ShaderResourceView* pTexture);

	// スプライトの中心座標の取得
	DirectX::SimpleMath::Vector2 GetSpriteCenterPos() const;
};
