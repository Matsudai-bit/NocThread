/*****************************************************************//**
 * @file    Sprite.h
 * @brief   スプライトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/04/29
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "Sprite.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Sprite::Sprite()
	: m_sourceRegion {}
	, m_rotationAngle{ 0.0f }
	, m_scale		 { 0.0f }
	, m_opacity		 { 1.0f }
	, m_texture		 {}
	, m_centerPos	 {}
{

}



/**
 * @brief デストラクタ
 */
Sprite::~Sprite()
{

}

/**
 * @brief 初期化処理
 * 
 * @param[in] texture	テクスチャ
 */
void Sprite::Initialize(ID3D11ShaderResourceView* texture)
{

	// サイズの取得
	SimpleMath::Vector2 size = GetTextureSize(texture);
	
	Initialize(texture, Region{ 0, 0, static_cast<int>(size.x), static_cast<int>(size.y) });
}

/**
 * @brief 初期化処理
 * 
 * @param[in] texture　 テクスチャ
 * @param[in] region	領域
 */
void Sprite::Initialize(ID3D11ShaderResourceView* texture, Region region)
{
	// テクスチャの設定
	m_texture = texture;

	// 拡大率の初期化
	m_scale = 1.0f;
	// 回転角の初期化
	m_rotationAngle = 0.0f;

	// 領域の設定
	SetRegion(region);

	// 不透明度の初期化
	m_opacity = 1.0f;


}

/**
 * @brief 描画処理
 * 
 *  * @param[in] spriteBatch	スプライトバッチ
 */
void Sprite::DrawSprite(SpriteBatch* spriteBatch)
{


	// 描画
	spriteBatch->Draw(
		m_texture,				// テクスチャ
		m_position,					// 描画座標
		&m_sourceRegion,
		SimpleMath::Vector4(1,1,1,m_opacity), // 色（テクスチャに乗算される色）
		m_rotationAngle,		// 回転角（ラジアン）
		m_centerPos,			// 原点の回転軸
		SimpleMath::Vector2(m_scale, m_scale));	//拡大率

	
	
}


void Sprite::Finalize()
{
	
}



/**
 * @brief 描画領域の設定
 * 
 * @param[in] region	領域
 */
void Sprite::SetRegion(Region region)
{
	m_sourceRegion.left		= region.topLeftX;
	m_sourceRegion.top		= region.topLeftY;
	m_sourceRegion.right	= region.topLeftX + region.width;
	m_sourceRegion.bottom	= region.topLeftY + region.height;

	// 中心座標の設定
	m_centerPos = GetSpriteCenterPos();

}

/**
 * @brief 描画領域の設定
 * 
 * @param[in] rect 領域(rect基準)
 */
void Sprite::SetRegion(RECT rect)
{
	m_sourceRegion = rect;

	// 中心座標の設定
	m_centerPos = GetSpriteCenterPos();

}

/**
 * @brief テクスチャのサイズを取得する
 *
 * @param[in]  pTexture		調べるテクスチャ
 * 
 * @return テクスチャサイズ
 */
SimpleMath::Vector2 Sprite::GetTextureSize(ID3D11ShaderResourceView* pTexture)
{
	// テクスチャの幅と高さを取得する
	ID3D11Resource* resource = nullptr;
	pTexture->GetResource(&resource);

	ID3D11Texture2D* texture = nullptr;
	resource->QueryInterface(&texture);

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	static_cast<float>(desc.Width);
	static_cast<float>(desc.Height);

	// リソースの解放
	resource->Release();
	texture->Release();

	return { static_cast<float>(desc.Width) , static_cast<float>(desc.Height) };
}

/**
 * @brief スプライトの中心座標の取得
 * 
 * @return スプライトの中心座標の取得
 */
DirectX::SimpleMath::Vector2 Sprite::GetSpriteCenterPos() const
{
	// 中心座標の設定
	return SimpleMath::Vector2(
		static_cast<float>((m_sourceRegion.right - m_sourceRegion.left) / 2.0f),
		static_cast<float>( (m_sourceRegion.bottom - m_sourceRegion.top)) / 2.0f);
}



