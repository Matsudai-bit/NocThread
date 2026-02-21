//
// Game.cpp
//


#include "pch.h"

// ヘッダーファイルの読み込み

#define GAME_MODE

#include "Game.h"

// シーン関連
#include "Game/Scene/TitleScene/TitleScene.h"
#include "Game/Scene/GameplayScene/GameplayScene.h"
#include "Game/Scene/ResultScene/ResultScene.h"
#include "Game/Scene/Loading/LoadingScreen.h"
#include "Game/Common/Screen.h"

// ファクトリー関連
#include "Game/Common/Factory/InputBindingFactory/InputBindingFactory.h"

// フレームワーク関連
#include "Game/Common/Framework/SoundManager/SoundManager.h"

// グラフィック関連
#include "Game/Common/Graphics/GameEffect/GameEffectController.h"

// ImGui系のヘッダーのインクルード
#include "Library/ImGui/imgui.h"
#include "Library/ImGui/imgui_impl_win32.h"
#include "Library/ImGui/imgui_impl_dx11.h"


extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
    
    // コピーレンダーテクスチャの作成
    m_copyRenderTexture = std::make_unique<DX::RenderTexture>(m_deviceResources->GetBackBufferFormat());
}

Game::~Game()
{

    // フルスクリーン状態であれば、ウィンドウモードに戻す
    if (m_fullscreen && m_deviceResources->GetSwapChain())
    {
        auto window = m_deviceResources->GetWindow();
        auto rcWindowed = m_deviceResources->GetOutputSize();
        // ウィンドウがまだ有効な場合、スタイルを戻す努力をするのが良い
        if (IsWindow(window)) // window は Game クラスの HWND メンバー
        {
            SetWindowLongPtr(window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_NOTOPMOST,
                rcWindowed.left, rcWindowed.top,
                rcWindowed.right - rcWindowed.left,
                rcWindowed.bottom - rcWindowed.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }

        // ここで SetFullscreenState(FALSE, NULL) を呼び出す
        HRESULT hr = m_deviceResources->GetSwapChain()->SetFullscreenState(FALSE, NULL);
        if (FAILED(hr))
        {
            // エラーログ出力など
        }
    }
    // ImGuiの破棄処理
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    SoundManager::GetInstance()->RemoveAll();
    m_resourceManager->Clear();

    m_sceneManager->Finalize();


    m_collisionManager->Finalize();
    // 3. 全ての unique_ptr を明示的に破棄する（宣言の逆順が望ましい）
    m_sceneManager.reset();    // シーンを消す（TitleSceneなどもここで消える）
    m_transitionMask.reset();
    m_copyRenderTexture.reset();
    m_commonResources.reset();
    m_resourceManager.reset();
    m_debugFont.reset();       // SpriteFontなどがここで解放される
    m_states.reset();          // CommonStatesがここで解放される
    m_audioEngine.reset();


    OutputDebugStringA("--- Game Destructor Called ---\n");
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);


    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();
    //  DirectX11用の初期化
    ID3D11Device* device = m_deviceResources->GetD3DDevice();
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();
    // 追加 :　フルスクリーンの要求があるかどうか
    if (m_requestedFullscreen)
        SetFullscreenState(m_requestedFullscreen);

    // 再度　画面サイズを取得
    width = m_deviceResources->GetOutputSize().right;  
    height = m_deviceResources->GetOutputSize().bottom;

    // スクリーンにサイズを登録
    Screen::Get()->SetScreenSize(static_cast<float>(width), static_cast<float>(height));

    // サウンド管理の設定
    SoundManager::GetInstance()->SetResourceManager(m_resourceManager.get());

   // **** 生成 ****
    m_collisionManager = std::make_unique<CollisionManager>();

   // シーンの生成
   m_sceneManager = std::make_unique<MyLib::SceneManager<CommonResources>>();

    // トラッカーの作成
    m_keyboardStateTracker  = std::make_unique<Keyboard::KeyboardStateTracker>();
    m_mouseStateTracker     = std::make_unique<Mouse::ButtonStateTracker>();
    m_gamePadStateTracker   = std::make_unique<GamePad::ButtonStateTracker>();

    // 入力デバイス毎のスプライトの表記を切り替え器の作成
    m_inputDeviceSpriteResolver = std::make_unique<InputDeviceSpriteResolver>(m_keyboardStateTracker.get(), m_gamePadStateTracker.get());

    // 入力システムの作成
    m_InputSystem = std::make_unique<InputSystem>(m_keyboardStateTracker.get(), m_mouseStateTracker.get(), m_gamePadStateTracker.get());

    // 入力システムの設定
    m_InputSystem->AddActionMap(std::move(InputActionMapFactory::PlayerInputMapFactory().Create(DefaultSpawnDesc())));
    m_InputSystem->AddActionMap(std::move(InputActionMapFactory::UIInputMapFactory().Create(DefaultSpawnDesc())));
    m_InputSystem->AddActionMap(std::move(InputActionMapFactory::SystemInputMapFactory().Create(DefaultSpawnDesc())));

    // 共通リソースの生成
    m_commonResources = std::make_unique<CommonResources>(
        &m_timer,
        m_deviceResources.get(),
        m_states.get(),
        m_debugFont.get(),
        m_resourceManager.get(),
        m_keyboardStateTracker.get(),
        m_mouseStateTracker.get(),
        m_gamePadStateTracker.get(),
        m_copyRenderTexture.get(),
        m_transitionMask.get(),
        m_inputDeviceSpriteResolver.get(),
        m_InputSystem.get(),
        m_collisionManager.get()
    );


  

    // **** 初期化処理 ****
    // シーンの共通リソースの設定
    m_sceneManager->SetCommonResources(m_commonResources.get());

    // 開始シーンの設定
    m_sceneManager->RequestSceneChange<TitleScene, LoadingScreen>();

	// 画面クリア
    context->ClearRenderTargetView(m_deviceResources->GetRenderTargetView(), Colors::Black);


    // ***** ImGuiの初期設定 *****
    //  バージョンの確認
    IMGUI_CHECKVERSION();

    //  コンテキストの作成
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // キーボードによるナビゲーションの有効化
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // コントローラーによるナビゲーションの有効化

    //  Win32用の初期化
    ImGui_ImplWin32_Init(window);
   
    ImGui_ImplDX11_Init(device, context);

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);

    
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{    
    // **** ImGuiの更新処理 ****
//  新フレームの開始
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float deltaTime = float(timer.GetElapsedSeconds());
    m_collisionManager->UpdateTask(deltaTime);

    // キーボードトラッカーの更新処理
    auto kb = Keyboard::Get().GetState();
    m_keyboardStateTracker->Update(kb);

    // マウストラッカーの更新処理
    auto mouse = Mouse::Get().GetState();
    m_mouseStateTracker->Update(mouse);

    // ゲームパッドトラッカーの更新処理
    auto gamePad = GamePad::Get().GetState(0);
    m_gamePadStateTracker->Update(gamePad);

    // トランジションマスクの更新処理
    m_transitionMask->Update(deltaTime);

    // シーン管理の更新処理
    m_sceneManager->Update(deltaTime);


    m_collisionManager->StartThread();


        // 入力デバイス切り替え器の更新処理
        m_inputDeviceSpriteResolver->Update();

    m_InputSystem->Update();


    
#ifdef GAME_MODE

    if (kb.RightAlt && kb.RightControl)
    {
        m_sceneManager->RequestSceneChange<TitleScene>();
    }
#endif

}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();



    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

    // シーン管理の描画処理
    m_sceneManager->Render();

	// 画面コピー要求の処理
    TryCopyScreen(context);

    // 終了していない場合描画する
    if (!m_transitionMask->IsEnd())
    {
        m_transitionMask->Draw(context, m_states.get(), m_copyRenderTexture->GetShaderResourceView(), m_deviceResources->GetOutputSize());

    }

    // FPSを取得する
    uint32_t fps = m_timer.GetFramesPerSecond();

    // FPSの表示
    m_debugFont->AddString(0, 0, Colors::White, L"FPS=%d", fps);

    // デバッグフォントの描画
    m_debugFont->Render(m_states.get());

       // ****  ImGuiの描画処理 ****

        //  デモウィンドウの描画
    ImGui::ShowDemoWindow();
    ImGui::Render();
    //ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // ***** ここから下には書かない　******

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();


}

// Helper method to clear the back buffers.
void Game::Clear()
{


    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();
   
    context->ClearRenderTargetView(renderTarget, Colors::Gray);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetdeltaTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1280;
    height = 720;
}
void Game::RequestFullscreenState(BOOL value)
{
    m_requestedFullscreen = value;
}
void Game::SetFullscreenState(BOOL value)
{
    m_fullscreen = value;

    // 画面の情報を上書きする
    m_deviceResources->GetSwapChain()->SetFullscreenState(m_fullscreen, nullptr);
    // 新しくリソースの作成
    if (value) m_deviceResources->CreateWindowSizeDependentResources();
    auto screenSize = m_deviceResources->GetOutputSize();
    Screen::Get()->SetScreenSize(static_cast<float>(screenSize.right), static_cast<float>(screenSize.bottom));
}

#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;

    // 共通ステートの作成
    m_states = std::make_unique<CommonStates>(device);

    // デバッグフォントの作成
    m_debugFont = std::make_unique<MyLib::DebugFont>(
        device, context, L"Resources\\Font\\SegoeUI_18.spritefont");

    // リソース管理の作成
    m_audioEngine = std::make_unique<AudioEngine>();
    m_resourceManager = std::make_unique<ResourceManager>(device, m_audioEngine.get());

    // デバイスの設定
    m_copyRenderTexture->SetDevice(device);

    // トランジションマスクの作成
    m_transitionMask = std::make_unique<TransitionMask>(device, context, 1.0f);


}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

        // 画面サイズの取得
    RECT rect = m_deviceResources->GetOutputSize();

    // 射影行列の作成
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        XMConvertToRadians(45.0f)
        , static_cast<float>(rect.right) / static_cast<float>(rect.bottom)
        , 0.1f, 100.0f);

    m_copyRenderTexture->SetWindow(rect);


}

// 画面コピー要求がある場合、画面をコピーする
bool Game::TryCopyScreen(ID3D11DeviceContext1* context)
{

    // 画面コピー要求がある場合、画面をコピーする
    if (m_commonResources->IsCopyScreenRequest())
    {
        auto renderTarget = m_deviceResources->GetRenderTarget();
        context->CopyResource(m_copyRenderTexture->GetRenderTarget(), renderTarget);

        m_commonResources->SetCopyScreenRequest(false);

		return true;
    }

	return false;
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
