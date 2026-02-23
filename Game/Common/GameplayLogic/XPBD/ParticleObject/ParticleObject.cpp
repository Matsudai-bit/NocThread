/*****************************************************************//**
 * @file    ParticleObject.h
 * @brief   パーティクルオブジェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/05/14
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ParticleObject.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ParticleObject::ParticleObject()
	: m_mass{ 0.0f }
{

}



/**
 * @brief デストラクタ
 */
ParticleObject::~ParticleObject()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ParticleObject::Initialize()
{

}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime 経過時間
 *
 * @return なし
 */
void ParticleObject::Update(float deltaTime)
{

	UNREFERENCED_PARAMETER(deltaTime);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ParticleObject::Draw()
{

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ParticleObject::Finalize()
{

}
