/*****************************************************************//**
 * @file    SpriteNumber.h
 * @brief   数値スプライトクラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/02/03
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <vector>
#include <memory>
#include "Game/Common/UserInterfaceTool/Sprite/ISprite2D.h"


// クラスの前方宣言 ===================================================
class ResourceManager;	// リソース管理
class Sprite;
class Canvas;

// クラスの定義 ===============================================================


/**
 * @brief 数値スプライト
 */
class SpriteNumber : public ISprite2D
{
private:
	std::vector<Sprite> m_numberSprite;	///< 番号のスプライト

	std::unique_ptr<Sprite> m_parentSprite;					///< 親スプライト

	DirectX::SimpleMath::Vector2 m_position;
	float m_scale;

	int m_minDigits;		///< 最小限確保する桁数 負の値の時は確保制限なし

// コンストラクタ・デストラクタ
public:

	// コンストラクタ
	SpriteNumber();

	// デストラクタ
	~SpriteNumber() = default;

// 操作
public:

	// 初期化処理
	void  Initialize(ID3D11ShaderResourceView* texture);

	// 数値スプライト群の描画処理
	 void DrawSprite(DirectX::SpriteBatch* pSpriteBatch) override;

	 // 高さの設定
	 void SetHeight(const int& height);

	 // 左上Y座標の設定（画像の左上の高さ）
	 void SetLeftTopY(const int& y);
	 
	 std::vector<Sprite> GetDrawingSprites();

// 取得/設定
public:

	// 番号の設定
	void SetNumber(const int& number);

	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector2& pos) { m_position = pos; }
	
	// 拡大率の設定
	void SetScale(const float& scale);

	// 最小限確保する桁数の設定
	void SetMinDigits(const unsigned int& digit) { m_minDigits = digit; }

	// 横幅の取得
	int GetWidth() const;

	// 一つのスプライトの横幅
	int GetOneSpriteWidth() const;

	// 縦幅の取得
	int GetHeight() const; 


// 内部実装
private:

	// 桁数の取得
	int  GetDigitCount(int value);


};

