/*****************************************************************//**
 * @file    CommonResources.h
 * @brief   共通リソースクラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/03/22
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================
namespace DX
{
	class StepTimer;		// タイマー
	class DeviceResources;	// デバイスリソース
	class RenderTexture;
}

namespace MyLib
{
	class DebugFont;	// デバックフォント
}

class ResourceManager;	// リソース管理
class TransitionMask;	// シーン遷移マスク表示クラス
class InputDeviceSpriteResolver; // 入力デバイス毎のスプライトの表記を切り替え器
class InputManager;		// 入力管理


// クラスの定義 ===============================================================
/**
 * @brief 共通リソース
 */
class CommonResources
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	DX::StepTimer*			m_pStepTimer;		///< タイマー
	DX::DeviceResources*	m_pDeviceResources;	///< デバイスリソース
	DirectX::CommonStates*	m_pCommonStates;	///< 共通状態

	MyLib::DebugFont*		m_pDebugFont;		///< デバックフォント

	ResourceManager*		m_pResourceManager;	///< リソース管理

	DirectX::Keyboard::KeyboardStateTracker*	m_pKeyboardState;	///< キーボードの状態
	DirectX::Mouse::ButtonStateTracker*			m_pMouseState;		///< マウスの状態
	DirectX::GamePad::ButtonStateTracker*		m_pGamePadState;	///< ゲームパッドの状態

	TransitionMask* m_pTransitionMask;								///< シーン遷移マスク表示クラス

	InputDeviceSpriteResolver*	m_pInputDeviceSpriteResolver;	///< 入力デバイス毎のスプライトの表記を切り替え器

	InputManager*				m_pInputManager;	///< 入力管理

	// ---- 仮実装 ----
	DX::RenderTexture* m_pCopyRenderTexture;			///< 画面をコピーしたテクスチャ
	bool m_copyScreenRequest;
	// -----------------

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CommonResources(
		DX::StepTimer*			pStepTimer, 
		DX::DeviceResources*	pDeviceResources, 
		DirectX::CommonStates*	pCommonStates,
		MyLib::DebugFont*		pDebugFont,
		ResourceManager*		pResourceManager,
		DirectX::Keyboard::KeyboardStateTracker*pKeyboardState,
		DirectX::Mouse::ButtonStateTracker*		pMouseState,
		DirectX::GamePad::ButtonStateTracker*	pGamePadState,
		DX::RenderTexture*						pCopyRenderTexture,
		TransitionMask* pTransitionMask,
		InputDeviceSpriteResolver*	pInputDeviceSpriteResolver,
		InputManager*				pInputManager);


	// デストラクタ
	~CommonResources();


// 取得
public:

	// タイマーの取得
	DX::StepTimer* GetStepTimer()				const;

	// デバイスリソースの取得
	DX::DeviceResources* GetDeviceResources()	const;

	// 共通状態の取得
	DirectX::CommonStates* GetCommonStates()	const;

	// デバックフォントの取得
	MyLib::DebugFont* GetDebugFont()			const;

	// リソース管理
	ResourceManager* GetResourceManager()		const;

	// キーボードトラッカーの取得
	DirectX::Keyboard::KeyboardStateTracker* GetKeyboardTracker() const;

	// マウストラッカー取得
	DirectX::Mouse::ButtonStateTracker* GetMouseTracker() const;

	// ゲームパッドの取得
	DirectX::GamePad::ButtonStateTracker* GetGamePadTracker() const;

	// シーン遷移マスク表示の取得
	TransitionMask* GetTransitionMask() const;

	// 入力デバイス毎のスプライトの表記を切り替え器の取得
	InputDeviceSpriteResolver* GetInputDeviceSpriteResolver() const;

	// 入力管理の取得
	InputManager* GetInputManager() const;

	// 画面をコピーしたテクスチャの取得
	DX::RenderTexture* GetCopyScreenTexture() const { return m_pCopyRenderTexture; }

	void SetCopyScreenRequest(bool request) { m_copyScreenRequest = request; }
	bool IsCopyScreenRequest() const	{ return m_copyScreenRequest; }

};
