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

// ライブラリ関連
#include "Library/MyLib/MathUtils/MathUtils.h"
#include "Library/DirectXFramework/DebugDraw.h"
#include "Library/DirectXFramework/StepTimer.h"
#include "Library/MyLib/DirectXMyToolKit/DebugFont/DebugFont.h"

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"
#include "Game/GameObjects/RopeObject/XPBDSimulator/SimParticle/SimParticle.h"

// ゲームプレイロジック関連
#include "Game/Common/GameplayLogic/CollisionManager/CollisionManager.h"

// グラフィック関連
#include "Game/Common/Graphics/Camera/MainCamera/MainCamera.h"
#include "Game/Common/Graphics/Camera/Camera.h"

// ユーティリティ関連
#include "Game/GameObjects/RopeObject/XPBDSimulator/Constraint/SteeringConstraint/SteeringConstraint.h"



using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pParticle パーティクル
 */
SteeringConstraintFactory::SteeringConstraintFactory(const CommonResources* pCommonResources)
    : ConstraintFactoryBase{ true }
    , m_pCommonResources{ pCommonResources }
    , m_elapsedTimeCounter{}
{
	m_playerInput = InputBindingFactory::PlayerInputFactory().Create(DefaultSpawnDesc());
}



/**
 * @brief デストラクタ
 */
SteeringConstraintFactory::~SteeringConstraintFactory()
{

}

/**
 * @brief リセット
 * 
 */
void SteeringConstraintFactory::Reset()
{
    m_elapsedTimeCounter.Reset();
}

/**
 * @brief 衝突制約の作成
 * 
 * @param[in] pParticle->パーティクル
 * 
 * @return 作成した衝突制約
 */
std::vector<std::unique_ptr<IConstraint>> SteeringConstraintFactory::CreateConstraint(std::vector<XPBDSimulator::Particle>* pParticles, XPBDSimulator::Parameter paramater)
{
	UNREFERENCED_PARAMETER(paramater);
    using namespace SimpleMath;

    // 作成した制約
    std::vector<std::unique_ptr<IConstraint>> creationConstraints;

    // 前フレームからの時間を取得
    float deltaTime = static_cast<float>(m_pCommonResources->GetStepTimer()->GetElapsedSeconds());
    m_elapsedTimeCounter.UpperTime(deltaTime);
    
    // ワイヤーの慣性の減衰率の算出
    float inertiaDamping = 1.f - std::min(DUMPING_DURATION, m_elapsedTimeCounter.GetElapsedTime()) / DUMPING_DURATION;

    // カメラの向きから移動基準となる「正面・右」ベクトルを算出
    auto camera = MainCamera::GetInstance()->GetCamera();
    Vector3 cameraForward = camera->GetTarget() - camera->GetEye();
    cameraForward.y = 0.0f;
    cameraForward.Normalize();

    // 処理用変数
    const int totalParticleNum = static_cast<int>(pParticles->size());
    const Vector3 rootPosition = pParticles->front().simP.GetXi(); // ワイヤーの根元位置

    //  *** 入力による操舵力を算出 ****
    auto inputSteeringForce = ComputeInputSteeringForce(cameraForward);

    for (int i = 0; i < totalParticleNum; ++i)
    {
        auto& particle = (*pParticles)[i].simP;

        // **** 自動で適用する操舵力を算出 ****
        Vector3 autoSteeringForce = ComputeCameraDirectedSteeringForce(cameraForward, rootPosition, particle, inertiaDamping);

        // ****最終的なステアリング量の算出 ****
        // 自動補正 ＋ プレイヤーの入力
        Vector3 totalSteeringOffset = autoSteeringForce + inputSteeringForce;

        // ステアリングがかかっていれば作成する
        if (!MyLib::ApproxEqual(totalSteeringOffset.LengthSquared(), 0.0f))
        {
            // --- 先端ほど強く流れるように調整 ---
            // 根元 (0.0) から先端 (1.0) に向かって影響度を上げる
            float influenceWeight = static_cast<float>(i) / static_cast<float>(totalParticleNum);

            // 予測位置を基準に、理想の位置(Target)を決定する
            Vector3 finalTargetPos = particle.GetXi() + (totalSteeringOffset * influenceWeight);
            // --- 制約の生成 ---
            creationConstraints.emplace_back(std::move(
                std::make_unique<SteeringConstraint>(m_pCommonResources, &particle, finalTargetPos)
            ));
        }


    }

    return creationConstraints;

}


/**
 * @brief 入力に対するステアリング力を計算する
 * 
 * @param[in] cameraForward カメラの正面ベクトル
 * 
 * @return 入力に対するステアリング力
 */
DirectX::SimpleMath::Vector3 SteeringConstraintFactory::ComputeInputSteeringForce(const DirectX::SimpleMath::Vector3& cameraForward) const
{
    using namespace SimpleMath;

	m_playerInput->Update(m_pCommonResources->GetKeyboardTracker(), m_pCommonResources->GetMouseTracker(), m_pCommonResources->GetGamePadTracker());

    // 入力の取得
    auto kb = Keyboard::Get().GetState();
    Vector3 rawInput;

    // 奥へ
    if (m_playerInput->IsInput(InputActionType::PlyayerActionID::FRONT_MOVE)) { rawInput += SimpleMath::Vector3::Forward; }
    // 手前へ
    if (m_playerInput->IsInput(InputActionType::PlyayerActionID::BACK_MOVE)) { rawInput += SimpleMath::Vector3::Backward; }
    // 右へ
    if (m_playerInput->IsInput(InputActionType::PlyayerActionID::RIGHT_MOVE)) { rawInput += SimpleMath::Vector3::Right; }
    // 左へ
    if (m_playerInput->IsInput(InputActionType::PlyayerActionID::LEFT_MOVE)) { rawInput += SimpleMath::Vector3::Left; }

    Vector3 cameraRight = cameraForward.Cross(Vector3::Up);
    cameraRight.Normalize();

    // 入力をカメラ空間の移動方向に変換
    Vector3 moveDirection = (rawInput.z * cameraForward) + (-rawInput.x * cameraRight);
    if (moveDirection.LengthSquared() > 0.001f) 
    {
        moveDirection.Normalize();
    }

    return (moveDirection * INPUT_INERTIA_STRENGTH);
}

/**
 * @brief カメラが向く方向へ導くステアリング力を計算する
 * 
 * @param[in] cameraForward         カメラの正面ベクトル
 * @param[in] particleRootPosition  パーティクルの根本座標
 * @param[in] particle              ステアリングを適用させるパーティクル
 * @param[in] inertiaDamping        慣性の減衰率
 * 
 * @return カメラが向く方向へ導くステアリング力
 */
DirectX::SimpleMath::Vector3 SteeringConstraintFactory::ComputeCameraDirectedSteeringForce(
    const DirectX::SimpleMath::Vector3& cameraForward,
    const DirectX::SimpleMath::Vector3& particleRootPosition,
    const SimParticle& particle,
    const float& inertiaDamping) const
{
    using namespace SimpleMath;

     // パーティクル → 根元方向の水平ベクトル 
    Vector3 particleToRootDir = particleRootPosition - particle.GetXi();
    particleToRootDir.y = 0.0f;
    particleToRootDir.Normalize();

    // カメラ方向との差分による慣性ステアリング力 
    Vector3 steeringForce = (particleToRootDir - cameraForward) * AUTO_INERTIA_STRENGTH * inertiaDamping;

    // オートオフセット：ワイヤーが進行方向の逆側に遅れてついてくるような力
    return steeringForce;
}
