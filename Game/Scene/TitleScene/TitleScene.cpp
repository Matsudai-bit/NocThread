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
#include "Library/ImaseLib/DebugFont.h"
#include "Game/Common/DeviceResources.h"
#include "Library/MyLib/EasingKit/EasingKit.h"

#include "Game/Scene/GameplayScene/GameplayScene.h"

#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

#include "Game/Common/SoundManager/SoundManager.h"
#include "Game/Common/SoundManager/SoundPaths.h"

#include "Game/UI/TutorialWindow/TutorialWindow.h"


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
	auto screen = Screen::Get();
	// スプライトの作成
	m_backgroundSprite = std::make_unique<Sprite>();
	m_alphaSprite = std::make_unique<Sprite>();
	m_logoSprite = std::make_unique<Sprite>();

	// ***** 後に状態で分ける ****
	// タイトルメニューの作成
	m_titleMenu = std::make_unique<TitleMenu>();
	// チュートリアルウィンドウの作成
	m_tutorialWindow = std::make_unique<TutorialWindow>();
	
	m_backgroundSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("Title/title_back.dds"));
	m_alphaSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("Title/title_alpha.dds"));
	m_logoSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("Title/title_logo.dds"));


	// キャンバスの作成
	m_canvas = std::make_unique<Canvas>();
	m_canvas->Initialize(GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext());

	// キャンバスにスプライトの登録
	m_canvas->AddSprite(m_backgroundSprite.get());
	m_canvas->AddSprite(m_alphaSprite.get());
	m_canvas->AddSprite(m_logoSprite.get());

	m_titleMenu->Initialize(m_canvas.get(), GetCommonResources(), [this](TitleMenu::MenuItem menuItem) { OnPushMenuItem(menuItem); });
	m_tutorialWindow->Initialize(GetCommonResources()->GetResourceManager(), [this]() {OnCloseTutorialWindow(); });

	// スプライトの座標の設定
	m_backgroundSprite->SetPosition(SimpleMath::Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	m_alphaSprite->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + m_alphaSprite->GetSpriteWidth() / 2.0f * screen->GetScreenScale(), screen->GetCenterYF()));
	m_logoSprite->SetPosition(SimpleMath::Vector2(screen->GetRightF() - 300.0f  * screen->GetScreenScale(), screen->GetTopF() + 150.0f * screen->GetScreenScale()));
	

	// スプライトの拡大率
	m_backgroundSprite->SetScale(0.68f * screen->GetScreenScale());
	m_alphaSprite->SetScale(1.68f * screen->GetScreenScale());
	m_logoSprite->SetScale(0.68f * screen->GetScreenScale());

	m_elapsedTimeCounter.Reset();

	SoundManager::GetInstance()->RemoveAll();
	m_bgmSoundID = SoundManager::GetInstance()->Play(SoundPaths::BGM_TITLE, true);
	//m_bgmSound->Play(true);

	//m_bgmSound->SetVolume(0.5f);
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime フレーム間の経過時間
 *
 * @return なし
 */
void TitleScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// ロゴの透過イージング
	float opacity = MyLib::EaseInOutQuint( m_elapsedTimeCounter.GetElapsedTime() / LOGO_EASING_TIME);
	if (m_elapsedTimeCounter.GetElapsedTime() <= LOGO_EASING_TIME)
	{
		m_elapsedTimeCounter.UpperTime(elapsedTime);
	}
	else
	{
		opacity = 1.0f;
	}

	m_logoSprite->SetOpacity(opacity);

	// 座標の算出
	SimpleMath::Vector2 fontSpritePos = SimpleMath::Vector2::Zero;


	if (m_isDisplayingTutorialWindow == false)
	{
		// タイトルメニューの更新処理
		m_titleMenu->Update(elapsedTime);
	}
	else
	{
		m_tutorialWindow->Update(elapsedTime, *GetCommonResources()->GetKeyboardTracker());
	}


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
	// 共通リソース
	auto states = GetCommonResources()->GetCommonStates();

	//GetCommonResources()->GetDebugFont()->AddString(0, 30, Colors::White, L"TitleScene");

	m_canvas->Draw(states);

	if (m_isDisplayingTutorialWindow == false)
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
 * 
 * @param[in] menuItem　押されたアイテム
 */
void TitleScene::OnPushMenuItem(TitleMenu::MenuItem menuItem)
{
	switch (menuItem)
	{
	case TitleMenu::MenuItem::PLAY:
	{
		std::string newStateName = "Change GameplayScene";
		OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());

		ChangeScene<GameplayScene, LoadingScreen>();
	}
		break;
	case TitleMenu::MenuItem::TUTORIAL:
		// キャンバスに追加
		m_canvas->AddSprite(m_tutorialWindow.get());
		m_isDisplayingTutorialWindow = true;
		break;
	case TitleMenu::MenuItem::SETTING:
		break;
	case TitleMenu::MenuItem::QUIT:
		PostQuitMessage(0);
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
