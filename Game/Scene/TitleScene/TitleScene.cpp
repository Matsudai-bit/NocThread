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

// ライブラリ関連
#include "Game/Scene/Loading/LoadingScreen.h"

#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Library/DirectXFramework/DeviceResources.h"
#include "Library/MyLib/EasingKit/EasingKit.h"

#include "Game/Scene/GameplayScene/GameplayScene.h"

#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

#include "Game/Common/SoundManager/SoundManager.h"
#include "Game/Common/Database/SoundDatabase.h"

#include "Game/UI/TutorialWindow/TutorialWindow.h"
#include "Game/Common/TransitionMask/TransitionMask.h"

// データベース関連
#include "Game/Common/Database/TextureDatabase.h"

#include "Library/DebugHelper.h"
#include <string>
#include <typeinfo>
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 */
TitleScene::TitleScene()
	: m_bgmSoundID{}
	, m_isDisplayingTutorialWindow{ false }
	, m_isPrevConnectedGamepad{ false }
	, m_isQuit{false}
{

}



/**
 * @brief デストラクタ
 */
TitleScene::~TitleScene()
{
	OutputDebugStringA("--- TitleScene Destructor Called ---\n");
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

	auto screen = Screen::Get();
	// スプライトの作成
	m_backgroundSprite = std::make_unique<Sprite>();
	m_alphaSprite = std::make_unique<Sprite>();
	m_logoSprite = std::make_unique<Sprite>();
	m_manualSprite = std::make_unique<Sprite>();

	// ***** 後に状態で分ける ****
	// タイトルメニューの作成
	m_titleMenu = std::make_unique<TitleMenu>();
	// チュートリアルウィンドウの作成
	m_tutorialWindow = std::make_unique<TutorialWindow>();

	// 処理用変数
	auto pResourceManager = GetCommonResources()->GetResourceManager();

	using namespace TextureDatabase;
	{
		
		m_backgroundSprite->Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_TITLE)));
		m_alphaSprite->		Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_TITLE_ALPHA_MASK)));
		m_logoSprite->		Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::BACKGROUND_TITLE_LOGO)));
		m_manualSprite->	Initialize(pResourceManager->CreateTexture(TEXTURE_PATH_MAP.at(TextureID::UI_GUIDE_UI_KEYBOARD)));
	}
	
	// キャンバスの作成
	m_canvas = std::make_unique<Canvas>(context, states);

	// キャンバスにスプライトの登録
	m_canvas->AddSprite(m_backgroundSprite.get());
	m_canvas->AddSprite(m_alphaSprite.get());
	m_canvas->AddSprite(m_logoSprite.get());
	m_canvas->AddSprite(m_manualSprite.get());

	m_titleMenu->		Initialize(m_canvas.get(), GetCommonResources(), [this](TitleMenu::MenuItem menuItem) { OnPushMenuItem(menuItem); });
	m_tutorialWindow->	Initialize(GetCommonResources()->GetResourceManager(), [this]() {OnCloseTutorialWindow(); });

	// スプライトの座標の設定
	// 背景スプライト: 画面中央
	m_backgroundSprite->SetPosition(SimpleMath::Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	// アルファマスクスプライト
	m_alphaSprite->		SetPosition(SimpleMath::Vector2(screen->GetLeftF() + m_alphaSprite->GetSpriteWidth() / 2.0f * screen->GetScreenScale(), screen->GetCenterYF()));
	// ロゴスプライト
	m_logoSprite->		SetPosition(SimpleMath::Vector2(screen->GetRightF() - LOGO_POS_OFFSET_X * screen->GetScreenScale(), screen->GetTopF() + LOGO_POS_OFFSET_Y * screen->GetScreenScale()));
	// マニュアルスプライト
	m_manualSprite->	SetPosition(SimpleMath::Vector2(screen->GetRightF() - MANUAL_POS_OFFSET_X * screen->GetScreenScale(), screen->GetBottomF() - MANUAL_POS_OFFSET_Y * screen->GetScreenScale()));


	// スプライトの拡大率
	m_backgroundSprite->SetScale(BACKGROUND_SCALE * screen->GetScreenScale());
	m_alphaSprite->		SetScale(ALPHA_SCALE * screen->GetScreenScale());
	m_logoSprite->		SetScale(LOGO_SCALE * screen->GetScreenScale());
	m_manualSprite->	SetScale(MANUAL_SCALE * screen->GetScreenScale());

	m_ElapsedTimeCounter.Reset();

	SoundManager::GetInstance()->RemoveAll();
	m_bgmSoundID = SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::BGM_TITLE), true);
	//m_bgmSound->Play(true);

	//m_bgmSound->SetVolume(0.5f);

	m_isPrevConnectedGamepad = false;

	GetCommonResources()->GetTransitionMask()->Open();
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
	// ロゴの演出更新
	UpdateLogoOpacity(deltaTime);

	// メインコンテンツ（メニュー or チュートリアル）の更新
	if (m_isDisplayingTutorialWindow)
	{
		UpdateTutorialWindow(deltaTime);
	}
	else
	{
		UpdateTitleMenu(deltaTime);
	}

	// ガイドUIの更新
	TryChangeCurrentGuideUI();
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
	if (m_isDisplayingTutorialWindow == false)
	{
		m_titleMenu->Draw();
	}

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
	switch (menuItem)
	{
	case TitleMenu::MenuItem::PLAY:
	{// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_BUTTON_CLICK_GAMESTART), false);
		std::string newStateName = "Change GameplayScene";
		OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());


		GetCommonResources()->GetTransitionMask()->Close([&]() {ChangeScene<GameplayScene, LoadingScreen>(); });
	}
	break;
	case TitleMenu::MenuItem::TUTORIAL:
	
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_DECIDE), false);
		// キャンバスに追加
		m_canvas->AddSprite(m_tutorialWindow.get());
		m_isDisplayingTutorialWindow = true;
		break;
	case TitleMenu::MenuItem::SETTING:
		
		break;
	case TitleMenu::MenuItem::QUIT:
		// SEの再生
		SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::SE_DECIDE), false);
		m_isQuit = true;
		GetCommonResources()->GetTransitionMask()->Close([&]() {PostQuitMessage(0);; });

		break;
	default:
		break;
	}
}

/**
 * @brief チュートリアルウィンドウを閉じる際の処理
 */
void TitleScene::OnCloseTutorialWindow()
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
bool TitleScene::TryChangeCurrentGuideUI()
{
	bool requestChange = false;
	bool changePC = true;;

	// ゲームパッドの接続されたらパッドガイドにに切り替える
	if (!m_isPrevConnectedGamepad && GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = false;
		requestChange = true;
		m_isPrevConnectedGamepad = true;
	}

	// ゲームパッドが接続されなくなったらPCガイドに切り替える
	if (m_isPrevConnectedGamepad && !GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = true;
		requestChange = true;
		m_isPrevConnectedGamepad = false;

	}

	if (requestChange)
	{
		std::string filePath;

		if (changePC)
		{
			filePath = TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::UI_GUIDE_UI_KEYBOARD);
		}

		else
		{
			filePath = TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::UI_GUIDE_UI_GAMEPAD);
		}

		// 拡大率と座標を保持する
		float scale = m_manualSprite->GetScale();
		SimpleMath::Vector2 position = m_manualSprite->GetPosition();
		m_manualSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture(filePath));
		m_manualSprite->SetScale(scale);
		m_manualSprite->SetPosition(position);

		return true;
	}

	return false;
}

/**
 * @brief ロゴの透過イージングの更新処理
 *
 * @param[in] deltaTime 経過時間
 */
void TitleScene::UpdateLogoOpacity(float deltaTime)
{
	float ratio = m_ElapsedTimeCounter.GetElapsedTime() / LOGO_EASING_TIME;
	float opacity = MyLib::EaseInOutQuint(ratio);

	if (m_ElapsedTimeCounter.GetElapsedTime() <= LOGO_EASING_TIME)
	{
		m_ElapsedTimeCounter.UpperTime(deltaTime);
	}
	else
	{
		opacity = 1.0f;
	}

	m_logoSprite->SetOpacity(opacity);
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
 * @brief チュートリアルウィンドウの更新処理
 *
 * @param[in] deltaTime 経過時間
 */
void TitleScene::UpdateTutorialWindow(float deltaTime)
{
	auto* common = GetCommonResources();

	m_tutorialWindow->Update(
		deltaTime,
		common->GetKeyboardTracker(),
		common->GetMouseTracker(),
		common->GetGamePadTracker()
	);
}
