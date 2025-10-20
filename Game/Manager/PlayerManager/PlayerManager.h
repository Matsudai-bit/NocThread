/*****************************************************************//**
 * @file    PlayerManager.h
 * @brief   プレイヤー管理に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/20
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/Input/PlayerInput/PlayerInput.h"
#include "Game/Common/Shadow/CircularShadow/CircularShadow.h"

// クラスの前方宣言 ===================================================
class CommonResources;	// 共通リソース
class CollisionManager; // 衝突管理
class PlayerCamera;		// カメラ
class Player;			// プレイヤー
class PlayerController;	// プレイヤーコントローラ
// クラスの定義 ===============================================================
/**
 * @brief プレイヤー管理
 */
class PlayerManager
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
		
	std::unique_ptr<Player>	m_player;						///< プレイヤー
	std::unique_ptr<PlayerInput> m_playerInput;				///< プレイヤー入力
	std::unique_ptr<PlayerController> m_playerController;	///< プレイヤーコントローラ

	std::unique_ptr< CircularShadow> m_playerShadow; ///< プレイヤーの影

	CommonResources* m_pCommonResources; ///< 共通リソース

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PlayerManager();

	// デストラクタ
	~PlayerManager();


// 操作
public:
	// 初期化処理
	void Initialize(CommonResources* pCommonResources, CollisionManager* pCollisionManager, const PlayerCamera* pPlayerCamera);

	// 更新処理
	void Update(float deltaTime, const DirectX::SimpleMath::Matrix& proj);

	// 描画処理
	void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);

	// 終了処理
	void Finalize();


// 取得/設定
public:

	// プレイヤーの取得
	const Player* GetPlayer() const;

// 内部実装
private:


};
