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
 */
CommonResources::CommonResources(
	DX::StepTimer*			pStepTimer,
	DX::DeviceResources*	pDeviceResources,
	CommonStates*	pCommonStates,
	Imase::DebugFont*		pDebugFont,
	ResourceManager*		pResourceManager,
	Keyboard::KeyboardStateTracker* pKeyboardState,
	Mouse::ButtonStateTracker* pMouseState)
	: m_pStepTimer		{pStepTimer}
	, m_pDeviceResources{pDeviceResources}
	, m_pCommonStates	{pCommonStates}
	, m_pDebugFont		{pDebugFont}
	,m_pResourceManager {pResourceManager}
	, m_pKeyboardState	{ pKeyboardState }
	, m_pMouseState		{ pMouseState }

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
Imase::DebugFont* CommonResources::GetDebugFont() const
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

