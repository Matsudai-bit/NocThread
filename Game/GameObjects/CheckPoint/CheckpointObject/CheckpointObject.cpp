/*****************************************************************//**
 * @file    CheckpointObjectController.h
 * @brief   チェックポイントオブジェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/12/18
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CheckpointObject.h"

// フレームワーク関連
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"
#include "Game/Common/Framework/GameObjectRegistry/GameObjectRegistry.h"

// グラフィック関連
#include "Game/Common/Camera/Camera.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CheckpointObjectController::CheckpointObjectController()
	: m_rotateAngle{}
	, m_state{}
	, m_lerpT{}
{

}



/**
 * @brief デストラクタ
 */
CheckpointObjectController::~CheckpointObjectController()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CheckpointObjectController::Initialize(ResourceManager* pResourceManager, ID3D11Device* device)
{
	
	// イベントの追加
	AddListener(CheckpointControllEventID::LOOK_AT_HELICOPTER, [this]() { LookAtHelicopter(); }, 0);

	// 待機状態
	m_state = State::IDLING;

	// デフォルト回転の初期化
	m_transform.SetInitialRotation(SimpleMath::Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(DEFAULT_ROTATION_Y_DEGREE), 0, 0.0f));

	// モデルの作成
	m_models[ModelPartID::BODY] = std::make_unique<Model>(pResourceManager->CreateModel("movingLight_body.sdkmesh"));
	m_models[ModelPartID::NECK] = std::make_unique<Model>(pResourceManager->CreateModel("movingLight_arm.sdkmesh"));
	m_models[ModelPartID::HEAD] = std::make_unique<Model>(pResourceManager->CreateModel("movingLight_head.sdkmesh"));

	// モデルパーツの作成
	m_modelParts[ModelPartID::ROOT] = std::make_unique< MyLib::ModelPart>();
	m_modelParts[ModelPartID::BODY] = std::make_unique<MyLib::ModelPart>(m_models[ModelPartID::BODY].get());
	m_modelParts[ModelPartID::NECK] = std::make_unique<MyLib::ModelPart>(m_models[ModelPartID::NECK].get());
	m_modelParts[ModelPartID::HEAD] = std::make_unique<MyLib::ModelPart>(m_models[ModelPartID::HEAD].get());

	// 親子関係の設定
	m_modelParts[ModelPartID::ROOT]->SetChild(m_modelParts[ModelPartID::BODY].get());
	m_modelParts[ModelPartID::BODY]->SetChild(m_modelParts[ModelPartID::NECK].get());
	m_modelParts[ModelPartID::NECK]->SetChild(m_modelParts[ModelPartID::HEAD].get());

	// 初期行列の設定
	m_modelParts[ModelPartID::BODY]->SetInitialMatrix(SimpleMath::Matrix::Identity);
	m_modelParts[ModelPartID::NECK]->SetInitialMatrix(SimpleMath::Matrix::CreateTranslation(SimpleMath::Vector3(0.0f, 0.35f, 0.0f)));
	m_modelParts[ModelPartID::HEAD]->SetInitialMatrix(SimpleMath::Matrix::CreateTranslation(SimpleMath::Vector3(0.0f, 1.65f, 0.0f)));


	m_modelParts[ModelPartID::ROOT]->SetupMatrix();


	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetLightingEnabled(false);
	m_basicEffect->SetLightEnabled(0, false);
	m_basicEffect->SetLightEnabled(1, false);
	m_basicEffect->SetLightEnabled(2, false);

	// --- 2. 色を明示的に指定（例：真っ赤） ---
	m_basicEffect->SetDiffuseColor	(Colors::Yellow);
	m_basicEffect->SetEmissiveColor	(SimpleMath::Vector3(0.0f, 0.0f, 0.0f)); // 発光色をゼロにする
	m_basicEffect->SetAmbientLightColor(SimpleMath::Vector3(0.0f, 0.0f, 0.0f)); // 環境光をゼロにする
	m_basicEffect->SetAlpha(0.5f); // 不透明にする

	m_basicEffect->SetTextureEnabled(false);	
	m_basicEffect->SetVertexColorEnabled(false);

	CreateInputLayoutFromEffect<VertexPositionColor>(device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf());
}



/**
 * @brief 更新処理
 *
 * @param[in] deltaTime フレーム間の経過時間
 *
 * @return なし
 */
void CheckpointObjectController::Update(float deltaTime)
{
	// イベントの適用
	ApplyEvents();
	if (State::IDLING == m_state)
	{
		m_rotateAngle += XMConvertToRadians(deltaTime * 50.0f);
		m_neckRotation = SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitY, m_rotateAngle);
	}


	if (State::LOOK_AT_HELICOPTER == m_state)
	{
		// 補間値を求める
		float lerpT = deltaTime / 2.0f;
		m_lerpT += lerpT;

		m_lerpT = std::min(m_lerpT, 1.0f);
		m_neckRotation = SimpleMath::Quaternion::Lerp(m_startNeckRotation, m_targetNeckRotation, m_lerpT);
		m_headRotation = SimpleMath::Quaternion::Lerp(m_startHeadRotation, m_targetHeadRotation, m_lerpT);
	}
}


/**
 * @brief 描画処理
 * 
 * @param[in] context　 コンテスト
 * @param[in] states	共通ステート
 * @param[in] camera	カメラ
 */
void CheckpointObjectController::Draw(ID3D11DeviceContext1* context, DirectX::CommonStates* states, const Camera& camera)
{
	using namespace SimpleMath;



	// 行列の作成
	Matrix transMat	= Matrix::CreateTranslation		(m_transform.GetPosition());
	Matrix rotateMat= Matrix::CreateFromQuaternion	(m_transform.GetRotation());
	Matrix scaleMat	= Matrix::CreateScale			(m_transform.GetScale());

	Matrix world = scaleMat * rotateMat * transMat;

	// 各パーツの行列の算出
	m_modelParts[ModelPartID::ROOT]->SetTransformMatrix(world);
	m_modelParts[ModelPartID::NECK]->SetTransformMatrix(Matrix::CreateFromQuaternion(m_neckRotation)); 
	m_modelParts[ModelPartID::HEAD]->SetTransformMatrix(Matrix::CreateFromQuaternion(m_headRotation));

	// 行列の更新
	m_modelParts[ModelPartID::ROOT]->UpdateMatrix();
	// 描画
	m_modelParts[ModelPartID::ROOT]->Draw(context, *states, camera.GetViewMatrix(), camera.GetProjectionMatrix());


	if (m_state == State::LOOK_AT_HELICOPTER && m_lerpT >= 1.0f)
	{
		Vector3 targetPosition = Vector3(130.0f, 110.6f, 120.2f);
		float length = Vector3::Distance(m_transform.GetPosition(), targetPosition);
		length /= 2.0f;

		// 目印
		auto cylinder = DirectX::GeometricPrimitive::CreateCylinder(context, length, 0.8f);

		world = SimpleMath::Matrix::CreateTranslation(0.0f, length / 2.0f, 0.0f) * m_modelParts[ModelPartID::HEAD]->GetWorldMatrix();
		m_basicEffect->Apply(context);
		m_basicEffect->SetWorld(world);
		m_basicEffect->SetView(camera.GetViewMatrix());
		m_basicEffect->SetProjection(camera.GetProjectionMatrix());

		cylinder->Draw(m_basicEffect.get(), m_inputLayout.Get(), true, false);

	}


}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CheckpointObjectController::Finalize()
{

}

/**
 * @brief ヘリコプターみるよう要求
 * 
 */
void CheckpointObjectController::LookAtHelicopter()
{
	using namespace SimpleMath;
	m_state = State::LOOK_AT_HELICOPTER;

	Vector3 targetPosition = Vector3(130.0f, 110.6f, 120.2f);

	// 首の回転の算出
	Vector3 direction = targetPosition - m_transform.GetPosition();
	direction.Normalize();

	// 角度を求める
	float radianY = atan2f(direction.x, direction.z);

	m_startNeckRotation = m_neckRotation;
	m_targetNeckRotation = Quaternion::CreateFromAxisAngle(Vector3::UnitY, radianY);


	// 頭の回転
	// 角度を求める
	float horizontal = std::sqrtf(direction.x * direction.x + direction.z * direction.z);
	float radianX = atan2f(horizontal, direction.y);
	m_startHeadRotation = m_headRotation;
	m_targetHeadRotation = Quaternion::CreateFromAxisAngle(Vector3::UnitX, radianX);

	m_lerpT = 0.0f;
}
