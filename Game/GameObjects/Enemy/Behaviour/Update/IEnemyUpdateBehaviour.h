/*****************************************************************//**
 * @file    IEnemyUpdateBehaviour.h
 * @brief   敵の更新処理インターフェースに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================
class Enemy;			// 敵
class CommonResources;	// 共通リソース
class Player;			// プレイヤー
	
// クラスの定義 ===============================================================
/**
 * @brief 敵の更新挙動インターフェース
 */
class IEnemyUpdateBehaviour
{
public:

	virtual ~IEnemyUpdateBehaviour() = default;

public:
	// 更新処理
	virtual void Update(Enemy* pEnemy, float elapsedTime,const CommonResources* pCommonResources) = 0;

};
