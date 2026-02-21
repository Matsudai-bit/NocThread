/**
 * @file   TitleScene.cpp
 *
 * @brief  タイトルシーンに関するソースファイル
 *
 * @author 松下大暉
 *
 * @date   2025/04/06
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TitleScene.h"

#include <string>
#include <typeinfo>

// ライブラリ関連
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Library/DirectXFramework/DeviceResources.h"
#include "Library/MyLib/EasingKit/EasingKit.h"
#include "Library/DebugHelper.h"

// シーン関連
#include "Game/Scene/Loading/LoadingScreen.h"
#include "Game/Scene/GameplayScene/GameplayScene.h"

// データベース関連
#include "Game/Common/Database/SoundDatabase.h"
#include "Game/Common/Database/TextureDatabase.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/SoundManager/SoundManager.h"
#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"
#include "Game/Common/Framework/Input/InputActionMap/InputActionMap.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"

// グラフィック関連
#include "Game/Common/Graphics/TransitionMask/TransitionMask.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/InputDeviceSpriteResolver/InputDeviceSpriteResolver.h"

// UIメニュー関連
#include "Game/UI/TutorialWindow/TutorialWindow.h"


using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 */
TitleScene::TitleScene()
	: m_bgmSoundID{}
	, m_isPrevConnectedGamepad{ false }
	, m_isQuit{false}
{

}



/**
 * @brief デストラクタ
 */
TitleScene::~TitleScene()
{
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Initialize()
{
	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto states = GetCommonResources()->GetCommonStates();

	// ***** 後に状態で分ける ****
	// タイトルメニューの作成
	m_titleMenu = std::make_unique<TitleMenu>();
	// チュートリアルウィンドウの作成
	m_tutorialWindow = std::make_unique<TutorialWindow>();

	// スプライトの作成
	CreateSprites();

	// デバイス毎の切り替えに登録
	GetCommonResources()->GetInputDeviceSpriteResolver()->AddKeyboardSprite(m_keyboardManualSprite.get());
	GetCommonResources()->GetInputDeviceSpriteResolver()->AddGamePadSprite(m_gamePadManualSprite.get());
	
	// キャンバスの作成
	m_canvas = std::make_unique<Canvas>(context, states);

	// キャンバスにスプライトの登録
	m_canvas->AddSprite(m_backgroundSprite.get());
	m_canvas->AddSprite(m_alphaSprite.get());
	m_canvas->AddSprite(m_logoSprite.get());
	m_canvas->AddSprite(m_keyboardManualSprite.get());
	m_canvas->AddSprite(m_gamePadManualSprite.get());

	m_titleMenu			->Initialize(m_canvas.get(), GetCommonResources(), [this](TitleMenu::MenuItem menuItem) { OnPushMenuItem(menuItem); });
	m_tutorialWindow	->Initialize(GetCommonResources()->GetResourceManager(), [this]() {OnCloseTutorialWindow(); });


	m_ElapsedTimeCounter.Reset();

	m_logoOpacityTween.Initialize(0.0f, 1.0f, LOGO_EASING_TIME);
	m_logoOpacityTween.SetDelay(1.0f);

	SoundManager::GetInstance()->RemoveAll();
	m_bgmSoundID = SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::BGM_TITLE), true);

	m_tutorialWindow->SetVisible(false);

	m_isPrevConnectedGamepad = false;

	GetCommonResources()->GetTransitionMask()->Open();

	// 入力に対するコールバックを登録
	RegisterBindCallbackToInput();
	// メニューアイテムイベントの作成
	CreateMenuItemEvent();

}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @return なし
 */
void TitleScene::Update(float deltaTime)
{
	// イベントの消化
	m_onPushMenuItemEvent->ApplyEvents();

	// ロゴの演出更新
	UpdateLogoOpacity(deltaTime);
	
	// メインコンテンツの更新
	UpdateTitleMenu(deltaTime);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Render()
{
	// 終了していなければ強制退避
	if (m_isQuit && GetCommonResources()->GetTransitionMask()->IsEnd())
	{
		return;
	}

	// キャンバスの描画
	m_canvas->DrawContents();
	// メインコンテンツの描画
	m_titleMenu->Draw();

	

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Finalize()
{
	SoundManager::GetInstance()->RemoveAll();

	// 入力とコールバックの紐づけを解除する
	UnBindCallbackToInput();

	// デバイス毎の切り替を削除する
	GetCommonResources()->GetInputDeviceSpriteResolver()->RemoveGamePadSprite(m_gamePadManualSprite.get());
	GetCommonResources()->GetInputDeviceSpriteResolver()->RemoveGamePadSprite(m_keyboardManualSprite.get());
}

// 中心の算出
DirectX::SimpleMath::Vector2 TitleScene::CalcCenterOrigin(ID3D11ShaderResourceView* pTexture)
{

	// テクスチャの幅と高さを取得する
	ID3D11Resource* resource = nullptr;
	pTexture->GetResource(&resource);

	ID3D11Texture2D* texture = nullptr;
	resource->QueryInterface(&texture);

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	float textureWidth = static_cast<float>(desc.Width);
	float textureHeight = static_cast<float>(desc.Height);

	// リソースの解放
	resource->Release();
	texture->Release();



	// テクスチャの中心を計算する
	return SimpleMath::Vector2(textureWidth / 2.0f, textureHeight / 2.0f);

}

/**
 * @brief メニューアイテムを押したときに呼ばれる
 * * @param[in] menuItem　押されたアイテム
 */
void TitleScene::OnPushMenuItem(TitleMenu::MenuItem menuItem)
{ 
	m_onPushMenuItemEvent->FireEvent(menuItem);
}

/**
 * @brief チュートリアルウィンドウを閉じる際の処理
 */
void TitleScene::OnCloseTutorialWindow()
{
	m_tutorialWindow->SetVisible(false);
	m_titleMenu->SetActive(true);
	m_canvas->RemoveSprite(m_tutorialWindow.get());
}


/**
 * @brief ロゴの透過イージングの更新処理
 *
 * @param[in] deltaTime 経過時間
 */
void TitleScene::UpdateLogoOpacity(float deltaTime)
{
	m_logoOpacityTween.Update(deltaTime);
	
	m_logoSprite->SetOpacity(m_logoOpacityTween.GetValue());
}

/**
 * @brief タイトルメニューの更新処理
 *
 * @param[in] deltaTime 経過時間
 */
void TitleScene::UpdateTitleMenu(float deltaTime)
{
	// 画面遷移（フェードなど）が終わっているか確認
	if (!GetCommonResources()->GetTransitionMask()->IsEnd())
	{
		return;
	}

	m_titleMenu->Update(deltaTime);
}
/**
 * @brief 入力に対するコールバックを登録
 */
void TitleScene::RegisterBindCallbackToInput()
{
	// 入力システムの取得
	auto pInputSystem	= GetCommonResources()->GetInputSystem();
	auto uiActionMap	= pInputSystem->GetInputActionMap(InputActionID::UI::MAP_NAME);

	// 上入力
	uiActionMap->BindInputEvent(
		InputActionID::UI::UP_MOVE,		this,
		[this](const InputEventData& data) { OnInputUp(data); });
	// 下入力
	uiActionMap->BindInputEvent(
		InputActionID::UI::DOWN_MOVE,	this,
		[this](const InputEventData& data) { OnInputDown(data); });
	// 左入力
	uiActionMap->BindInputEvent(
		InputActionID::UI::LEFT_MOVE,	this,
		[this](const InputEventData& data) { OnInputLeft(data); });
	// 右入力
	uiActionMap->BindInputEvent(
		InputActionID::UI::RIGHT_MOVE,	this,
		[this](const InputEventData& data) { OnInputRight(data); });
	// 決定入力
	uiActionMap->BindInputEvent(
		InputActionID::UI::CONFIRM,		this,
		[this](const InputEventData& data) { OnInputConfirm(data); });
	// 戻る入力
	uiActionMap->BindInputEvent(
		InputActionID::UI::CANCEL,		this,
		[this](const InputEventData& data) { OnInputExit(data); });
}

/**
 * @brief 紐づけの解除をする
 */
void TitleScene::UnBindCallbackToInput()
{
	// 入力システムの取得
	auto pInputSystem = GetCommonResources()->GetInputSystem();
	auto uiActionMap = pInputSystem->GetInputActionMap(InputActionID::UI::MAP_NAME);

	uiActionMap->UnBindAllInputEvent(InputActionID::UI::UP_MOVE,	this);
	uiActionMap->UnBindAllInputEvent(InputActionID::UI::LEFT_MOVE,	this);
	uiActionMap->UnBindAllInputEvent(InputActionID::UI::RIGHT_MOVE, this);
	uiActionMap->UnBindAllInputEvent(InputActionID::UI::DOWN_MOVE,	this);
	uiActionMap->UnBindAllInputEvent(InputActionID::UI::CONFIRM,	this);
	uiActionMap->UnBindAllInputEvent(InputActionID::UI::CANCEL,		this);

}

/**
 * @brief 左入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void TitleScene::OnInputLeft(const InputEventData& data)
{
	if (m_tutorialWindow && m_tutorialWindow->IsVisible() && GetCommonResources()->GetTransitionMask()->IsEnd())
		m_tutorialWindow->OnMoveDownLeft(data);
}

/**
 * @brief 右入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void TitleScene::OnInputRight(const InputEventData& data)
{
	if (m_tutorialWindow && m_tutorialWindow->IsVisible() && GetCommonResources()->GetTransitionMask()->IsEnd())
		m_tutorialWindow->OnMoveUpRight(data);
}

/**
 * @brief 上入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void TitleScene::OnInputUp(const InputEventData& data)
{
	if (m_titleMenu && m_titleMenu->IsActive() && GetCommonResources()->GetTransitionMask()->IsEnd())
		m_titleMenu->OnMoveUpSelector(data);
}

/**
 * @brief 下入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void TitleScene::OnInputDown(const InputEventData& data)
{
	if (m_titleMenu && m_titleMenu->IsActive() && GetCommonResources()->GetTransitionMask()->IsEnd())
		m_titleMenu->OnMoveDownSelector(data);
}

/**
 * @brief 決定入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void TitleScene::OnInputConfirm(const InputEventData& data)
{
	if (GetCommonResources()->GetTransitionMask()->IsEnd())
	{
		if (m_titleMenu && m_titleMenu->IsActive())
			m_titleMenu->OnSelect(data);

		if (m_tutorialWindow && m_tutorialWindow->IsVisible())
			m_tutorialWindow->OnSelect(data);
	}

}
/**
 * @brief 出る入力された時に呼ばれる
 *
 * @param[in] data	入力イベントデータ
 */
void TitleScene::OnInputExit(const InputEventData& data)
{
	

	if (m_tutorialWindow->IsVisible() && GetCommonResources()->GetTransitionMask()->IsEnd())
		m_tutorialWindow->OnSelect(data);
}

/**
 * @brief メニューアイテムを押された時のイベントの作成
 *
 */
void TitleScene::CreateMenuItemEvent()
{
	m_onPushMenuItemEvent = std::make_unique<EventSystem<TitleMenu::MenuItem>>();

	// プレイボタンを押したとき
	m_onPushMenuItemEvent->AddListener(TitleMenu::MenuItem::PLAY, [&]()
		{
			// SEの再生
			SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_BUTTON_CLICK_GAMESTART), false);
			std::string newStateName = "Change GameplayScene";
			OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());

			GetCommonResources()->GetTransitionMask()->Close([&]() {ChangeScene<GameplayScene, LoadingScreen>(); });

		});

	// チュートリアルボタンをを押したとき
	m_onPushMenuItemEvent->AddListener(TitleMenu::MenuItem::TUTORIAL, [&]()
		{
			// SEの再生
			SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_DECIDE), false);
			// キャンバスに追加
			m_canvas->AddSprite(m_tutorialWindow.get());
			m_tutorialWindow->SetVisible(true);
			m_titleMenu->SetActive(false);
		});
	m_onPushMenuItemEvent->AddListener(TitleMenu::MenuItem::SETTING, [&]() {});
			
	// 終了ボタンを押したとき
	m_onPushMenuItemEvent->AddListener(TitleMenu::MenuItem::QUIT, [&]() 
		{
			// SEの再生
			SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_DECIDE), false);
			m_isQuit = true;
			GetCommonResources()->GetTransitionMask()->Close([&]() {PostQuitMessage(0);; });

		});
	
}

/**
 * @brief スプライトの作成
 */
void TitleScene::CreateSprites()
{

	auto screen = Screen::Get();

	// スプライトの作成
	m_backgroundSprite	= std::make_unique<Sprite>();
	m_alphaSprite		= std::make_unique<Sprite>();
	m_logoSprite		= std::make_unique<Sprite>();
	m_keyboardManualSprite	= std::make_unique<Sprite>();
	m_gamePadManualSprite	= std::make_unique<Sprite>();

	// 処理用変数
	auto pResourceManager = GetCommonResources()->GetResourceManager();

	using namespace TextureDatabase;
	{

		m_backgroundSprite	  ->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_TITLE)));
		m_alphaSprite		  ->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_TITLE_ALPHA_MASK)));
		m_logoSprite		  ->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_TITLE_LOGO)));
		m_keyboardManualSprite->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_UI_KEYBOARD)));
		m_gamePadManualSprite ->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_UI_GAMEPAD)));
	}

	// スプライトの座標の設定
	// 背景スプライト: 画面中央
	m_backgroundSprite->SetPosition(SimpleMath::Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	// アルファマスクスプライト
	m_alphaSprite->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + m_alphaSprite->GetSpriteWidth() / 2.0f * screen->GetScreenScale(), screen->GetCenterYF()));
	// ロゴスプライト
	m_logoSprite->SetPosition(SimpleMath::Vector2(screen->GetRightF() - LOGO_POS_OFFSET_X * screen->GetScreenScale(), screen->GetTopF() + LOGO_POS_OFFSET_Y * screen->GetScreenScale()));
	// マニュアルスプライト
	m_keyboardManualSprite->SetPosition(SimpleMath::Vector2(screen->GetRightF() - MANUAL_POS_OFFSET_X * screen->GetScreenScale(), screen->GetBottomF() - MANUAL_POS_OFFSET_Y * screen->GetScreenScale()));
	m_gamePadManualSprite->SetPosition(SimpleMath::Vector2(screen->GetRightF() - MANUAL_POS_OFFSET_X * screen->GetScreenScale(), screen->GetBottomF() - MANUAL_POS_OFFSET_Y * screen->GetScreenScale()));


	// スプライトの拡大率
	m_backgroundSprite->SetScale(BACKGROUND_SCALE * screen->GetScreenScale());
	m_alphaSprite->SetScale(ALPHA_SCALE * screen->GetScreenScale());
	m_logoSprite->SetScale(LOGO_SCALE * screen->GetScreenScale());
	m_keyboardManualSprite->SetScale(MANUAL_SCALE * screen->GetScreenScale());
	m_gamePadManualSprite->SetScale(MANUAL_SCALE * screen->GetScreenScale());
}
