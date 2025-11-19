/*****************************************************************//**
 * @file    Canvas.h
 * @brief   キャンバスに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/04/29
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Canvas.h"

#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Canvas::Canvas(ID3D11DeviceContext* context, const DirectX::CommonStates* pCommonStates)
	: m_pCommonStates{ pCommonStates }
{
	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
}



/**
 * @brief デストラクタ
 */
Canvas::~Canvas()
{

}



/**
 * @brief タスクの描画
 * 
 * @param[in] camera　カメラ
 */
void Canvas::DrawTask(const Camera& camera)
{
	DrawContents();
}

/**
 * @brief 描画
 * 
 */
void Canvas::DrawContents()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_pCommonStates->NonPremultiplied());

	// 描画処理
	for (auto sprite : m_sprites)
	{
		sprite->DrawSprite(m_spriteBatch.get());
	}

	m_spriteBatch->End();
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Canvas::Finalize()
{

}

/**
 * @brief スプライトの追加
 * 
 * @param[in] addSprite	追加スプライト
 */
void Canvas::AddSprite(ISprite2D* addSprite)
{
	m_sprites.push_back(addSprite);
}

/**
 * @brief 削除する
 * 
 * @param[in] addSprite
 */
void Canvas::RemoveSprite(ISprite2D* removeSprite)
{
	auto it = std::find(m_sprites.begin(), m_sprites.end(), removeSprite);

	if (it == m_sprites.end())
		return;

	// 要素の削除
	m_sprites.erase(it);
}
