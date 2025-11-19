/*****************************************************************//**
 * @file   PoseGameplayState.h
 * @brief  ゲームプレイシーンのポーズ状態に関するソースファイル
 *
 * @author 松下大暉
 * @date   2025/09/15
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

#include "Game/Manager/StageManager/StageManager.h"
#include "Game/Common/GameEffect/GameEffectController.h"
#include "Game/Common/Camera/MainCamera/MainCamera.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PoseGameplayState::PoseGameplayState()
	: m_isDisplayingTutorialWindow{ false }
	, m_isPrevConnectedGamepad{ false }
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
 * */
void PoseGameplayState::OnStartState()
{
	using namespace SimpleMath;

	auto resourceManager = GetOwner()->GetCommonResources()->GetResourceManager();
	auto context = GetOwner()->GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto screen = Screen::Get();
	// ***** キャンバスの作成 *********************************************
	m_canvas = std::make_unique<Canvas>(context, GetOwner()->GetCommonResources()->GetCommonStates());
	m_canvas->SetOt(0);  // 一番手前にする

	// **** スプライトの作成 **********************************************
	m_backgroundAlphaSprite = std::make_unique<Sprite>();
	m_poseFontSprite = std::make_unique<Sprite>();
	m_operatingFontSprite = std::make_unique<Sprite>();
	m_operatingSprite = std::make_unique<Sprite>();
	m_manualSprite = std::make_unique<Sprite>();

	// キャンバスへ登録
	m_canvas->AddSprite(m_backgroundAlphaSprite.get());
	m_canvas->AddSprite(m_poseFontSprite.get());
	m_canvas->AddSprite(m_operatingFontSprite.get());
	m_canvas->AddSprite(m_operatingSprite.get());
	m_canvas->AddSprite(m_manualSprite.get());


	// スプライトのテクスチャ設定
	m_backgroundAlphaSprite	->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_ALPHA));
	m_poseFontSprite		->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_FONT_POSE));
	m_operatingFontSprite	->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_FONT_OPERATING));
	m_operatingSprite		->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_OPERATING_PC));
	m_manualSprite			->Initialize(resourceManager->CreateTexture(TEXTURE_PATH_MANUAL_PC));


	// スプライトの座標設定
	m_backgroundAlphaSprite	->SetPosition(Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	m_poseFontSprite		->SetPosition(Vector2(screen->GetLeftF() + FONT_POSE_POS_X_OFFSET * screen->GetScreenScale(),			screen->GetBottomF() - FONT_POSE_POS_Y_OFFSET * screen->GetScreenScale()));
	m_operatingFontSprite	->SetPosition(Vector2(screen->GetRightF() - FONT_OPERATING_POS_X_OFFSET * screen->GetScreenScale(),		screen->GetTopF() + FONT_OPERATING_POS_Y_OFFSET * screen->GetScreenScale()));
	m_operatingSprite		->SetPosition(Vector2(screen->GetRightF() - OPERATING_UI_POS_X_OFFSET * screen->GetScreenScale(),		screen->GetCenterYF() + OPERATING_UI_POS_Y_OFFSET * screen->GetScreenScale()));
	m_manualSprite			->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + MANUAL_UI_POS_X_OFFSET * screen->GetScreenScale(),screen->GetBottomF() - MANUAL_UI_POS_Y_OFFSET * screen->GetScreenScale()));

	// スプライトの拡大率設定
	m_backgroundAlphaSprite	->SetScale(ALPHA_SPRITE_SCALE * screen->GetScreenScale());
	m_poseFontSprite		->SetScale(FONT_SPRITE_SCALE * screen->GetScreenScale());
	m_operatingFontSprite	->SetScale(FONT_SPRITE_SCALE * screen->GetScreenScale());
	m_operatingSprite		->SetScale(OPERATING_SPRITE_SCALE_PC * screen->GetScreenScale());
	m_manualSprite			->SetScale(MANUAL_SPRITE_SCALE * screen->GetScreenScale());

	// **** 2D用ラインの作成 **********************************************
	// 線の設定
	m_lines.resize(2);
	for (int i = 0; i < m_lines.size(); i++)
	{
		m_lines[i] = std::make_unique<Line2D>();
	}

	m_lines[0]->Initialize(GetOwner()->GetCommonResources()->GetDeviceResources());
	m_lines[1]->Initialize(GetOwner()->GetCommonResources()->GetDeviceResources());

	m_lines[0]->SetAngle(XMConvertToRadians(LINE0_ANGLE_DEGREE));
	m_lines[1]->SetAngle(XMConvertToRadians(LINE1_ANGLE_DEGREE));

	m_lines[0]->SetPosition(Vector2(screen->GetCenterXF() - LINE0_POS_X_OFFSET * screen->GetScreenScale(), screen->GetCenterYF()));
	m_lines[1]->SetPosition(Vector2(screen->GetCenterXF() + LINE1_POS_X_OFFSET * screen->GetScreenScale(), screen->GetCenterYF()));

	m_lines[0]->SetLength(LINE_LENGTH * screen->GetScreenScale());
	m_lines[1]->SetLength(LINE_LENGTH * screen->GetScreenScale());

	m_lines[0]->SetThickness(LINE_THICKNESS * screen->GetScreenScale());
	m_lines[1]->SetThickness(LINE_THICKNESS * screen->GetScreenScale());

	// ポーズメニューの作成 **********************************************
	m_poseMenu = std::make_unique<PoseMenu>();
	m_poseMenu->Initialize(m_canvas.get(), GetOwner()->GetCommonResources(), [&](PoseMenu::MenuItem pushItem) {
		OnPushMenuItem(pushItem); });

	// チュートリアルウィンドウの作成 **********************************************
	m_tutorialWindow = std::make_unique<TutorialWindow>();
	m_tutorialWindow->Initialize(resourceManager, [this]() {OnCloseTutorialWindow(); });

	// 入力の作成 **********************************************
	m_systemInput = InputBindingFactory::CreateSystemInput();

	m_isPrevConnectedGamepad = false;


	// **** タスク管理へ登録 ****
	GetOwner()->GetTaskManager()->AddTask(m_canvas.get());

	// ステージ管理の更新を停止する
	GetOwner()->GetStageManager()->StopUpdating();
}

/**
 * @brief 更新処理
 * * @param[in] deltaTime
 */
void PoseGameplayState::OnUpdate(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	// 入力の更新処理
	m_systemInput->Update(
		GetOwner()->GetCommonResources()->GetKeyboardTracker(),
		GetOwner()->GetCommonResources()->GetMouseTracker(),
		GetOwner()->GetCommonResources()->GetGamePadTracker());


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

	// ガイドUI変更を試みる
	TryChangeCurrentGuideUI();


	// タスク管理の更新処理
	GetOwner()->GetTaskManager()->Update(deltaTime);
}


/**
 * @brief 描画処理
 * */
void PoseGameplayState::OnDraw()
{
	auto states = GetOwner()->GetCommonResources()->GetCommonStates();
	auto screen = Screen::Get();

	// 現在のカメラの取得
	const Camera* pCurrentCamera = MainCamera::GetInstance()->GetCamera();
	// エフェクトにカメラを設定する
	GameEffectController::GetInstance()->SetCamera(pCurrentCamera);
	// タスク管理の描画処理
	GetOwner()->GetTaskManager()->Render(*pCurrentCamera);


	if (m_isDisplayingTutorialWindow == false)
	{
		m_poseMenu->Draw();

		for (auto& line : m_lines)
		{
			line->Draw(states, screen->GetWidthF(), screen->GetHeightF());
		}
	}


}

void PoseGameplayState::OnExitState()
{
	// キャンバスの削除
	GetOwner()->GetTaskManager()->DeleteTask(m_canvas.get());
}

/**
 * @brief メニューアイテムを押したときに呼ばれる
 * * @param[in] menuItem　押されたメニューアイテム
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
 * */
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


/**
 * @brief 現在のガイドガイドUIの変更を試みる
 *
 * 簡易実装用の後にクラス化する
 *
 * @returns true 成功
 * @returns false 失敗
 */
bool PoseGameplayState::TryChangeCurrentGuideUI()
{

	bool requestChange = false;
	bool changePC = true;;

	// ゲームパッドの接続されたらパッドガイドにに切り替える
	if (!m_isPrevConnectedGamepad && GetOwner()->GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = false;
		requestChange = true;
		m_isPrevConnectedGamepad = true;
	}

	// ゲームパッドが接続されなくなったらPCガイドに切り替える
	if (m_isPrevConnectedGamepad && !GetOwner()->GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = true;
		requestChange = true;
		m_isPrevConnectedGamepad = false;

	}

	if (requestChange)
	{
		const char* manualFilePath;
		const char* operatingFilePath; // std::string から const char* に型を変更

		if (changePC)
		{
			manualFilePath = TEXTURE_PATH_MANUAL_PC;
			operatingFilePath = TEXTURE_PATH_OPERATING_PC;
		}

		else
		{
			manualFilePath = TEXTURE_PATH_MANUAL_GAMEPAD;
			operatingFilePath = TEXTURE_PATH_OPERATING_GAMEPAD;

		}

		// ガイドUI
		// 拡大率と座標を保持する
		float manualScale = m_manualSprite->GetScale();
		SimpleMath::Vector2 manualPosition = m_manualSprite->GetPosition();
		m_manualSprite->Initialize(GetOwner()->GetCommonResources()->GetResourceManager()->CreateTexture(manualFilePath));
		m_manualSprite->SetScale(manualScale);
		m_manualSprite->SetPosition(manualPosition);


		// 操作説明
		float operatingScale = 0.0f;
		auto screen = Screen::Get();

		// PCとゲームパッドだとサイズが異なるためここで修正
		if (changePC)
		{
			operatingScale = OPERATING_SPRITE_SCALE_PC * screen->GetScreenScale();
		}
		else
		{
			operatingScale = OPERATING_SPRITE_SCALE_GAMEPAD * screen->GetScreenScale();
		}
		SimpleMath::Vector2 operatingPosition = m_operatingSprite->GetPosition();
		m_operatingSprite->Initialize(GetOwner()->GetCommonResources()->GetResourceManager()->CreateTexture(operatingFilePath));
		m_operatingSprite->SetScale(operatingScale);
		m_operatingSprite->SetPosition(operatingPosition);

		return true;
	}

	return false;
}
