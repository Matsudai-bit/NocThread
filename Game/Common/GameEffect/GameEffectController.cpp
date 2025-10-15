/*****************************************************************//**
 * @file    GameEffectController.h
 * @brief   ゲームエフェクトコントローラに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/11
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GameEffectController.h"


// 静的変数の初期化
std::unique_ptr<GameEffectController> GameEffectController::s_instance = nullptr;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GameEffectController::GameEffectController()
    : m_pGameEffectManager{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
GameEffectController::~GameEffectController()
{

}

/**
 * @brief エフェクトの再生
 *
 * @param[in] effect　	エフェクト
 * @param[in] clip		クリップ
 *
 * @returns 1以上 識別番号
 * @returns 0未満 エラー
 */
int GameEffectController::PlayEffect(std::unique_ptr<IGameEffect> effect, GameEffectManager::EffectClip clip)
{
    return m_pGameEffectManager->PlayEffect(std::move(effect), clip);
}

/**
 * @brief エフェクトの停止
 *
 * @param[in] id 識別番号
 */
void GameEffectController::StopEffect(unsigned int id)
{
    m_pGameEffectManager->StopEffect(id);
}

/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime　経過時間
 */
void GameEffectController::Update(float deltaTime)
{
    m_pGameEffectManager->Update(deltaTime);
}

/**
 * @brief 描画処理
 */
void GameEffectController::Draw()
{
    m_pGameEffectManager->Draw(m_view, m_projection);
}

/**
 * @brief ビュー行列の設定
 * 
 * @param[in] view　ビュー行列
 */
void GameEffectController::SetView(const DirectX::SimpleMath::Matrix& view)
{
    m_view = view;
}

/**
 * @brief 射影行列の設定
 * 
 * @param[in] projection　射影行列
 */
void GameEffectController::SetProjection(const DirectX::SimpleMath::Matrix& projection)
{
    m_projection = projection;
}

/**
 * @brief インスタンスの取得
 * 
 * @return インスタンス
 */
GameEffectController* GameEffectController::GetInstance()
{
    if (s_instance.get() == nullptr)
    {
        s_instance.reset(new GameEffectController);
    }

    return s_instance.get();
}

/**
 * @brief ゲームエフェクト管理の設定
 * 
 * @param[in] pGameEffectManager
 */
void GameEffectController::SetGameEffectManager(GameEffectManager* pGameEffectManager)
{
    m_pGameEffectManager = pGameEffectManager;
}

