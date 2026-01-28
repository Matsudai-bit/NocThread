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
#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"

#include <memory>
#include <vector>

// クラスの前方宣言 ===================================================
class Sprite;
class ResourceManager;
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

	std::unique_ptr < InputSystem<InputActionType::UIActionID>> m_uiInput; ///< UI入力

	bool m_isVisible; ///< 見えるかどうか

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

	// 更新処理
	void Update(
		float deltaTime,
		const DirectX::Keyboard::KeyboardStateTracker* pKeyboardStateTracker,
		const DirectX::Mouse::ButtonStateTracker* pMouseStateTracker,
		const DirectX::GamePad::ButtonStateTracker* pGamePadStateTracker);
	// 描画処理
	void Draw();
	// 終了処理
	void Finalize();
	// スプライト描画
	void DrawSprite(DirectX::SpriteBatch* pSpriteBatch) override;

	// 見えるかどうか
	void SetVisible(bool isVisible) { m_isVisible = isVisible; }
	bool IsVisible() const override { return m_isVisible; }

// 取得/設定
public:


// 内部実装
private:

	// 右に動くことが出来るかどうか
	bool CanMoveRight() const;
	// 上に動くことが出来るかどうか
	bool CanMoveLeft() const;

};
