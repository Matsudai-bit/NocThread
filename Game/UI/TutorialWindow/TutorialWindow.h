/*****************************************************************//**
 * @file    TutorialWindow.h
 * @brief   チュートリアルウィンドウに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/16
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/UserInterfaceTool/Sprite/ISprite2D.h"


#include <memory>
#include <vector>

// クラスの前方宣言 ===================================================
class Sprite;
class ResourceManager;
struct InputEventData;
// クラスの定義 ===============================================================
/**
 * @brief チュートリアルウィンドウ
 */
class TutorialWindow
	: public ISprite2D
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	int m_currentTutorialSpriteIndex; ///< 現在の添え字　（チュートリアルスプライトの位置）

	std::vector<std::unique_ptr<Sprite>> m_tutorialSprites; ///< チュートリアルスプライト

	std::unique_ptr<Sprite> m_backgroundAlpha;	///< 透過背景
	std::unique_ptr<Sprite> m_arrowSprite;		///< 矢印スプライト

	std::function<void()> m_closeWindow;

	bool m_isActive; ///< 見えるかどうか

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	TutorialWindow();

	// デストラクタ
	~TutorialWindow();


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pResourceManager, std::function<void()> closeWindow);
	// 終了処理
	void Finalize();
	// スプライト描画
	void DrawSprite(DirectX::SpriteBatch* pSpriteBatch) override;

	// 右が動かす
	void OnMoveUpRight(InputEventData data);
	// 左が動かす
	void OnMoveDownLeft(InputEventData data);
	// 選択する
	void OnSelect(InputEventData data);


// 取得/設定
public:
	// 見えるかどうか
	void SetVisible(bool isVisible) { m_isActive = isVisible; }
	bool IsVisible() const override { return m_isActive; }


// 内部実装
private:



};
