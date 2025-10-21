/*****************************************************************//**
 * @file    PoseMenu.h
 * @brief   ポーズメニューに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/14
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

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PoseMenu::PoseMenu()
	: m_currentSelectItemForInt{}
	, m_pCommonResources{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
PoseMenu::~PoseMenu()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void PoseMenu::Initialize(Canvas* pCanvas, const CommonResources* pCommonResources, std::function<void(MenuItem)> pushButtonFunc)
{
	auto screen = Screen::Get();
	
	m_poseFontSprites.resize(static_cast<int>(MenuItem::NUM));
	std::for_each(m_poseFontSprites.begin(), m_poseFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite)
		{sprite = std::make_unique<Sprite>(); }
	);

	// フォント類
	m_poseFontSprites[0]->Initialize(pCommonResources->GetResourceManager()->CreateTexture("Pose/pose_font_continue.dds"));
	m_poseFontSprites[1]->Initialize(pCommonResources->GetResourceManager()->CreateTexture("Pose/pose_font_tutorial.dds"));
	m_poseFontSprites[2]->Initialize(pCommonResources->GetResourceManager()->CreateTexture("Pose/pose_font_setting.dds"));
	m_poseFontSprites[3]->Initialize(pCommonResources->GetResourceManager()->CreateTexture("Pose/pose_font_returnTitle.dds"));

	// キャンバスにスプライトの登録
	std::for_each(m_poseFontSprites.begin(), m_poseFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite) {pCanvas->AddSprite(sprite.get()); });

	// フォント類の各種設定
	std::for_each(m_poseFontSprites.begin(), m_poseFontSprites.end(), [&](std::unique_ptr<Sprite>& sprite)
		{sprite->SetScale(0.30f * screen->GetScreenScale()); });

	// 画面のスケールと余白の計算
	auto margin = SimpleMath::Vector2(70.0f, 00.0f) * screen->GetScreenScale();
	// スプライト群全体の高さを計算
	float totalHeight = (m_poseFontSprites[0]->GetSpriteHeight() * m_poseFontSprites.size()) + (margin.y * (m_poseFontSprites.size() - 1));
	// スプライト群の中心が画面中央に来るように、最初のスプライトのY座標の始点を計算
	float displayOriginY = screen->GetCenterYF() - (totalHeight / 2.0f);

	auto fixedMargin = SimpleMath::Vector2(350.0f,10.0f ) * screen->GetScreenScale();

	// 座標の設定 (ループは簡潔に)
	for (int i = 0; i < m_poseFontSprites.size(); i++)
	{
		// 各スプライトのY座標を計算
		auto position = SimpleMath::Vector2(screen->GetLeftF()+ fixedMargin.x + margin.x * i, displayOriginY + (m_poseFontSprites[i]->GetSpriteHeight() + margin.y ) * i + fixedMargin.y);
		m_poseFontSprites[i]->SetPosition(position);
	}

	m_poseFontSprites.back()->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + (fixedMargin.x) + 240.0f * screen->GetScreenScale(), (displayOriginY) + 450.0f * screen->GetScreenScale()) );

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
void PoseMenu::Update(float deltaTime)
{
	// 入力の更新処理
	m_uiInput->Update(
		m_pCommonResources->GetKeyboardTracker(),
		m_pCommonResources->GetMouseTracker(),
		m_pCommonResources->GetGamePadTracker());

	// 加算
	m_ElapsedTimeCounter.UpperTime(deltaTime);

	Sprite* currentSelectElementOfSprite = m_poseFontSprites[m_currentSelectItemForInt].get();

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
void PoseMenu::Draw()
{
	auto screen = Screen::Get();

	Sprite* currentSelectElementOfSprite = m_poseFontSprites[m_currentSelectItemForInt].get();

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
void PoseMenu::Finalize()
{

}

/**
 * @brief 下にセレクターが動くことが出来るかどうか
 * 
 * @return true 可能
 */
bool PoseMenu::CanMoveDownSelector() const
{
	auto keyboardTracker = m_pCommonResources->GetKeyboardTracker();

	return (m_uiInput->IsInput(InputActionType::UIActionID::DOWN_MOVE, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}

/**
 * @brief 上にセレクターが動くことが出来るかどうか
 * 
 * @return true 可能
 */
bool PoseMenu::CanMoveUpSelector() const
{
	auto keyboardTracker = m_pCommonResources->GetKeyboardTracker();

	return (m_uiInput->IsInput(InputActionType::UIActionID::UP_MOVE, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}

/**
 * @brief 選択することが出来るかどうか
 * 
 * @return true 可能
 */
bool PoseMenu::CanPush() const
{
	auto keyboardTracker = m_pCommonResources->GetKeyboardTracker();

	return (m_uiInput->IsInput(InputActionType::UIActionID::CONFIRM, InputSystem<InputActionType::UIActionID>::InputOption::PRESSED));
}
