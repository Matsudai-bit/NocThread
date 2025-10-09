/**
 * @file   LoadingScreen.cpp
 *
 * @brief	ロード画面に関するソースファイル
 *
 * @author 松下大暉
 *
 * @date   2025/08/03
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "LoadingScreen.h"

#include "Game/Common/CommonResources/CommonResources.h"
#include "Library/ImaseLib/DebugFont.h"
#include "Game/Common/DeviceResources.h"

#include "Game/Scene/GameplayScene/GameplayScene.h"

#include "Game/Common/ResourceManager/ResourceManager.h"

#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

#include "Game/Common/Screen.h"
using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 */
LoadingScreen::LoadingScreen()

{

}



/**
 * @brief デストラクタ
 */
LoadingScreen::~LoadingScreen()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void LoadingScreen::Initialize()
{
	auto screen = Screen::Get();

	// **** スプライトの作成 ****
	m_backgroundSprite	= std::make_unique<Sprite>();
	m_loadingSprite		= std::make_unique<Sprite>();
	m_loadingFontSprite = std::make_unique<Sprite>();

	// スプライトの初期化処理

	m_backgroundSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("back_loading.dds"));

	Sprite::Region region;
	region.topLeftX = 0;
	region.topLeftY = 0;
	region.width = 64;
	region.height = 64;
	m_loadingSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("spyware.dds"), region);

	Sprite::Region fontRegion;
	fontRegion.topLeftX = 0;
	fontRegion.topLeftY = 0;
	fontRegion.width = 320;
	fontRegion.height = 128;
	m_loadingFontSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("loading_font.dds"), fontRegion);



	// **** アニメータの作成 **** 
	m_loadingAnimator		= std::make_unique<SpriteAnimator>();
	m_loadingFontAnimator	= std::make_unique<SpriteAnimator>();

	//	アニメータの初期化処理
	SpriteAnimator::AnimationClip animationClip;
	animationClip.startRegion	= region;
	animationClip.numFrames		= 6;
	animationClip.numColumns	= 6;
	animationClip.duration		= 1.0f;
	animationClip.isLoop		= true;
	m_loadingAnimator->Initialize(m_loadingSprite.get(), animationClip);

	SpriteAnimator::AnimationClip fontAnimationClip;
	fontAnimationClip.startRegion = fontRegion;
	fontAnimationClip.numFrames = 14;
	fontAnimationClip.numColumns = 14;
	fontAnimationClip.duration = 2.0f;
	fontAnimationClip.isLoop = true;
	m_loadingFontAnimator->Initialize(m_loadingFontSprite.get(), fontAnimationClip);

	// **** キャンバスの作成 ****
	m_canvas = std::make_unique<Canvas>();
	m_canvas->Initialize(GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext());

	// キャンバスにスプライトの登録
	m_canvas->AddSprite(m_backgroundSprite.get());
	m_canvas->AddSprite(m_loadingSprite.get());
	m_canvas->AddSprite(m_loadingFontSprite.get());


		
	// スプライトの座標の設定
	m_backgroundSprite->SetPosition(SimpleMath::Vector2(screen->GetCenterXF(), screen->GetCenterYF()));
	m_backgroundSprite->SetScale(screen->GetScreenScale());

	m_loadingSprite->SetPosition(SimpleMath::Vector2(screen->GetRightF() - 90.0f * screen->GetScreenScale(), screen->GetBottomF() - 100.0f * screen->GetScreenScale()));
	m_loadingSprite->SetScale(2.0f * screen->GetScreenScale());

	m_loadingFontSprite->SetPosition(SimpleMath::Vector2(screen->GetRightF() - 300.0f * screen->GetScreenScale(), screen->GetBottomF() - 100.0f * screen->GetScreenScale()));
	//m_loadingFontSprite->SetScale(2.0f);

	// アニメータの再生
	m_loadingAnimator->Play();
	m_loadingFontAnimator->Play();
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime フレーム間の経過時間
 *
 * @return なし
 */
void LoadingScreen::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// アニメータの更新処理
	m_loadingAnimator->Update(elapsedTime);
	m_loadingFontAnimator->Update(elapsedTime);

	// キーボードの情報取得する
	auto kb = Keyboard::Get().GetState();


}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void LoadingScreen::Render()
{
	// 共通リソース
	auto states = GetCommonResources()->GetCommonStates();

	//GetCommonResources()->GetDebugFont()->AddString(0, 30, Colors::White, L"LoadingScreen");

	// キャンバスの描画処理
	m_canvas->Draw(states);

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void LoadingScreen::Finalize()
{

}
