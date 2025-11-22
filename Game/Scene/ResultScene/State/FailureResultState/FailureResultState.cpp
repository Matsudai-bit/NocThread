/*****************************************************************//**
 * @file    FailureResultState.h
 * @brief   リザルトシーンの成功状態に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "FailureResultState.h"

#include "Game/Scene/ResultScene/ResultScene.h"

#include "Game/Common/Screen.h"

#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/DeviceResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"

#include "Game/Common/ResultData/ResultData.h"




using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
FailureResultState::FailureResultState()
{

}



/**
 * @brief デストラクタ
 */
FailureResultState::~FailureResultState()
{

}

void FailureResultState::OnStartState()
{
	using namespace SimpleMath;

	auto resourceManager = GetOwner()->GetCommonResources()->GetResourceManager();
	auto screen = Screen::Get();

	// スプライトの作成
	m_failureFontSprite		= std::make_unique<Sprite>();	
	m_resultFontSprite		= std::make_unique<Sprite>();
	m_survivalTime = std::make_unique<Sprite>();
	m_destroyedNumFontSprite = std::make_unique<Sprite>();

	m_failureFontSprite->Initialize(resourceManager->CreateTexture("Result/result_font_failure.dds"));
	m_resultFontSprite->Initialize(resourceManager->CreateTexture("Result/result_font_failure_result.dds"));
	m_survivalTime->Initialize(resourceManager->CreateTexture("Result/result_font_failure_survivalTime.dds"));
	m_destroyedNumFontSprite->Initialize(resourceManager->CreateTexture("Result/result_font_destroyedNum.dds"));

	// クリア時間UIの作成
	Sprite::Region region;
	region.topLeftX = 0;
	region.topLeftY = 0;
	region.width = 29;
	region.height = 47;
	m_clearTime = std::make_unique<TimeUI>();
	m_clearTime->Initialize(
		region,
		resourceManager->CreateTexture("Number/number_thorn.dds"),
		resourceManager->CreateTexture("Number/number_thorn_punc.dds"));

	// 撃破数の数字スプライトの作成
	m_destroyedNumSprite = std::make_unique<SpriteNumber>();
	m_destroyedNumSprite->Initialize(resourceManager->CreateTexture("Number/number_thorn.dds"));
	m_destroyedNumSprite->SetHeight(region.height);
	m_destroyedNumSprite->SetLeftTopY(0);

	// キャンバスへ登録
	auto canvas = GetOwner()->GetCanvas();
	canvas->AddSprite(m_failureFontSprite.get());
	canvas->AddSprite(m_resultFontSprite.get());
	canvas->AddSprite(m_survivalTime.get());
	canvas->AddSprite(m_destroyedNumFontSprite.get());

	canvas->AddSprite(m_clearTime.get());
	canvas->AddSprite(m_destroyedNumSprite.get());

	// 拡大率の設定
	m_failureFontSprite->SetScale(0.3f * screen->GetScreenScale());
	m_resultFontSprite->SetScale(0.3f * screen->GetScreenScale());
	m_survivalTime->SetScale(0.3f * screen->GetScreenScale());
	m_destroyedNumFontSprite->SetScale(0.3f * screen->GetScreenScale());

	m_clearTime->SetScale(1.1f * screen->GetScreenScale());
	m_destroyedNumSprite->SetScale(1.5f * screen->GetScreenScale());

	// 座標の設定
	m_failureFontSprite->SetPosition(Vector2(screen->GetLeftF() + 250.0f * screen->GetScreenScale(), screen->GetTopF() + 100.0f * screen->GetScreenScale()));
	m_resultFontSprite->SetPosition(Vector2(screen->GetRightF() - 270.0f * screen->GetScreenScale(), screen->GetBottomF() - 150.0f * screen->GetScreenScale()));
	m_survivalTime->SetPosition(Vector2(screen->GetLeftF() + 470.0f * screen->GetScreenScale(), screen->GetTopF() + 300.0f * screen->GetScreenScale()));
	m_destroyedNumFontSprite->SetPosition(Vector2(screen->GetLeftF() + 470.0f * screen->GetScreenScale(), screen->GetTopF() + 500.0f * screen->GetScreenScale()));

	m_clearTime->SetPosition(Vector2(screen->GetLeftF() + 550.0f * screen->GetScreenScale(), screen->GetTopF() + 260.0f * screen->GetScreenScale()));
	m_destroyedNumSprite->SetPosition(Vector2(screen->GetLeftF() + 550.0f * screen->GetScreenScale(), screen->GetTopF() + 460.0f * screen->GetScreenScale()));

	m_elapsedCounterForEasing.Reset();


	auto resultData = ResultData::GetInstance();
	// 時間の設定
	m_clearTime->UpdateTime(resultData->GetGamePlayingTime());
	// 撃破数の設定
	m_destroyedNumSprite->SetNumber(resultData->GetDestroyedNum());
}

void FailureResultState::OnUpdate(float deltaTime)
{
	m_elapsedCounterForEasing.UpperTime(deltaTime);

	float ratio= 0.3f +0.7f * MyLib::EaseOutSine(m_elapsedCounterForEasing.GetElapsedTime() / EASING_TIME);
	m_resultFontSprite->SetOpacity(ratio);

	if (m_elapsedCounterForEasing.GetElapsedTime() >= EASING_TIME)
	{
		m_elapsedCounterForEasing.Reset();
	}


}

void FailureResultState::OnDraw()
{
}

