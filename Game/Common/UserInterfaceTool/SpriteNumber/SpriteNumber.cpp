/*****************************************************************//**
 * @file    SpriteNumber.h
 * @brief   数値スプライトへクラスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/02/03
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SpriteNumber.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"

#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
using namespace DirectX;

// メンバ関数の定義 ===========================================================

/**
 * @brief コンストラクタ
 * 
 */
SpriteNumber::SpriteNumber()
	: m_numberSprite{}
	, m_scale{1.0f}
	, m_minDigits{ -1 }
	, m_isVisible{ true }

{
}


/**
 * @brief 初期化処理
 * 
 * @param[in] texture
 */
void SpriteNumber::Initialize(ID3D11ShaderResourceView* texture)
{
	// 親スプライトの作成
	m_parentSprite = std::make_unique<Sprite>();
	// 初期化処理
	m_parentSprite->Initialize(texture);

	// 拡大率の設定
	m_scale = 1.0f;

	// 最小限確保する桁数を無効
	m_minDigits = -1;
}

/**
 * @brief 数値スプライト群の描画処理
 * 
 * @param[in] numberSprite
 * @param[in] position
 */
void SpriteNumber::DrawSprite(DirectX::SpriteBatch* pSpriteBatch)
{
	using namespace::SimpleMath;

	/*float x = m_position.x;

	for (auto& sprite : m_numberSprite)
	{
		sprite.SetPosition(Vector2(x , m_position.y));
		sprite.DrawSprite({ pSpriteBatch });
		x += static_cast<float>(sprite.GetSpriteWidth() );
	}*/

	auto drawingSprite = GetDrawingSprites();
	for (auto& sprite : drawingSprite)
	{
		sprite.DrawSprite(pSpriteBatch);
	}
}

/**
 * @brief 高さの設定
 * 
 * @param[in] height	縦幅
 */
void SpriteNumber::SetHeight(const int& height)
{
	RECT region = m_parentSprite->GetRegion();
	
	m_parentSprite->SetRegion(Sprite::Region{region.left, region.top, region.right, height });
}

/**
 * @brief 左上Y座標の設定（画像の左上の高さ）
 * 
 * @param[in] y
 */
void SpriteNumber::SetLeftTopY(const int& y)
{
	RECT region = m_parentSprite->GetRegion();

	m_parentSprite->SetRegion(Sprite::Region{ region.left, y, region.right, region.bottom });
}

std::vector<Sprite> SpriteNumber::GetDrawingSprites()
{
	using namespace::SimpleMath;

	std::vector<Sprite> result;

	float x = m_position.x;

	for (auto& sprite : m_numberSprite)
	{
		sprite.SetPosition(Vector2(x, m_position.y));
		result.push_back(sprite);
		x += static_cast<float>(sprite.GetSpriteWidth());
	}
	return result;
}




void SpriteNumber::SetNumber(const int& number)
{
	// 幅の取得
	int width = m_parentSprite->GetSpriteWidth() / 10;

	int digit = GetDigitCount(number);

	if (m_minDigits > 0 && digit < m_minDigits)
	{
		digit = m_minDigits;
	}


	m_numberSprite.resize(digit);

	for (int i = digit; i > 0; i--)
	{
		Sprite digitSprite = *m_parentSprite;
		digitSprite.SetScale(m_scale);

		RECT region = m_parentSprite->GetRegion();

		// 現在の桁の数値を取り出す
		int digitNum = number / static_cast<int>(std::pow(10.0, i - 1)) % 10;


		// 開始地点
		int topLeftX	= width * digitNum;
		int topLeftY	= region.top;
		int spriteWidth = width;
		int spriteHeight= m_parentSprite->GetSpriteHeight();

		digitSprite.SetRegion(Sprite::Region{ topLeftX,topLeftY, spriteWidth, spriteHeight });

		m_numberSprite[static_cast<size_t>(digit - (i))] = digitSprite;

		//m_numberSprite.emplace_back(std::move(digitSprite));
	}

}

/**
 * @brief 拡大率の設定
 * 
 * @param[in] scale　拡大率
 */
void SpriteNumber::SetScale(const float& scale)
{
	m_scale = scale;
}


/**
 * @brief 横幅の取得
 * 
 * @return 横幅
 */
int SpriteNumber::GetWidth() const
{
	return static_cast<int>(m_numberSprite.size()) * static_cast<int>(static_cast<float>(m_parentSprite->GetSpriteWidth()) / 10.0 * m_scale)  ;
}
/**
 * @brief 一つのスプライトの横幅の取得
 * 
 * @return 
 */
int SpriteNumber::GetOneSpriteWidth() const
{
	return  static_cast<int>(static_cast<float>(m_parentSprite->GetSpriteWidth()) / 10.0 * m_scale);
}
/**
 * @brief 縦幅の取得
 * 
 * @return 縦幅
 */
int SpriteNumber::GetHeight() const
{
	return static_cast<int>(m_numberSprite.size()) * m_parentSprite->GetSpriteWidth() / 10;
}
/**
 * @brief 桁数の取得
 * 
 * @param[in] value 数値
 * @return 桁数
 */
int SpriteNumber::GetDigitCount(int value)
{
	if (value == 0) return 1; // 0の場合は1桁
	return static_cast<int>(std::log10(std::abs(value))) + 1;
}