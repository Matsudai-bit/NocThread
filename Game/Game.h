//
// Game.h
//

#pragma once

#include "Game/Common/DeviceResources.h"
#include "Game/Common/StepTimer.h"

#include<memory>

// **** 追加インクルード ****

// ライブラリ
#include "Library/ImaseLib/DebugFont.h"             // デバックフォント
#include "Library/DirectXFramework/RenderTexture.h" // レンダーテクスチャ

// システム
#include "Game/Common/CommonResources/CommonResources.h"    // 共通リソース
#include "Game/Common/ResourceManager/ResourceManager.h"    // リソース管理
#include "Game/Manager/SceneManager/SceneManager.h"         // シーン管理

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
// 追加データメンバ
private:
    // 射影行列
    DirectX::SimpleMath::Matrix m_proj;

    // 共通ステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    // デバッグフォント
    std::unique_ptr<Imase::DebugFont> m_debugFont;

    // リソース
    std::unique_ptr<CommonResources> m_commonResources;     ///< 共通リソース
    std::unique_ptr<MyLib::SceneManager<CommonResources>>       m_sceneManager;     ///< シーン管理
    std::unique_ptr<ResourceManager>    m_resourceManager;  ///< リソース管理
    std::unique_ptr<DirectX::AudioEngine>                       m_audioEngine;          ///< オーディオエンジン

    // 入力デバイス
    std::unique_ptr<DirectX::Mouse::ButtonStateTracker>         m_mouseStateTracker;    ///< マウストラッカー
    std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker>    m_keyboardStateTracker; ///< キーボードトラッカー
    std::unique_ptr<DirectX::GamePad::ButtonStateTracker>       m_gamePadStateTracker;  ///< ゲームパッドトラッカー

    // その他
    std::unique_ptr<DX::RenderTexture>  m_copyRenderTexture;

public:

    Game() noexcept(false);
    ~Game();

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

    void RequestFullscreenState(BOOL value);
    void SetFullscreenState(BOOL value);

private:

    BOOL m_fullscreen = false;
    BOOL m_requestedFullscreen = false;

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;



};
