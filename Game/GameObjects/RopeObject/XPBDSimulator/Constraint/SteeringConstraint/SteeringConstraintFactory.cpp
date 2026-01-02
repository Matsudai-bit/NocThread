/*****************************************************************//**
 * @file    SteeringConstraintFactory.h
 * @brief   衝突制約の生成器に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/10/09
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SteeringConstraintFactory.h"

#include "Game/Common/Collision/CollisionManager/CollisionManager.h"

#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/SteeringConstraint/SteeringConstraint.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"

#include "Game/Common/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Camera/Camera.h"

#include "Library/MyLib/MathUtils/MathUtils.h"
#include "Library/DirectXFramework/DebugDraw.h"

#include "Game/Common/CommonResources/CommonResources.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
SteeringConstraintFactory::SteeringConstraintFactory(const CommonResources* pCommonResources, XPBDSimulator::Parameter paramater)
	: ConstraintFactoryBase		{ true }
	, m_paramater			{ paramater }
	, m_pCommonResources{ pCommonResources }
{
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(pCommonResources->GetDeviceResources()->GetD3DDeviceContext());
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(pCommonResources->GetDeviceResources()->GetD3DDevice());
	m_basicEffect->SetLightEnabled(0, false);
	m_basicEffect->SetLightEnabled(1, false);
	m_basicEffect->SetLightEnabled(2, false);

	m_basicEffect->SetVertexColorEnabled(true);
	m_basicEffect->SetTextureEnabled(false);

	CreateInputLayoutFromEffect<VertexPositionColor>(pCommonResources->GetDeviceResources()->GetD3DDevice(), m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());
}



/**
 * @brief デストラクタ
 */
SteeringConstraintFactory::~SteeringConstraintFactory()
{

}

/**
 * @brief 衝突制約の作成
 * 
 * @param[in] pParticle->パーティクル
 * 
 * @return 作成した衝突制約
 */
std::vector<std::unique_ptr<IConstraint>> SteeringConstraintFactory::CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles)
{
    using namespace SimpleMath;
    std::vector<std::unique_ptr<IConstraint>> creationConstraints;

    // ---------------------------------------------------------
    // 1. 全パーティクル共通の入力・カメラ計算（ループの外で1回だけ行う）
    // ---------------------------------------------------------

    auto kb = Keyboard::Get().GetState();
    Vector3 rawInput;
    if (kb.A) rawInput += Vector3::Left;
    if (kb.D) rawInput += Vector3::Right;
    if (kb.W) rawInput += Vector3::Backward;
    if (kb.S) rawInput += Vector3::Forward;

    // カメラの向きから移動基準となる「正面・右」ベクトルを算出
    auto camera = MainCamera::GetInstance()->GetCamera();
    Vector3 cameraForward = camera->GetTarget() - camera->GetEye();
    cameraForward.y = 0.0f;
    cameraForward.Normalize();

    Vector3 cameraRight = cameraForward.Cross(Vector3::Up);
    cameraRight.Normalize();

    // 入力をカメラ空間の移動方向に変換
    Vector3 moveDirection = (-rawInput.z * cameraForward) + (-rawInput.x * cameraRight);
    if (moveDirection.LengthSquared() > 0.001f) {
        moveDirection.Normalize();
    }

    // ---------------------------------------------------------
    // 2. 各パーティクルごとの理想位置（Target）の算出
    // ---------------------------------------------------------

    const int totalParticles = static_cast<int>(pParticles->size());
    const Vector3 rootPosition = pParticles->front().simP.GetXi(); // ワイヤーの根元位置

    for (int i = 0; i < totalParticles; ++i)
    {
        auto& particle = (*pParticles)[i].simP;

        // --- 自動補正（慣性・膨らみ）の計算 ---
        // 根元から自分への方向を出し、カメラの向きと合成して「たわみ」を作る
        Vector3 rootToParticle = rootPosition - particle.GetXi();
        rootToParticle.y = 0.0f;
        rootToParticle.Normalize();

        // オートオフセット：ワイヤーが進行方向の逆側に遅れてついてくるような力
        Vector3 autoInertiaOffset = (rootToParticle - cameraForward) * 50.0f;

        // --- 最終的なステアリング量の算出 ---
        // 自動補正 ＋ プレイヤーの入力。 100.0f は演出の強さ（ウェイト）
        Vector3 totalSteeringOffset = autoInertiaOffset + (moveDirection * 130.0f);

        // --- 先端ほど強く流れるように調整 ---
        // 根元 (0.0) から先端 (1.0) に向かって影響度を上げる
        float influenceWeight = static_cast<float>(i) / static_cast<float>(totalParticles);

        // 予測位置を基準に、理想の位置(Target)を決定する
        Vector3 finalTargetPos = particle.GetXi() + (totalSteeringOffset * influenceWeight);

        // --- 制約の生成 ---
        creationConstraints.emplace_back(std::move(
            std::make_unique<SteeringConstraint>(m_pCommonResources, &particle, finalTargetPos)
        ));
    }

    return creationConstraints;
	
}
