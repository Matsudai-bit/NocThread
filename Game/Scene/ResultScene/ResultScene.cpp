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

// シーン関連
#include "Game/Scene/TitleScene/TitleScene.h"
#include "Game/Scene/GameplayScene/GameplayScene.h"


// ライブラリ関連
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"
#include "Library/DirectXFramework/DeviceResources.h"
#include "Game/Scene/Loading/LoadingScreen.h"

// ファクトリー関連
#include "Game/Common/Factory/InputBindingFactory/InputBindingFactory.h"

// データベース関連
#include "Game/Common/Database/SoundDatabase.h"
#include "Game/Common/Database/TextureDatabase.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/SoundManager/SoundManager.h"
#include "Game/Common/Framework/Input/InputActionMap/InputActionMap.h"
#include "Game/Common/Framework/Input/InputManager/InputManager.h"

// グラフィック関連
#include "Game/Common/Graphics/TransitionMask/TransitionMask.h"

// UIツール関連
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/ResultData/ResultData.h"

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

	auto context = GetCommonResources()->GetDeviceResources()->GetD3DDeviceContext();
	// 共通リソース
	auto states = GetCommonResources()->GetCommonStates();
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

	auto pResourceManager = GetCommonResources()->GetResourceManager();
	m_backgroundSprite				->Initialize(pResourceManager->CreateTexture(TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::BACKGROUND_RESULT)));
	m_backgroundAlphaFilterSprite	->Initialize(pResourceManager->CreateTexture(TextureDatabase::TEXTURE_PATH_MAP.at(TextureDatabase::TextureID::BACKGROUND_RESULT_ALPHA_MASK)));

	// キャンバスの作成
	m_canvas = std::make_unique<Canvas>(context, states);

	// キャンバスにスプライトの登録
	m_canvas->AddSprite(m_backgroundSprite.get());
	m_canvas->AddSprite(m_backgroundAlphaFilterSprite.get());

	// スプライトの座標の設定
	m_backgroundSprite				->SetPosition(SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF()));
	m_backgroundAlphaFilterSprite	->SetPosition(SimpleMath::Vector2(Screen::Get()->GetCenterXF(), Screen::Get()->GetCenterYF()));

	// 拡大率の設定
	m_backgroundSprite->SetScale(1.0f * Screen::Get()->GetScreenScale() );
	m_backgroundAlphaFilterSprite->SetScale(1.60f * Screen::Get()->GetScreenScale());

	SoundManager::GetInstance()->Play(SoundDatabase::SOUND_CLIP_MAP.at(SoundDatabase::BGM_RESULT), true);

	// 入力とコールバックの紐づけ
	RegisterBindCallbackToInput();

	GetCommonResources()->GetTransitionMask()->Open();

}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @return なし
 */
void ResultScene::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);


	// ステートマシーンの更新処理
	m_stateMachine->Update(deltaTime);
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
	

	//GetCommonResources()->GetDebugFont()->AddString(0, 30, Colors::White, L"ResultScene");

	m_canvas->DrawContents();

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

	// コールバックの紐づけを解除する
	UnBindCallbackToInput();
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

/**
 * @brief 入力とコールバックの紐づけ
 */
void ResultScene::RegisterBindCallbackToInput()
{	
	// 入力管理の取得
	auto pInputManager = GetCommonResources()->GetInputManager();
	// 次に行くの入力
	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->BindInputEvent(InputActionID::UI::CONFIRM, this,
		[this](const InputEventData& data) { OnInputExit(data); });
}

/**
 * @brief 入力とコールバックの紐づけを解除する
 * 
 */
void ResultScene::UnBindCallbackToInput()
{
	// 入力管理の取得
	auto pInputManager = GetCommonResources()->GetInputManager();

	pInputManager->GetInputActionMap(InputActionID::UI::MAP_NAME)->UnBindAllInputEvent(InputActionID::UI::CONFIRM, this);
}

/**
 * @brief 出る
 * 
 * @param[in] data　入力イベントデータ
 */
void ResultScene::OnInputExit(const InputEventData& data)
{
	if (GetCommonResources()->GetTransitionMask()->IsEnd() && data.inputOption.pressed)
	{
		GetCommonResources()->GetTransitionMask()->Close([&]() {ChangeScene<TitleScene, LoadingScreen>();; });

		
	}
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
