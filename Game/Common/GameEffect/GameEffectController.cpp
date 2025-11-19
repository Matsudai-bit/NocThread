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
    : m_pGameEffectManager  { nullptr }
    , m_pCamera             { nullptr }
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
 * @brief カメラの設定
 * 
 * @param[in] pCamera　カメラのポインタ
 */
void GameEffectController::SetCamera(const Camera* pCamera)
{
    m_pCamera = pCamera;
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

