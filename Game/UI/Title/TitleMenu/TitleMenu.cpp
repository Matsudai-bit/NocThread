/*****************************************************************//**
 * @file    TitleMenu.h
 * @brief   タイトルメニューに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/14
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TitleMenu.h"

#include "Library/MyLib/MathUtils/MathUtils.h"
#include "Game/Common/Screen.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"

#include "Library/MyLib/EasingKit/EasingKit.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TitleMenu::TitleMenu()
	: m_currentSelectItemForInt{}
	, m_pCommonResources{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
TitleMenu::~TitleMenu()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleMenu::Initialize(Canvas* pCanvas, const CommonResources* pCommonResources, std::function<void(MenuItem)> pushButtonFunc)
{
	auto screen = Screen::Get();
	
	m_titleFontSprites.resize(static_cast<int>(MenuItem::NUM));
	std::for_each(m_titleFontSprites.begin(), m_titleFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite)
		{sprite = std::make_unique<Sprite>(); }
	);

	// フォント類
	m_titleFontSprites[0]->Initialize(pCommonResources->GetResourceManager()->CreateTexture("Title/title_font_play.dds"));
	m_titleFontSprites[1]->Initialize(pCommonResources->GetResourceManager()->CreateTexture("Title/title_font_tutorial.dds"));
	m_titleFontSprites[2]->Initialize(pCommonResources->GetResourceManager()->CreateTexture("Title/title_font_setting.dds"));
	m_titleFontSprites[3]->Initialize(pCommonResources->GetResourceManager()->CreateTexture("Title/title_font_quit.dds"));

	// キャンバスにスプライトの登録
	std::for_each(m_titleFontSprites.begin(), m_titleFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite) {pCanvas->AddSprite(sprite.get()); });

	// フォント類の各種設定
	std::for_each(m_titleFontSprites.begin(), m_titleFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite)
		{sprite->SetScale(0.35f * screen->GetScreenScale()); });

	// 画面のスケールと余白の計算
	auto margin = SimpleMath::Vector2(280.0f, 20.0f) * screen->GetScreenScale();
	// スプライト群全体の高さを計算
	float totalHeight = (m_titleFontSprites[0]->GetSpriteHeight() * m_titleFontSprites.size()) + (margin.y * (m_titleFontSprites.size() - 1));
	// スプライト群の中心が画面中央に来るように、最初のスプライトのY座標の始点を計算
	float displayOriginY = screen->GetCenterYF() - (totalHeight / 2.0f);

	float fixedMarginY = 30.0f * screen->GetScreenScale();

	// 座標の設定 (ループは簡潔に)
	for (int i = 0; i < m_titleFontSprites.size(); i++)
	{
		// 各スプライトのY座標を計算
		auto position = SimpleMath::Vector2(screen->GetLeftF() + margin.x, displayOriginY + (m_titleFontSprites[i]->GetSpriteHeight() + margin.y ) * i + fixedMarginY);
		m_titleFontSprites[i]->SetPosition(position);
	}

	m_pushButtonFunc = pushButtonFunc;

	m_pCommonResources = pCommonResources;

	m_line.Initialize(pCommonResources->GetDeviceResources());

	m_ElapsedTimeCounter.Reset();

	// UI入力システムの作成
	m_uiInput = InputBindingFactory::CreateUIInput();
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void TitleMenu::Update(float deltaTime)
{
	// 入力の更新処理
	m_uiInput->Update(m_pCommonResources->GetKeyboardTracker(), m_pCommonResources->GetMouseTracker(), m_pCommonResources->GetGamePadTracker());

	// 加算
	m_ElapsedTimeCounter.UpperTime(deltaTime);

	Sprite* currentSelectElementOfSprite = m_titleFontSprites[m_currentSelectItemForInt].get();

	float ratio = MyLib::EaseOutSine(m_ElapsedTimeCounter.GetdeltaTime() / EASING_TIME );
	float width = static_cast<float>(currentSelectElementOfSprite->GetSpriteWidth()) + 20.0f * Screen::Get()->GetScreenScale();

	width = MAX_SELECTOR_LENGTH;
	float length = width * 0.2f + (width - width * 0.2f) * Screen::Get()->GetScreenScale() * ratio;

	if (m_ElapsedTimeCounter.GetdeltaTime() >= EASING_TIME)
	{
		m_ElapsedTimeCounter.Reset();
	}
	m_line.SetLength(length);



	if (CanMoveDownSelector())
	{
		// 減算する
		m_currentSelectItemForInt++;
	}

	if (CanMoveUpSelector())
	{
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
void TitleMenu::Draw()
{
	auto screen = Screen::Get();

	Sprite* currentSelectElementOfSprite = m_titleFontSprites[m_currentSelectItemForInt].get();

	SimpleMath::Vector2 cursorPosition = currentSelectElementOfSprite->GetPosition();

	cursorPosition.y = cursorPosition.y + currentSelectElementOfSprite->GetSpriteHeight() / 2.0f - 30.0f * screen->GetScreenScale();

	//m_line.SetLength(300.0f * screen->GetScreenScale());
	m_line.SetThickness(2.0f * screen->GetScreenScale());
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
void TitleMenu::Finalize()
{

}

/**
 * @brief 下にセレクターが動くことが出来るかどうか
 * 
 * @return true 可能
 */
bool TitleMenu::CanMoveDownSelector() const
{

	return (m_uiInput->IsInput(InputActionType::UIActionID::DOWN_MOVE, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}

/**
 * @brief 上にセレクターが動くことが出来るかどうか
 * 
 * @return true 可能
 */
bool TitleMenu::CanMoveUpSelector() const
{

	return (m_uiInput->IsInput(InputActionType::UIActionID::UP_MOVE, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}

/**
 * @brief 選択することが出来るかどうか
 * 
 * @return true 可能
 */
bool TitleMenu::CanPush() const
{
	return (m_uiInput->IsInput(InputActionType::UIActionID::CONFIRM, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}
