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

	std::random_device randomDevice;
	m_randomGenerator.seed(randomDevice());

	m_model = GeometricPrimitive::CreateBox(pDeviceResources->GetD3DDeviceContext(), DirectX::SimpleMath::Vector3(1, 1, 1));

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
	// 時間のカウント
	m_timeCounter.UpperTime(elapsedTime);

	if (m_timeCounter.GetElapsedTime() >= 0.1f)
	{
		Particle particle;
		particle.time = 0.5f;

		particle.speed = 1.0f;
		
		float radius = 2.0f;
		float radian = XMConvertToRadians(m_randomGenerator() % 360);
		particle.position.y = m_position.y;
		particle.position.x = m_position.x + std::cos(radian) * radius;
		particle.position.z = m_position.z + std::sin(radian) * radius;

		m_particles.emplace_back(particle);

		m_timeCounter.Reset();
	}

	for (auto& particle : m_particles)
	{
		if (particle.time <= 0)
		{
			particle.isActive = false;
		}
		if (particle.isActive == false){continue;}

		particle.position.y += particle.speed * elapsedTime;
	}
}

/**
 * @brief 描画処理
 * 
 * @param[in] view　ビュー行列
 * @param[in] proj	射影行列
 */
void SimpleParticle::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{

	for (auto& particle : m_particles)
	{
		m_model->Draw(SimpleMath::Matrix::CreateTranslation(particle.position), view, proj);
	}

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

/**
 * @brief 座標の設定
 * 
 * @param[in] position 座標
 */
void SimpleParticle::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

