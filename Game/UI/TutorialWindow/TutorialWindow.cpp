/*****************************************************************//**
 * @file    TutorialWindow.h
 * @brief   チュートリアルウィンドウに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/16
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TutorialWindow.h"

#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Game/Common/Screen.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TutorialWindow::TutorialWindow()
	: m_currentTutorialSpriteIndex{}
{

}



/**
 * @brief デストラクタ
 */
TutorialWindow::~TutorialWindow()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TutorialWindow::Initialize(ResourceManager* pResourceManager, std::function<void()> closeWindow)
{
	using namespace SimpleMath;

	// 閉じるときに実行するもの
	m_closeWindow = closeWindow;

	// スプライトの作成
	m_backgroundAlpha	= std::make_unique<Sprite>();
	m_arrowSprite		= std::make_unique<Sprite>();

	m_tutorialSprites.resize(5);
	for (int i = 0; i < m_tutorialSprites.size(); i++)
	{
		m_tutorialSprites[i] = std::make_unique<Sprite>();
	}

	m_backgroundAlpha	->Initialize(pResourceManager->CreateTexture("Tutorial/tutorial_alpha.dds"));
	m_arrowSprite		->Initialize(pResourceManager->CreateTexture("Tutorial/tutorial_arrow.dds"));

	m_tutorialSprites[0]->Initialize(pResourceManager->CreateTexture("Tutorial/tutorial_gameFlow_1.dds"));
	m_tutorialSprites[1]->Initialize(pResourceManager->CreateTexture("Tutorial/tutorial_gameFlow_2.dds"));
	m_tutorialSprites[2]->Initialize(pResourceManager->CreateTexture("Tutorial/tutorial_gameFlow_3.dds"));
	m_tutorialSprites[3]->Initialize(pResourceManager->CreateTexture("Tutorial/tutorial_point_1.dds"));
	m_tutorialSprites[4]->Initialize(pResourceManager->CreateTexture("Tutorial/tutorial_point_2.dds"));
	auto screen = Screen::Get();

	m_tutorialSprites[0]->SetScale(0.6f * screen->GetScreenScale());
	m_tutorialSprites[1]->SetScale(0.6f * screen->GetScreenScale());
	m_tutorialSprites[2]->SetScale(0.6f * screen->GetScreenScale());
	m_tutorialSprites[3]->SetScale(1.0f * screen->GetScreenScale());
	m_tutorialSprites[4]->SetScale(1.0f * screen->GetScreenScale());
}

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void TutorialWindow::Update(float deltaTime, const DirectX::Keyboard::KeyboardStateTracker& keyboardStateTracker)
{
	UNREFERENCED_PARAMETER(deltaTime);

	if (CanMoveRight(keyboardStateTracker))
	{
		m_currentTutorialSpriteIndex++;
	}
	if (CanMoveLeft(keyboardStateTracker))
	{
		m_currentTutorialSpriteIndex--;
	}

	// 最後にクランプする
	int max = static_cast<int>(m_tutorialSprites.size());
	m_currentTutorialSpriteIndex = (m_currentTutorialSpriteIndex + static_cast<int>(max)) % static_cast<int>(max);

	// 終了するかどうか
	if (keyboardStateTracker.IsKeyPressed(Keyboard::Escape) ||
		keyboardStateTracker.IsKeyPressed(Keyboard::Space) ||
		keyboardStateTracker.IsKeyPressed(Keyboard::Enter))
	{
		m_closeWindow();
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TutorialWindow::Draw()
{

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TutorialWindow::Finalize()
{

}

/**
 * @brief スプライト描画
 * 
 * @param[in] pSpriteBatch
 */
void TutorialWindow::DrawSprite(DirectX::SpriteBatch* pSpriteBatch)
{
	auto screen = Screen::Get();


	// 背景描画
	m_backgroundAlpha->SetPosition(SimpleMath::Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	m_backgroundAlpha->DrawSprite(pSpriteBatch);

	// 矢印の描画
	m_arrowSprite->SetScale(0.3f * screen->GetScreenScale());

	m_arrowSprite->SetAngle(XMConvertToRadians(0.0f));
	m_arrowSprite->SetPosition(SimpleMath::Vector2(screen->GetCenterXF() - 500.0f * screen->GetScreenScale(), screen->GetCenterYF()));
	m_arrowSprite->DrawSprite(pSpriteBatch);

	m_arrowSprite->SetAngle(XMConvertToRadians(180.0f));
	m_arrowSprite->SetPosition(SimpleMath::Vector2(screen->GetCenterXF() + 500.0f * screen->GetScreenScale(), screen->GetCenterYF()));
	m_arrowSprite->DrawSprite(pSpriteBatch);


	// チュートリアルスプライトの描画
	auto tutorialSprite = m_tutorialSprites[m_currentTutorialSpriteIndex].get();

	tutorialSprite->SetPosition(SimpleMath::Vector2(screen->GetCenterXF(), screen->GetCenterYF()));

	tutorialSprite->DrawSprite(pSpriteBatch);
}

/**
 * @brief 下にセレクターが動くことが出来るかどうか
 *
 * @return true 可能
 */
bool TutorialWindow::CanMoveRight(const DirectX::Keyboard::KeyboardStateTracker& keyboardStateTracker) const
{

	return (keyboardStateTracker.IsKeyPressed(Keyboard::Right) || keyboardStateTracker.IsKeyPressed(Keyboard::D));
}

/**
 * @brief 上にセレクターが動くことが出来るかどうか
 *
 * @return true 可能
 */
bool TutorialWindow::CanMoveLeft(const DirectX::Keyboard::KeyboardStateTracker& keyboardStateTracker) const
{

	return (keyboardStateTracker.IsKeyPressed(Keyboard::Left) || keyboardStateTracker.IsKeyPressed(Keyboard::A));
}

