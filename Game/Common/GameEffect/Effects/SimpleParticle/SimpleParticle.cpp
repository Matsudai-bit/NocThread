/*****************************************************************//**
 * @file    SimpleParticle.h
 * @brief   シンプルなパーティクルエフェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/12
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SimpleParticle.h"

#include "Game/Common/Shadow/CircularShadow/CircularShadow.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
SimpleParticle::SimpleParticle(DX::DeviceResources* pDeviceResources, DirectX::SimpleMath::Vector3 position)
	: m_commonStates{pDeviceResources->GetD3DDevice()}
{
	m_circleShadow = std::make_unique<CircularShadow>();
	m_circleShadow->Initialize(pDeviceResources, position.y);

	m_position = position;

}



/**
 * @brief デストラクタ
 */
SimpleParticle::~SimpleParticle()
{

}




/**
 * @brief 再生
 * 
 */
void SimpleParticle::Play()
{
}

/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void SimpleParticle::Update(float elapsedTime)
{
}

/**
 * @brief 描画処理
 * 
 * @param[in] view　ビュー行列
 * @param[in] proj	射影行列
 */
void SimpleParticle::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
//	m_circleShadow->Draw(view, proj, &m_commonStates, m_position);
}

/**
 * @brief プレイ中かどうか
 * 
 * @returns true  プレイ中
 * @returns false プレイしていない
 */
bool SimpleParticle::IsPlaying() const
{
	return true;
}

