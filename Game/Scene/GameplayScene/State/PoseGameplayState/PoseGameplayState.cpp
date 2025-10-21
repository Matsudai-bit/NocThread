/*****************************************************************//**
 * @file    PoseGameplayState.h
 * @brief   ゲームプレイシーンのポーズ状態に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PoseGameplayState.h"


#include "Game/Scene/GameplayScene/GameplayScene.h"

#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Game/Common/Screen.h"
#include "Game/Common/DeviceResources.h"
#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"


#include "Game/UI/TutorialWindow/TutorialWindow.h"

// シーン
#include "Game/Scene/TitleScene/TitleScene.h"

// 状態
#include "Game/Scene/GameplayScene/State/NormalGameplayState/NormalGameplayState.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PoseGameplayState::PoseGameplayState()
	: m_isDisplayingTutorialWindow{ false }
{

}



/**
 * @brief デストラクタ
 */
PoseGameplayState::~PoseGameplayState()
{
	
}

/**
 * @brief 開始処理
 * 
 */
void PoseGameplayState::OnStartState()
{
	using namespace SimpleMath;

	auto resourceManager	= GetOwner()->GetCommonResources()->GetResourceManager();
	auto context			= GetOwner()->GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto screen				= Screen::Get();
	// キャンバスの作成
	m_canvas = std::make_unique<Canvas>();
	m_canvas->Initialize(context);

	// スプライトの作成
	m_backgroundAlphaSprite = std::make_unique<Sprite>();
	m_poseFontSprite		= std::make_unique<Sprite>();
	m_operatingFontSprite	= std::make_unique<Sprite>();
	m_operatingSprite		= std::make_unique<Sprite>();

	// キャンバスへ登録
	m_canvas->AddSprite(m_backgroundAlphaSprite.get());
	m_canvas->AddSprite(m_poseFontSprite.get());
	m_canvas->AddSprite(m_operatingFontSprite.get());
	m_canvas->AddSprite(m_operatingSprite.get());


	// スプライトのテクスチャ設定
	m_backgroundAlphaSprite	->Initialize(resourceManager->CreateTexture("Pose/pose_alpha.dds"));
	m_poseFontSprite		->Initialize(resourceManager->CreateTexture("Pose/pose_font_pose.dds"));
	m_operatingFontSprite	->Initialize(resourceManager->CreateTexture("Pose/pose_font_operating.dds"));
	m_operatingSprite		->Initialize(resourceManager->CreateTexture("Pose/pose_operating.dds"));


	// スプライトの座標設定
	m_backgroundAlphaSprite	->SetPosition(Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	m_poseFontSprite		->SetPosition(Vector2(screen->GetLeftF() + 160.0f * screen->GetScreenScale(), screen->GetBottomF() - 80.0f * screen->GetScreenScale()));
	m_operatingFontSprite	->SetPosition(Vector2(screen->GetRightF() - 450.0f * screen->GetScreenScale(), screen->GetTopF() + 80.0f * screen->GetScreenScale()));
	m_operatingSprite		->SetPosition(Vector2(screen->GetRightF() - 300.0f * screen->GetScreenScale(), screen->GetCenterYF() +40.0f * screen->GetScreenScale()));

	// スプライトの拡大率設定
	m_backgroundAlphaSprite	->SetScale(1.4f * screen->GetScreenScale());
	m_poseFontSprite		->SetScale(0.35f * screen->GetScreenScale());
	m_operatingFontSprite	->SetScale(0.35f * screen->GetScreenScale());
	m_operatingSprite		->SetScale(0.275f * screen->GetScreenScale());
	
	// 線の設定
	m_lines.resize(2);

	for (int i = 0; i < m_lines.size(); i++)
	{
		m_lines[i] = std::make_unique<Line2D>();
	}

	m_lines[0]->Initialize(GetOwner()->GetCommonResources()->GetDeviceResources());
	m_lines[1]->Initialize(GetOwner()->GetCommonResources()->GetDeviceResources());

	m_lines[0]->SetAngle(XMConvertToRadians(70.0f));
	m_lines[1]->SetAngle(XMConvertToRadians(60.0f));

	m_lines[0]->SetPosition(Vector2(screen->GetCenterXF() - 400.0f * screen->GetScreenScale(), screen->GetCenterYF()));
	m_lines[1]->SetPosition(Vector2(screen->GetCenterXF() + 100.0f * screen->GetScreenScale(), screen->GetCenterYF()));

	m_lines[0]->SetLength(1000.0f * screen->GetScreenScale());
	m_lines[1]->SetLength(1000.0f * screen->GetScreenScale());

	m_lines[0]->SetThickness(2.0f * screen->GetScreenScale());
	m_lines[1]->SetThickness(2.0f * screen->GetScreenScale());

	// ポーズメニューの作成
	m_poseMenu = std::make_unique<PoseMenu>();
	m_poseMenu->Initialize(m_canvas.get(), GetOwner()->GetCommonResources(), [&](PoseMenu::MenuItem pushItem) {
		OnPushMenuItem(pushItem); });

	// チュートリアルウィンドウの作成
	m_tutorialWindow = std::make_unique<TutorialWindow>();
	m_tutorialWindow->Initialize(resourceManager, [this]() {OnCloseTutorialWindow(); });

	// 入力の作成
	m_systemInput = InputBindingFactory::CreateSystemInput();
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime
 */
void PoseGameplayState::OnUpdate(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	// 入力の更新処理
	m_systemInput->Update(
		GetOwner()->GetCommonResources()->GetKeyboardTracker(),
		GetOwner()->GetCommonResources()->GetMouseTracker(),
		GetOwner()->GetCommonResources()->GetGamePadTracker());

	auto keyboardStateTracker = GetOwner()->GetCommonResources()->GetKeyboardTracker();



	if (m_isDisplayingTutorialWindow == false)
	{
		// ポーズメニューの更新処理
		m_poseMenu->Update(deltaTime);
		if (m_systemInput->IsInput(InputActionType::SystemActionID::PAUSE, InputSystem<InputActionType::SystemActionID>::InputOption::PRESSED))
		{
			// ゲームに戻る
			ContinueGame();
		}
	}
	else
	{
		// チュートリアルウィンドウの更新処理
		m_tutorialWindow->Update(
			deltaTime,
			GetOwner()->GetCommonResources()->GetKeyboardTracker(),
			GetOwner()->GetCommonResources()->GetMouseTracker(), 
			GetOwner()->GetCommonResources()->GetGamePadTracker());
	}
}


/**
 * @brief 描画処理
 * 
 */
void PoseGameplayState::OnDraw()
{
	auto states = GetOwner()->GetCommonResources()->GetCommonStates();
	auto screen = Screen::Get();

	GetOwner()->DrawInGameObjects();



	// キャンバスの描画
	m_canvas->Draw(states);



	if (m_isDisplayingTutorialWindow == false)
	{
		m_poseMenu->Draw();

		for (auto& line : m_lines)
		{
			line->Draw(states, screen->GetWidthF(), screen->GetHeightF());
		}
	}

	
}

/**
 * @brief メニューアイテムを押したときに呼ばれる
 * 
 * @param[in] menuItem　押されたメニューアイテム
 */
void PoseGameplayState::OnPushMenuItem(PoseMenu::MenuItem menuItem)
{
	switch (menuItem)
	{
	case PoseMenu::MenuItem::CONTINUE:
		ContinueGame();
		break;
	case PoseMenu::MenuItem::TUTORIAL:
		m_isDisplayingTutorialWindow = true;
		m_canvas->AddSprite(m_tutorialWindow.get());
		break;
	case PoseMenu::MenuItem::SETTING:
		break;
	case PoseMenu::MenuItem::RETURN_TITLE:
		GetOwner()->ChangeScene<TitleScene>();

		break;
	default:
		break;
	}
}

/**
 * @brief ゲームに戻る
 * 
 */
void PoseGameplayState::ContinueGame()
{
	GetStateMachine()->ChangeState<NormalGameplayState>();
}

/**
 * @brief チュートリアルウィンドウを閉じる際の処理
 */
void PoseGameplayState::OnCloseTutorialWindow()
{
	m_isDisplayingTutorialWindow = false;
	m_canvas->RemoveSprite(m_tutorialWindow.get());
}