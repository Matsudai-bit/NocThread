/**
 * @file   ResultScene.cpp
 *
 * @brief  タイトルシーンに関するソースファイル
 *
 * @author 松下大暉
 *
 * @date   2025/09/04
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ResultScene.h"
#include "Game/Scene/TitleScene/TitleScene.h"

// ライブラリ関連
#include "Game/Scene/Loading/LoadingScreen.h"

#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/ResourceManager/ResourceManager.h"
#include "Library/ImaseLib/DebugFont.h"
#include "Game/Common/DeviceResources.h"

#include "Game/Scene/GameplayScene/GameplayScene.h"

#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

#include "Game/Common/SoundManager/SoundManager.h"
#include "Game/Common/SoundManager/SoundPaths.h"

#include "Game/Common/ResultData/ResultData.h"

// 状態
#include "Game/Scene/ResultScene/State/SuccessResultState/SuccessResultState.h"
#include "Game/Scene/ResultScene/State/FailureResultState/FailureResultState.h"


using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 */
ResultScene::ResultScene()
{

}



/**
 * @brief デストラクタ
 */
ResultScene::~ResultScene()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ResultScene::Initialize()
{
	// ステートマシーンの作成
	m_stateMachine = std::make_unique <StateMachine<ResultScene>>(this);

	// 成功かどうか
	if (ResultData::GetInstance()->IsSuccess())
	{
		m_stateMachine->ChangeState<SuccessResultState>();
	}
	else
	{
		m_stateMachine->ChangeState<FailureResultState>();
	}
	

	// スプライトの作成
	m_backgroundSprite				= std::make_unique<Sprite>();
	m_backgroundAlphaFilterSprite	= std::make_unique<Sprite>();

	m_backgroundSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("Result/result_back.dds"));
	m_backgroundAlphaFilterSprite->Initialize(GetCommonResources()->GetResourceManager()->CreateTexture("Result/result_alpha.dds"));

	// キャンバスの作成
	m_canvas = std::make_unique<Canvas>();
	m_canvas->Initialize(GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext());

	// キャンバスにスプライトの登録
	m_canvas->AddSprite(m_backgroundSprite.get());
	m_canvas->AddSprite(m_backgroundAlphaFilterSprite.get());

	// スプライトの座標の設定
	m_backgroundSprite				->SetPosition(SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF()));
	m_backgroundAlphaFilterSprite	->SetPosition(SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF()));

	// 拡大率の設定
	m_backgroundSprite->SetScale(1.0f * Screen::Get()->GetScreenScale() );
	m_backgroundAlphaFilterSprite->SetScale(1.60f * Screen::Get()->GetScreenScale());

	SoundManager::GetInstance()->Play(SoundPaths::BGM_RESULT);
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime フレーム間の経過時間
 *
 * @return なし
 */
void ResultScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);


	// キーボードの情報取得する
	auto kbTracker = GetCommonResources()->GetKeyboardTracker();

	// ステートマシーンの更新処理
	m_stateMachine->Update(elapsedTime);

	if (kbTracker->IsKeyPressed(Keyboard::Space))
	{
		ChangeScene<TitleScene, LoadingScreen>();
	}

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ResultScene::Render()
{
	// 共通リソース
	auto states = GetCommonResources()->GetCommonStates();

	GetCommonResources()->GetDebugFont()->AddString(0, 30, Colors::White, L"ResultScene");

	m_canvas->Draw(states);

	// ステートマシーンの描画処理
	m_stateMachine->Draw();
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ResultScene::Finalize()
{
	SoundManager::GetInstance()->RemoveAll();
}

/**
 * @brief キャンバスの取得
 * 
 * @return キャンバス
 */
Canvas* ResultScene::GetCanvas() const
{
	return m_canvas.get();
}

// 中心の算出
DirectX::SimpleMath::Vector2 ResultScene::CalcCenterOrigin(ID3D11ShaderResourceView* pTexture)
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
