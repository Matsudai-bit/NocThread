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
	: m_pCommonResources{ nullptr }
{
	GameObjectRegistry::GetInstance()->Registr(this);

	// トランスフォームの作成
	m_transform = std::make_unique<Transform>();
}



/**
 * @brief デストラクタ
 */
GameObject::~GameObject()
{

}


