/*****************************************************************//**
 * @file    PauseNavigator.h
 * @brief   ポーズ画面へのナビゲーターに関するソースファイル
 *
 * @author  松下大暉
 * @date    2026/01/31
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PauseNavigator.h"

// フレームワーク関連
#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"
#include "Game/Common/Framework/Input/InputActionMap/InputActionMap.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"


// UIツール関連
#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
// UI関連
#include "Game/UI/Gameplay/PauseGuideUI/PauseGuideUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
PauseNavigator::PauseNavigator()
	: m_pCanvas			{ nullptr }
	, m_pInputSystem	{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
PauseNavigator::~PauseNavigator()
{
	m_pCanvas->RemoveSprite(m_pauseGuideUI.get());

	// 入力システムの取得
	m_pInputSystem->GetInputActionMap(InputActionID::System::MAP_NAME)->UnBindAllInputEvent(InputActionID::System::PAUSE, this);
}




/**
 * @brief 初期化処理
 * 
 * @param[in] pResourceManager			 リソース管理
 * @param[in] pInputDeviceSpriteResolver 入力デバイス毎の切り替え器
 * @param[in] pCanvas					 キャンバス
 * @param[in] pInputSystem				 入力システム
 */
void PauseNavigator::Initialize(
	ResourceManager* pResourceManager, 
	InputDeviceSpriteResolver* pInputDeviceSpriteResolver,
	Canvas* pCanvas,
	InputSystem* pInputSystem)
{
	// ポーズガイドUIの作成
	m_pauseGuideUI = std::make_unique<PauseGuideUI>(pResourceManager, pInputDeviceSpriteResolver);

	// キャンバスの設定
	m_pCanvas = pCanvas;

	// キャンバスに追加
	m_pCanvas->AddSprite(m_pauseGuideUI.get());

	// 入力システムの設定
	m_pInputSystem = pInputSystem;
}

/**
 * @brief ポーズ画面への入力コールバックの設定
 * 
 * @param[in] callback コールバック
 */
void PauseNavigator::SetInputCallback(const std::function<void(const InputEventData&)>& callback)
{
	// システムアクションマップの取得
	auto systemActionMap = m_pInputSystem->GetInputActionMap(InputActionID::System::MAP_NAME);
	// ポーズ入力の登録
	systemActionMap->BindInputEvent(InputActionID::System::PAUSE, this,	callback);
}
