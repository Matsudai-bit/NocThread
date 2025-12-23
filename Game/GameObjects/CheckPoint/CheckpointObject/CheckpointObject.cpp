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
#include "Game/Common/ResourceManager/ResourceManager.h"
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
void CheckpointObjectController::Initialize(ResourceManager* pResourceManager)
{
	// デフォルト回転の初期化
	SetInitialRotation(SimpleMath::Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(DEFAULT_ROTATION_Y_DEGREE), 0, 0.0f));

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
	m_rotateAngle += XMConvertToRadians(deltaTime * 50.0f);

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
	Matrix transMat	= Matrix::CreateTranslation		(GetPosition());
	Matrix rotateMat= Matrix::CreateFromQuaternion	(GetRotation());
	Matrix scaleMat	= Matrix::CreateScale			(GetScale());

	Matrix world = scaleMat * rotateMat * transMat;

	// 各パーツの行列の算出
	m_modelParts[ModelPartID::ROOT]->SetTransformMatrix(world);
	m_modelParts[ModelPartID::NECK]->SetTransformMatrix(Matrix::CreateRotationY(m_rotateAngle));
	//m_modelParts[ModelPartID::HEAD]->SetTransformMatrix(Matrix::CreateRotationX(90.0f * std::sin(m_rotateAngle))));

	// 行列の更新
	m_modelParts[ModelPartID::ROOT]->UpdateMatrix();
	// 描画
	m_modelParts[ModelPartID::ROOT]->Draw(context, *states, camera.GetViewMatrix(), camera.GetProjectionMatrix());
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
