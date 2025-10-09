/*****************************************************************//**
 * @file    TimeUI.h
 * @brief   時間表記UI関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/04/30
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "TimeUI.h"

#include "Game/Common/UserInterfaceTool/SpriteNumber/SpriteNumber.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TimeUI::TimeUI()
	: m_punctuationTexture	{ nullptr }
	, m_scale				{ 0.0f }
	, m_times				{ TIME_NUM }
{

}



/**
 * @brief デストラクタ
 */
TimeUI::~TimeUI()
	
{

}



/**
 * @brief 初期化処理
 * 
 * @param[in] numberTexture			数値テクスチャ
 * @param[in] punctuationTexture	区切りテクスチャ
 */
void TimeUI::Initialize(const Sprite::Region& numberSpriteRegion, ID3D11ShaderResourceView* numberTexture, ID3D11ShaderResourceView* punctuationTexture)
{
	m_punctuationTexture = punctuationTexture;

	m_punctuationSprite = std::make_unique<Sprite>();
	m_punctuationSprite->Initialize(punctuationTexture);
	
	// 時間の初期化処理
	for (auto& time : m_times)
	{
		time.Initialize(numberTexture);

		time.SetHeight(numberSpriteRegion.height);
		time.SetLeftTopY(numberSpriteRegion.topLeftY);
		time.SetMinDigits(TIME_MIN_DIGIT);
		time.SetNumber(0);
	}
}



/**
 * @brief 時間の更新処理
 *
 * @param[in] second 時間
 *
 * @return なし
 */
void TimeUI::UpdateTime(float second)
{
	using namespace SimpleMath;

	// 時間の算出
	int timeMin		=	static_cast<int>(second) / MINUTE_TO_SECONDS;	// 分
	int timeSecond	=	static_cast<int>(second) % MINUTE_TO_SECONDS;	// 秒
	int timeDecimal =	static_cast<int>((second - static_cast<float>(timeMin * MINUTE_TO_SECONDS + timeSecond)) * std::pow(10, TIME_MIN_DIGIT)) ; // 小数 第二位まで表示

	if (m_times.size() >= 1)
		m_times[0].SetNumber(timeMin);
	if (m_times.size() >= 2)
		m_times[1].SetNumber(timeSecond);
	if (m_times.size() >= 3)
		m_times[2].SetNumber(timeDecimal);

}

/**
 * @brief 描画処理
 * 
 * @param[in] pSpriteBatch	スプライトバッチ
 */
void TimeUI::DrawSprite(DirectX::SpriteBatch* pSpriteBatch)
{
	using namespace SimpleMath;
	auto drawingSprite = GetDrawingSprites();
	for (auto& sprite : drawingSprite)
	{
		sprite.DrawSprite(pSpriteBatch);
	}
}




/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TimeUI::Finalize()
{

}

/**
 * @brief 拡大率の設定
 * 
 * @param[in] scale 拡大率
 */
void TimeUI::SetScale(const float& scale)
{
	for (auto& time : m_times)
		time.SetScale(scale);

	m_punctuationSprite->SetScale(scale);
}

/**
 * @brief 横幅の取得
 * 
 * @return 横幅
 */
int TimeUI::GetWidth() const
{
	int timeTotalWidth		  = (m_times[0].GetOneSpriteWidth() * TIME_MIN_DIGIT) * TIME_NUM;
	int punctuationTotalWidth = m_punctuationSprite->GetSpriteWidth() * (TIME_NUM - 1);

	return timeTotalWidth + punctuationTotalWidth;
}

/**
 * @brief 配列の取得
 * 
 * @return 配列
 */
std::vector<Sprite> TimeUI::GetDrawingSprites() 
{
	using namespace SimpleMath;

	std::vector<Sprite> result;
	
	// スプライト同士の距離を考慮したX座標
	float x = m_position.x;
	// 時間の区切りスプライトの横幅
	float punctuationWidth = static_cast<float>(m_punctuationSprite->GetSpriteWidth());

	// 各描画処理
	for (size_t i = 0; i < m_times.size(); i++)
	{
		// 時間の座標の描画
		m_times[i].SetPosition(Vector2(x, m_position.y));
		auto numberSprite = m_times[i].GetDrawingSprites();
		result.insert(result.end(), numberSprite.begin(), numberSprite.end());

		x += static_cast<float>(m_times[i].GetWidth()) + static_cast<float>(punctuationWidth) / 2.0f;

		// まだ数字が続くとき
		if (i < m_times.size() - 1)
		{
			// 時間区切りの座標の描画
			m_punctuationSprite->SetPosition(Vector2(x, m_position.y));
			result.push_back(*m_punctuationSprite);

			x += static_cast<float>(punctuationWidth) / 2.0f + static_cast<float>(m_times[i].GetOneSpriteWidth());

		}

	}	return result;
}
