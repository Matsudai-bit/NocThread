/*****************************************************************//**
 * @file   TitleMenu.h
 * @brief  タイトルメニューに関するソースファイル
 *
 * @author 松下大暉
 * @date   2025/09/14
 *********************************************************************/

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TitleMenu.h"

// ライブラリ関連
#include "Library/MyLib/MathUtils/MathUtils.h"
#include "Library/MyLib/EasingKit/EasingKit.h"

// データベース関連
#include "Game/Common/Database/SoundDatabase.h"
#include "Game/Common/Database/TextureDatabase.h"

// ファクトリー関連
#include "Game/Common/Factory/InputBindingFactory/InputBindingFactory.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/SoundManager/SoundManager.h"
#include "Game/Common/Framework/Input/InputActionMap/InputActionMap.h"

// グラフィック関連
#include "Game/Common/Screen.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

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
	, m_isActive{ false }
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

	using namespace TextureDatabase;
	// フォント類
	m_titleFontSprites[0]->Initialize(pCommonResources->GetResourceManager()->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_TITLE_FONT_PLAY)));
	m_titleFontSprites[1]->Initialize(pCommonResources->GetResourceManager()->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_TITLE_FONT_TUTORIAL)));
	m_titleFontSprites[2]->Initialize(pCommonResources->GetResourceManager()->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_TITLE_FONT_SETTING)));
	m_titleFontSprites[3]->Initialize(pCommonResources->GetResourceManager()->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_TITLE_FONT_QUIT)));

	// キャンバスにスプライトの登録
	std::for_each(m_titleFontSprites.begin(), m_titleFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite) {pCanvas->AddSprite(sprite.get()); });

	// フォント類の各種設定
	std::for_each(m_titleFontSprites.begin(), m_titleFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite)
		{sprite->SetScale(FONT_SPRITE_SCALE * screen->GetScreenScale()); });

	// 画面のスケールと余白の計算
	auto margin = SimpleMath::Vector2(MENU_MARGIN_X, MENU_MARGIN_Y) * screen->GetScreenScale();
	// スプライト群全体の高さを計算
	float totalHeight = (m_titleFontSprites[0]->GetSpriteHeight() * m_titleFontSprites.size()) + (margin.y * (m_titleFontSprites.size() - 1));
	// スプライト群の中心が画面中央に来るように、最初のスプライトのY座標の始点を計算
	float displayOriginY = screen->GetCenterYF() - (totalHeight / 2.0f);

	float fixedMarginY = FIXED_POS_Y_OFFSET * screen->GetScreenScale();

	// 座標の設定 (ループは簡潔に)
	for (int i = 0; i < m_titleFontSprites.size(); i++)
	{
		// 各スプライトのY座標を計算
		auto position = SimpleMath::Vector2(screen->GetLeftF() + margin.x, displayOriginY + (m_titleFontSprites[i]->GetSpriteHeight() + margin.y) * i + fixedMarginY);
		m_titleFontSprites[i]->SetPosition(position);
	}

	m_pushButtonFunc = pushButtonFunc;

	m_pCommonResources = pCommonResources;

	m_line.Initialize(pCommonResources->GetDeviceResources());

	m_ElapsedTimeCounter.Reset();

	// コールバックの登録
	//RegisterInputCallback();

	m_isActive = true;
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
	// 加算
	m_ElapsedTimeCounter.UpperTime(deltaTime);

	Sprite* currentSelectElementOfSprite = m_titleFontSprites[m_currentSelectItemForInt].get();

	float ratio = MyLib::EaseOutSine(m_ElapsedTimeCounter.GetElapsedTime() / EASING_TIME);
	float width = static_cast<float>(currentSelectElementOfSprite->GetSpriteWidth()) + SELECTOR_WIDTH_OFFSET * Screen::Get()->GetScreenScale();

	width = MAX_SELECTOR_LENGTH;
	float length = width * SELECTOR_MIN_LENGTH_RATIO + (width - width * SELECTOR_MIN_LENGTH_RATIO) * Screen::Get()->GetScreenScale() * ratio;

	if (m_ElapsedTimeCounter.GetElapsedTime() >= EASING_TIME)
	{
		m_ElapsedTimeCounter.Reset();
	}
	m_line.SetLength(length);

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

	if (!m_isActive) { return; }

	Sprite* currentSelectElementOfSprite = m_titleFontSprites[m_currentSelectItemForInt].get();

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
void TitleMenu::Finalize()
{

}

void TitleMenu::OnMoveUpSelector(const InputEventData& data)
{
	if (data.inputOption.pressed && m_isActive)
	{
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_CURSOR_MOVING), false);
		// 減算する
		m_currentSelectItemForInt--;
		// 最後にクランプする
		m_currentSelectItemForInt = (m_currentSelectItemForInt + static_cast<int>(MenuItem::NUM)) % static_cast<int>(MenuItem::NUM);
	}

}

void TitleMenu::OnMoveDownSelector(const InputEventData& data)
{
	if (data.inputOption.pressed && m_isActive)
	{
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_CURSOR_MOVING), false);
		// 加算する
		m_currentSelectItemForInt++;
		// 最後にクランプする
		m_currentSelectItemForInt = (m_currentSelectItemForInt + static_cast<int>(MenuItem::NUM)) % static_cast<int>(MenuItem::NUM);
	}
}

void TitleMenu::OnSelect(const InputEventData& data)
{
	if (data.inputOption.pressed && m_isActive && m_pushButtonFunc)
	{
		m_pushButtonFunc(static_cast<MenuItem>(m_currentSelectItemForInt));
	}
}

