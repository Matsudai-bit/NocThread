/*****************************************************************//**
 * @file    PlayerFactory.h
 * @brief   敵の生成に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// ファクトリー関連
#include "Game/Common/Factory/FactoryBase.h"

// フレームワーク関連
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Enemy/Enemy.h"

// クラスの前方宣言 ===================================================
class Player;
class BuildingManager;
class PlayerCamera;
class CommonResources;
class CollisionManager;

namespace PlayerFactory
{

    struct PlayerDesk
    {
        const CommonResources&  commonResources;
        CollisionManager*       pCollisionManager;
        int                     tileNumber;
        const BuildingManager&  buildingManager;
        PlayerCamera*           pPlayerCamera;


        PlayerDesk(
            const CommonResources&  commonResources,
            CollisionManager*       pCollisionManager,
            const BuildingManager&  buildingManager,
            int                     tileNumber,
            PlayerCamera*           pPlayerCamera
        )
            : commonResources(commonResources)      
            , pCollisionManager(pCollisionManager)  
            , tileNumber(tileNumber)                
            , buildingManager(buildingManager)      
            , pPlayerCamera(pPlayerCamera)          

        {
        }
    };

	

	/**
	 * @brief 追跡敵
	 */
	class StagePlayer
		: public FactoryBase<Player, PlayerDesk>
	{
	public:
		// コンストラクタ
		StagePlayer() = default;

		// デストラクタ
		~StagePlayer() = default;

	protected:

		// 生成したオブジェクトを組み立てる
		void Assemble(Player* instance, const PlayerDesk& desc) override;
	};

	


}