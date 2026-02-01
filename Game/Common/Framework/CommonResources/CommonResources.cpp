/*****************************************************************//**
 * @file    CommonResources.h
 * @brief   共通リソースに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/03/22
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CommonResources.h"



using namespace DirectX;
// メンバ関数の定義 ===========================================================

/**
 * @brief コンストラクタ
 * 
 * @param[in] pStepTimer		タイマー
 * @param[in] pDeviceResources	デバイスリソース
 * @param[in] pCommonStates		共通ステート
 * @param[in] pDebugFont		デバックフォント
 * @param[in] pResourceManger	リソース管理
 * @param[in] pKeyboardState	キーボードの状態
 * @param[in] pMouseState		マウスの状態
 * @param[in] pGamePadState		ゲームパッドの状態
 * @param[in] pCopyRenderTexture画面をコピーしたテクスチャ
 * @param[in] pTransitionMask	シーン遷移表示用マスク
 * @param[in] pInputDeviceSpriteResolver	入力デバイス毎のスプライトの表記を切り替え器
 * @param[in] pInputSystem		入力システム

 */
CommonResources::CommonResources(
	DX::StepTimer*			pStepTimer,
	DX::DeviceResources*	pDeviceResources,
	CommonStates*			pCommonStates,
	MyLib::DebugFont*		pDebugFont,
	ResourceManager*		pResourceManager,
	Keyboard::KeyboardStateTracker* pKeyboardState,
	Mouse::ButtonStateTracker*		pMouseState,
	GamePad::ButtonStateTracker*	pGamePadState,
	DX::RenderTexture*				pCopyRenderTexture,
	TransitionMask*					pTransitionMask,
	InputDeviceSpriteResolver*		pInputDeviceSpriteResolver,
	InputSystem*					pInputSystem)

	: m_pStepTimer		{pStepTimer}
	, m_pDeviceResources{pDeviceResources}
	, m_pCommonStates	{pCommonStates}
	, m_pDebugFont		{pDebugFont}
	,m_pResourceManager {pResourceManager}
	, m_pKeyboardState	{ pKeyboardState }
	, m_pMouseState		{ pMouseState }
	, m_pGamePadState	{ pGamePadState }
	, m_pCopyRenderTexture{ pCopyRenderTexture }
	, m_copyScreenRequest{ false }
	, m_pTransitionMask{ pTransitionMask }
	, m_pInputDeviceSpriteResolver	{ pInputDeviceSpriteResolver }
	, m_pInputSystem				{ pInputSystem }

{

}



/**
 * @brief デストラクタ
 */
CommonResources::~CommonResources()
{

}

/**
 * @brief タイマーの取得
 * 
 * @return タイマー
 */
DX::StepTimer* CommonResources::GetStepTimer() const
{
	return m_pStepTimer;
}

/**
 * @brief デバイスリソースの取得
 * 
 * @return デバイスリソース
 */
DX::DeviceResources* CommonResources::GetDeviceResources() const
{
	return m_pDeviceResources;
}

/**
 * @brief 共通状態の取得
 * 
 * @return 共通状態
 */
CommonStates* CommonResources::GetCommonStates() const
{
	return m_pCommonStates;
}

/**
 * @brief デバックフォントの取得
 * 
 * @return デバックフォント
 */
MyLib::DebugFont* CommonResources::GetDebugFont() const
{
	return m_pDebugFont;
}

/**
 * @brief リソース管理の取得
 * 
 * @return リソース管理
 */
ResourceManager* CommonResources::GetResourceManager() const
{
	return m_pResourceManager;
}

/**
 * @brief キーボードトラッカーの取得
 * 
 * @return キーボードトラッカー
 */
DirectX::Keyboard::KeyboardStateTracker* CommonResources::GetKeyboardTracker() const
{
	return m_pKeyboardState;
}

/**
 * @brief マウストラッカーの取得
 * 
 * @return マウストラッカー
 */
DirectX::Mouse::ButtonStateTracker* CommonResources::GetMouseTracker() const
{
	return m_pMouseState;
}

/**
 * @brief ゲームパッドの取得
 * 
 * @return ゲームパッド
 */
DirectX::GamePad::ButtonStateTracker* CommonResources::GetGamePadTracker() const
{
	return m_pGamePadState;
}

/**
 * @brief シーン遷移マスク表示の取得
 *
 * @return トランジションマスク
 */
TransitionMask* CommonResources::GetTransitionMask() const
{
	return m_pTransitionMask;
}

/**
 * @brief 入力デバイス毎のスプライトの表記を切り替え器
 * 
 * @return 入力デバイス毎のスプライトの表記を切り替え器のポインタ
 */
InputDeviceSpriteResolver* CommonResources::GetInputDeviceSpriteResolver() const
{
	return m_pInputDeviceSpriteResolver;
}

/**
 * @brief 入力システムの取得
 * 
 * @return 入力システムのポインタ
 */
InputSystem* CommonResources::GetInputSystem() const
{
	return m_pInputSystem;
}



