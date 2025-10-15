/*****************************************************************//**
 * @file    GameObject.h
 * @brief   ゲームオブジェクトに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/03/22
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "GameObject.h"

#include "Game/Common/GameObjectRegistry/GameObjectRegistry.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GameObject::GameObject()
	: m_position{}
	, m_pCommonResources{ nullptr }
	, m_scale{ 1.0f }
{
	GameObjectRegistry::GetInstance()->Registr(this);
}



/**
 * @brief デストラクタ
 */
GameObject::~GameObject()
{

}
//
///**
// * @brief 更新処理
// * 
// * @param[in] deltaTime　経過時間
// */
//void GameObject::Update(float deltaTime)
//{
//	UNREFERENCED_PARAMETER(deltaTime);
//}
//
///**
// * @brief 描画処理
// * 
// * @param[in] view　		ビュー行列
// * @param[in] projection　	射影行列
// */
//void GameObject::Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
//{
//	UNREFERENCED_PARAMETER(view);
//	UNREFERENCED_PARAMETER(projection);
//}



/**
 * @brief 回転の取得
 * 
 * @return 回転
 */
DirectX::SimpleMath::Quaternion GameObject::GetRotate() const
{
	
	return m_rotation ;
}
DirectX::SimpleMath::Quaternion GameObject::GetDefaultRotate() const
{
	return m_defaultRotation;
}
/**
 * @brief 回転の設定
 * 
 * @param[in] rotate　回転
 */
void GameObject::SetRotate(const DirectX::SimpleMath::Quaternion& rotate)
{
	m_rotation = rotate;
}

void GameObject::SetDefaultRotate(const DirectX::SimpleMath::Quaternion& rotate)
{
	m_defaultRotation = rotate;
}

/**
 * @brief 共通リソースの取得
 * 
 * @return 共通リソース
 */
const CommonResources* GameObject::GetCommonResources() const
{
	return m_pCommonResources;
}

/**
 * @brief 座標の取得
 *
 * @return 座標
 */
DirectX::SimpleMath::Vector3 GameObject::GetPosition() const
{
	return m_position;
}
/**
 * @brief 座標の設定
 * 
 * @param[in] position　設定座標
 */
void GameObject::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}


/**
 * @brief 拡大率の取得
 *
 * @return 拡大率
 */
float GameObject::GetScale() const
{
	return m_scale;
}
/**
 * @brief 拡大率の設定
 *
 * @param[in] scale　設定拡大率
 */
void GameObject::SetScale(const float& scale)
{
	m_scale = scale;
}


DirectX::SimpleMath::Vector3 GameObject::GetForward() const
{
	SimpleMath::Vector3 forward = SimpleMath::Vector3::Forward;
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateFromQuaternion(GetRotate());

	return SimpleMath::Vector3::TransformNormal(forward, rot);
}


/**
 * @brief 共通リソースの設定
 * 
 * @param[in] pCommonResources　共通リソース
 */
void GameObject::SetCommonResources(const CommonResources* pCommonResources)
{
	m_pCommonResources = pCommonResources;
}
