/*****************************************************************//**
 * @file    NormalGameplayState.h
 * @brief   ゲームプレイシーンの通常状態に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/15
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "NormalGameplayState.h"


#include "Game/Scene/GameplayScene/GameplayScene.h"
#include "Game/Scene/GameplayScene/State/PoseGameplayState/PoseGameplayState.h"
#include "Game/Common/CommonResources/CommonResources.h"
#include "Game/Common/Input/InputBindingFactory/InputBindingFactory.h"

#include "Game/Common/UserInterfaceTool/Canvas/Canvas.h"
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"
#include "Game/Common/Screen.h"
#include "Game/Common/ResourceManager/ResourceManager.h"

#include "Game/Manager/StageManager/StageManager.h"
using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
NormalGameplayState::NormalGameplayState()
	: m_isPrevConnectedGamepad{ false }
{

}



/**
 * @brief デストラクタ
 */
NormalGameplayState::~NormalGameplayState()
{

}

void NormalGameplayState::OnStartState()
{
	auto screen = Screen::Get();

	// 入力の作成
	m_systemInput = InputBindingFactory::CreateSystemInput();


	m_manualSprite = std::make_unique<Sprite>();

	// キャンバスへ登録
	GetOwner()->GetCanvas()->AddSprite(m_manualSprite.get());

	// スプライトのテクスチャ設定
	m_manualSprite->Initialize(GetOwner()->GetCommonResources()->GetResourceManager()->CreateTexture("Manual/ui_manual._ingame_pc.dds"));


	// スプライトの座標設定
	m_manualSprite->SetPosition(SimpleMath::Vector2(screen->GetLeftF() + 200.0f * screen->GetScreenScale(), screen->GetBottomF() - 40.0f * screen->GetScreenScale()));

	// スプライトの拡大率設定
	m_manualSprite->SetScale(0.2f * screen->GetScreenScale());
	
	// ゲームパッドへの接続されているかどうかの初期化
	m_isPrevConnectedGamepad = false;
}

void NormalGameplayState::OnExitState()
{
	// キャンバスへ登録
	GetOwner()->GetCanvas()->RemoveSprite(m_manualSprite.get());

}

void NormalGameplayState::OnUpdate(float deltaTime)
{
	// 入力の更新処理
	m_systemInput->Update(
	GetOwner()->GetCommonResources()->GetKeyboardTracker(), 
	GetOwner()->GetCommonResources()->GetMouseTracker(), 
	GetOwner()->GetCommonResources()->GetGamePadTracker());

	// ゲームオブジェクトの更新処理
	GetOwner()->UpdateInGameObjects(deltaTime);


	if (m_systemInput->IsInput(InputActionType::SystemActionID::PAUSE, InputSystem<InputActionType::SystemActionID>::InputOption::PRESSED))
	{
		// ポーズ状態にする
		GetStateMachine()->ChangeState<PoseGameplayState>();
	}

	// ガイドUIの変更を試みる
	TryChangeCurrentGuideUI();

	
}

void NormalGameplayState::OnDraw()
{
	GetOwner()->DrawInGameObjects();

}

/**
 * @brief 現在のガイドガイドUIの変更を試みる 
 * 
 * 簡易実装用の後にクラス化する
 * 
 * @returns true 成功
 * @returns false 失敗
 */
bool NormalGameplayState::TryChangeCurrentGuideUI()
{
	bool requestChange = false;
	bool changePC = true;;

	// ゲームパッドの接続されたらパッドガイドにに切り替える
	if (!m_isPrevConnectedGamepad &&  GetOwner()->GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = false;
		requestChange = true;
		m_isPrevConnectedGamepad = true;
	}

	// ゲームパッドが接続されなくなったらPCガイドに切り替える
	if (m_isPrevConnectedGamepad && !GetOwner()->GetCommonResources()->GetGamePadTracker()->GetLastState().IsConnected())
	{
		changePC = true;
		requestChange = true;
		m_isPrevConnectedGamepad = false;
	}

	if (requestChange)
	{
		std::string filePath;

		if (changePC)
		{
			filePath = "Manual/ui_manual._ingame_pc.dds";
		}

		else 
		{
			filePath = "Manual/ui_manual._ingame_gamepad.dds";
		}

		// 拡大率と座標を保持する
		float scale = m_manualSprite->GetScale();
		SimpleMath::Vector2 position = m_manualSprite->GetPosition();
		m_manualSprite->Initialize(GetOwner()->GetCommonResources()->GetResourceManager()->CreateTexture(filePath));
		m_manualSprite->SetScale(scale);
		m_manualSprite->SetPosition(position);

		return true;
	}

	return false;
}

