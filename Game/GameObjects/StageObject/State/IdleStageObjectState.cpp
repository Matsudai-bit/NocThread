/*****************************************************************//**
 * @file    IdleStageObjectState.h
 * @brief   ステージオブジェクトの待機状態に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/06/25
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "IdleStageObjectState.h"

#include "Game/GameObjects/StageObject/StageObject.h"



// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
IdleStageObjectState::IdleStageObjectState()
{

}



/**
 * @brief デストラクタ
 */
IdleStageObjectState::~IdleStageObjectState()
{

}



void IdleStageObjectState::OnStartState()
{

	// イベントの追加
	GetOwner()->AddEventListener<ThrowHitEventData>(GameObjectEventType::THROW_HIT, 
		[this](const ThrowHitEventData data) 
								{
			GetOwner()->SetActive(false); 
		}
	);
}

void IdleStageObjectState::OnExitState()
{
	GetOwner()->RemoveAllEventListener();
}

/**

/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime　経過時間
 */
void IdleStageObjectState::OnUpdate(float elapsedTime)
{
	GetOwner()->ApplyPhysic(elapsedTime);
	GetOwner()->Move(elapsedTime);

}

/**
 * @brief 描画処理
 *
 */
void IdleStageObjectState::OnDraw()
{
}

