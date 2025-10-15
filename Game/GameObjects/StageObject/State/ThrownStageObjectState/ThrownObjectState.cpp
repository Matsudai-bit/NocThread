/*****************************************************************//**
 * @file    ThrownStageObjectState.h
 * @brief   ステージオブジェクトの投げられている状態に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/25
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ThrownStageObjectState.h"

#include "Game/GameObjects/StageObject/StageObject.h"
#include "Game/GameObjects/StageObject/State/IdleStageObjectState.h"

// ライブラリ
#include "Library/MyLib/MathUtils/MathUtils.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ThrownStageObjectState::ThrownStageObjectState()
{

}



/**
 * @brief デストラクタ
 */
ThrownStageObjectState::~ThrownStageObjectState()
{

}



/**

/**
 * @brief 更新処理
 *
 * @param[in] deltaTime　経過時間
 */
void ThrownStageObjectState::OnUpdate(float deltaTime)
{
	GetOwner()->ApplyPhysic(deltaTime);
	GetOwner()->Move(deltaTime);

	SimpleMath::Vector3 velocity = GetOwner()->GetVelocity();

	// **** 注意 : 今後Y軸も考慮する必要がある ****
	if (MyLib::ApproxEqual(velocity.x, 0.0f) &&  MyLib::ApproxEqual(velocity.z, 0.0f))
	{
		GetStateMachine()->ChangeState<IdleStageObjectState>();
	}
}

/**
 * @brief 描画処理
 *
 */
void ThrownStageObjectState::OnDraw()
{
}

