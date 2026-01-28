/*****************************************************************//**
 * @file    TimeUI.h
 * @brief   時間表記UIに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/04/30
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <vector>
#include <memory>
#include "Game/Common/UserInterfaceTool/Sprite/ISprite2D.h"

#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"


// クラスの前方宣言 ===================================================
class SpriteNumber;	// 数値表スプライト

// クラスの定義 ===============================================================
/**
 * @brief 時間表記UI
 */
class TimeUI : public ISprite2D
{
// クラス定数の宣言 -------------------------------------------------
public:

	static constexpr int TIME_NUM = 2;			// 時間の数(分,秒,小数)

	static constexpr int TIME_MIN_DIGIT = 2;	// 最小限表示する桁数

	static constexpr int MINUTE_TO_SECONDS = 60;// １分の秒数

// データメンバの宣言 -----------------------------------------------
private:
	std::unique_ptr<Sprite>		m_punctuationSprite;	///< 時間の区切り用スプライト
	std::vector<SpriteNumber>	m_times;				///< 各位の時間（例  {03},{10}） 3分10秒なら

	DirectX::SimpleMath::Vector2	m_position;			///< 座標
	float m_scale;										///< 拡大率

	ID3D11ShaderResourceView*		m_punctuationTexture;	///<　区切りテクスチャ

	bool m_isVisible;		///< 見えるかどうか

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TimeUI();

	// デストラクタ
	~TimeUI();


// 操作
public:
	// 初期化処理
	void Initialize(const Sprite::Region& numberSpriteRegion, ID3D11ShaderResourceView* numberTexture, ID3D11ShaderResourceView* punctuationTexture);

	// 時間の更新処理
	void UpdateTime(float second);

	// 描画処理
	void DrawSprite(DirectX::SpriteBatch* pSpriteBatch) override;

	// 終了処理
	void Finalize();


// 取得/設定
public:


	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector2& pos) { m_position = pos; }

	// 拡大率の設定
	void SetScale(const float& scale);


	// 横幅の取得
	int GetWidth() const;

	// 配列の取得
	std::vector<Sprite> GetDrawingSprites() ;

	// 見えるかどうか
	void SetVisible(bool isVisible) { m_isVisible = isVisible; }
	bool IsVisible() const override { return m_isVisible; }

// 内部実装
private:


};
