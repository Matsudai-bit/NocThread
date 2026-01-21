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

#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"
#include <Game/Common/SoundManager/SoundManager.h>

// データベース関連
#include "Game/Common/Database/SoundDatabase.h"
#include "Game/Common/Database/TextureDatabase.h"
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

	const int TUTORIAL_WINDOW_COUNT = 8;

	// 閉じるときに実行するもの
	m_closeWindow = closeWindow;

	// スプライトの作成
	m_backgroundAlpha	= std::make_unique<Sprite>();
	m_arrowSprite		= std::make_unique<Sprite>();
	using namespace TextureDatabase;

	TextureID tutorialTextures[TUTORIAL_WINDOW_COUNT] = {
		TextureID::UI_TUTORIAL_WINDOW_1,
		TextureID::UI_TUTORIAL_WINDOW_2,
		TextureID::UI_TUTORIAL_WINDOW_3,
		TextureID::UI_TUTORIAL_WINDOW_4,
		TextureID::UI_TUTORIAL_WINDOW_5,
		TextureID::UI_TUTORIAL_WINDOW_6,
		TextureID::UI_TUTORIAL_WINDOW_7,
		TextureID::UI_TUTORIAL_WINDOW_8,
	};

	m_tutorialSprites.resize(TUTORIAL_WINDOW_COUNT);
	auto screen = Screen::Get();
	for (int i = 0; i < m_tutorialSprites.size(); i++)
	{
		m_tutorialSprites[i] = std::make_unique<Sprite>();

		m_tutorialSprites[i]->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(tutorialTextures[i])));
		m_tutorialSprites[i]->SetScale(0.45f * screen->GetScreenScale());
	}

	m_backgroundAlpha	->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_ALPHA_MASK)));
	m_arrowSprite		->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_TUTORIAL_ARROW)));

	// UI入力の作成
	m_uiInput = InputBindingFactory::UIInputFactory().Create(DefaultSpawnDesc());
}

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void TutorialWindow::Update(
	float deltaTime, 
	const DirectX::Keyboard::KeyboardStateTracker* pKeyboardStateTracker,
	const DirectX::Mouse::ButtonStateTracker* pMouseStateTracker,
	const DirectX::GamePad::ButtonStateTracker* pGamePadStateTracker)
{
	UNREFERENCED_PARAMETER(deltaTime);

	m_uiInput->Update(pKeyboardStateTracker, pMouseStateTracker, pGamePadStateTracker);

	if (CanMoveRight())
	{
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_CURSOR_MOVING), false);	
		m_currentTutorialSpriteIndex++;
	}
	if (CanMoveLeft())
	{
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_CURSOR_MOVING), false);
		m_currentTutorialSpriteIndex--;
	}

	// 最後にクランプする
	int max = static_cast<int>(m_tutorialSprites.size());
	m_currentTutorialSpriteIndex = (m_currentTutorialSpriteIndex + static_cast<int>(max)) % static_cast<int>(max);

	// 終了するかどうか
	if (m_uiInput->IsInput(InputActionType::UIActionID::CONFIRM, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED) ||
		m_uiInput->IsInput(InputActionType::UIActionID::CANCEL, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED))
	{
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_CANCEL), false);		
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
 * @brief 右にセレクターが動くことが出来るかどうか
 *
 * @return true 可能
 */
bool TutorialWindow::CanMoveRight() const
{

	return (m_uiInput->IsInput(InputActionType::UIActionID::RIGHT_MOVE, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}

/**
 * @brief 左にセレクターが動くことが出来るかどうか
 *
 * @return true 可能
 */
bool TutorialWindow::CanMoveLeft() const
{

	return (m_uiInput->IsInput(InputActionType::UIActionID::LEFT_MOVE, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}

