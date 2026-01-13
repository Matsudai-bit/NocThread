/*****************************************************************//**
 * @file   PauseMenu.h
 * @brief  ポーズメニューに関するソースファイル
 *
 * @author 松下大暉
 * @date   2025/09/14
 *********************************************************************/

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PoseMenu.h"


#include "Library/MyLib/MathUtils/MathUtils.h"
#include "Game/Common/Screen.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"

#include "Library/MyLib/EasingKit/EasingKit.h"
#include <Game\Common\SoundManager\SoundManager.h>
#include "Game/Common/Database/SoundDatabase.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PauseMenu::PauseMenu()
	: m_currentSelectItemForInt{}
	, m_pCommonResources{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
PauseMenu::~PauseMenu()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PauseMenu::Initialize(Canvas* pCanvas, const CommonResources* pCommonResources, std::function<void(MenuItem)> pushButtonFunc)
{

	auto screen = Screen::Get();

	m_pauseFontSprites.resize(static_cast<int>(MenuItem::NUM));
	std::for_each(m_pauseFontSprites.begin(), m_pauseFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite)
		{sprite = std::make_unique<Sprite>(); }
	);

	// フォント類
	m_pauseFontSprites[0]->Initialize(pCommonResources->GetResourceManager()->CreateTexture(TEXTURE_PATH_CONTINUE));
	m_pauseFontSprites[1]->Initialize(pCommonResources->GetResourceManager()->CreateTexture(TEXTURE_PATH_TUTORIAL));
	m_pauseFontSprites[2]->Initialize(pCommonResources->GetResourceManager()->CreateTexture(TEXTURE_PATH_SETTING));
	m_pauseFontSprites[3]->Initialize(pCommonResources->GetResourceManager()->CreateTexture(TEXTURE_PATH_RETURN_TITLE));

	// キャンバスにスプライトの登録
	std::for_each(m_pauseFontSprites.begin(), m_pauseFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite) {pCanvas->AddSprite(sprite.get()); });

	// フォント類の各種設定
	std::for_each(m_pauseFontSprites.begin(), m_pauseFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite)
		{sprite->SetScale(FONT_SPRITE_SCALE * screen->GetScreenScale()); });

	// 画面のスケールと余白の計算
	auto margin = SimpleMath::Vector2(MENU_MARGIN_X, MENU_MARGIN_Y) * screen->GetScreenScale();
	// スプライト群全体の高さを計算
	float totalHeight = (m_pauseFontSprites[0]->GetSpriteHeight() * m_pauseFontSprites.size()) + (margin.y * (m_pauseFontSprites.size() - 1));
	// スプライト群の中心が画面中央に来るように、最初のスプライトのY座標の始点を計算
	float displayOriginY = screen->GetCenterYF() - (totalHeight / 2.0f);

	auto fixedMargin = SimpleMath::Vector2(FIXED_POS_X_OFFSET, FIXED_POS_Y_OFFSET) * screen->GetScreenScale();

	// 座標の設定 (ループは簡潔に)
	for (int i = 0; i < m_pauseFontSprites.size(); i++)
	{
		// 各スプライトのY座標を計算
		auto position = SimpleMath::Vector2(screen->GetLeftF() + fixedMargin.x + margin.x * i, displayOriginY + (m_pauseFontSprites[i]->GetSpriteHeight() + margin.y) * i + fixedMargin.y);
		m_pauseFontSprites[i]->SetPosition(position);
	}

	m_pauseFontSprites.back()->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + (fixedMargin.x) + RETURN_TITLE_OFFSET_X * screen->GetScreenScale(), (displayOriginY)+RETURN_TITLE_OFFSET_Y * screen->GetScreenScale()));

	// ボタンを押した際の処理を設定
	m_pushButtonFunc = pushButtonFunc;

	m_pCommonResources = pCommonResources;

	m_line.Initialize(pCommonResources->GetDeviceResources());

	m_ElapsedTimeCounter.Reset();

	// 入力の作成
	m_uiInput = InputBindingFactory::CreateUIInput();
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void PauseMenu::Update(float deltaTime)
{

	// 入力の更新処理
	m_uiInput->Update(
		m_pCommonResources->GetKeyboardTracker(),
		m_pCommonResources->GetMouseTracker(),
		m_pCommonResources->GetGamePadTracker());

	// 加算
	m_ElapsedTimeCounter.UpperTime(deltaTime);

	Sprite* currentSelectElementOfSprite = m_pauseFontSprites[m_currentSelectItemForInt].get();

	float ratio = MyLib::EaseOutSine(m_ElapsedTimeCounter.GetElapsedTime() / EASING_TIME);
	float width = static_cast<float>(currentSelectElementOfSprite->GetSpriteWidth()) + SELECTOR_WIDTH_OFFSET * Screen::Get()->GetScreenScale();

	width = MAX_SELECTOR_LENGTH;
	float length = width * SELECTOR_MIN_LENGTH_RATIO + (width - width * SELECTOR_MIN_LENGTH_RATIO) * Screen::Get()->GetScreenScale() * ratio;

	if (m_ElapsedTimeCounter.GetElapsedTime() >= EASING_TIME)
	{
		m_ElapsedTimeCounter.Reset();
	}
	m_line.SetLength(length);



	if (CanMoveDownSelector())
	{	
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_CURSOR_MOVING), false);
		
		m_currentSelectItemForInt++;
	}

	if (CanMoveUpSelector())
	{
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_CURSOR_MOVING), false);		
		// 加算する
		m_currentSelectItemForInt--;
	}

	// 最後にクランプする
	m_currentSelectItemForInt = (m_currentSelectItemForInt + static_cast<int>(MenuItem::NUM)) % static_cast<int>(MenuItem::NUM);

	if (CanPush() && m_pushButtonFunc)
	{
		m_pushButtonFunc(static_cast<MenuItem>(m_currentSelectItemForInt));
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PauseMenu::Draw()
{

	auto screen = Screen::Get();

	Sprite* currentSelectElementOfSprite = m_pauseFontSprites[m_currentSelectItemForInt].get();

	SimpleMath::Vector2 cursorPosition = currentSelectElementOfSprite->GetPosition();

	cursorPosition.y = cursorPosition.y + currentSelectElementOfSprite->GetSpriteHeight() / 2.0f - SELECTOR_CURSOR_Y_OFFSET * screen->GetScreenScale();

	//m_line.SetLength(300.0f * screen->GetScreenScale());
	m_line.SetThickness(SELECTOR_LINE_THICKNESS * screen->GetScreenScale());
	m_line.SetPosition(cursorPosition);

	m_line.Draw(m_pCommonResources->GetCommonStates(), screen->GetWidthF(), screen->GetHeightF());
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PauseMenu::Finalize()
{

}

/**
 * @brief 下にセレクターが動くことが出来るかどうか
 * * @return true 可能
 */
bool PauseMenu::CanMoveDownSelector() const
{
	return (m_uiInput->IsInput(InputActionType::UIActionID::DOWN_MOVE, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}

/**
 * @brief 上にセレクターが動くことが出来るかどうか
 * * @return true 可能
 */
bool PauseMenu::CanMoveUpSelector() const
{
	return (m_uiInput->IsInput(InputActionType::UIActionID::UP_MOVE, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}

/**
 * @brief 選択することが出来るかどうか
 * * @return true 可能
 */
bool PauseMenu::CanPush() const
{
	return (m_uiInput->IsInput(InputActionType::UIActionID::CONFIRM, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}
