/*****************************************************************//**
 * @file    SimpleModel.h
 * @brief   DirectX::Modelクラスをゲ使いやすくしたクラスに関するヘッダーファイルに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/05/29
 *********************************************************************/

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SimpleModel.h"

// フレームワーク関連
#include "Game/Common/Framework/ResourceManager/ResourceManager.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================

/**
 * @brief コンストラクタ
 * 
 * @param[in] context  コンテスト
 * @param[in] pStates　状態
 */
SimpleModel::SimpleModel(CommonStates* pStates)
	: m_pStates{ pStates }
	, m_scale {1.0f}
{

}


/**
 * @brief 描画処理
 * 
 * @param[in] proj　射影行列
 * @param[in] view　ビュー行列
 */
void SimpleModel::Draw(CommonResources* pCommonResources,ID3D11DeviceContext* context, const DirectX::SimpleMath::Matrix& proj, const DirectX::SimpleMath::Matrix& view)
{
	using namespace SimpleMath;
	UNREFERENCED_PARAMETER(view);
	UNREFERENCED_PARAMETER(proj);
	UNREFERENCED_PARAMETER(pCommonResources);
	UNREFERENCED_PARAMETER(context);


	

	Matrix transform = Matrix::CreateTranslation(m_position);
	Matrix rotate = Matrix::CreateFromYawPitchRoll(m_rotation);
	Matrix scale = Matrix::CreateScale(m_scale);

	Matrix world = scale * rotate * transform;



	

	//m_model.Draw(context, *m_pStates, world, view, proj);
}

