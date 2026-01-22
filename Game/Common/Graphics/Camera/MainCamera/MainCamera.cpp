/*****************************************************************//**
 * @file    MainCamera.h
 * @brief   メインカメラに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/14
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "MainCamera.h"


// 静的変数の初期化
std::unique_ptr<MainCamera> MainCamera::s_instance = nullptr;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
MainCamera::MainCamera()
	: m_pMainCamera{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
MainCamera::~MainCamera()
{

}

/**
 * @brief インスタンスの取得
 */
MainCamera* MainCamera::GetInstance()
{
	if (s_instance.get() == nullptr)
	{
		s_instance.reset(new MainCamera());
	}

	return s_instance.get();
}

/**
 * @brief カメラの設定
 * 
 * @param[in] pCamera　カメラ
 */
void MainCamera::SetCamera(Camera* pCamera)
{
	m_pMainCamera = pCamera;
}

/**
 * @brief カメラの取得
 * 
 * @return メインカメラ
 */
const Camera* MainCamera::GetCamera() const
{
	return m_pMainCamera;
}

