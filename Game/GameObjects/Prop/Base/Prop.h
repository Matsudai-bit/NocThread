/*****************************************************************//**
 * @file    Prop.h
 * @brief   ステージ上に配置される背景オブジェクトの基底クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/24
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/GameObjects/Prop/Base/IProp.h"

// クラスの前方宣言 ===================================================
class CollisionManager; // 衝突管理
class CommonResources; // 共通リソース

// クラスの定義 ===============================================================
/**
 * @brief ステージ上に配置される背景オブジェクトの基底クラス
 */
class Prop 
	: public IProp
	, public GameObject
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Prop();

	// デストラクタ
	virtual ~Prop();


// 操作
public:
	// 初期化処理
	void Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager)override = 0;

	// 更新処理
	void Update(float elapsedTime) override= 0; 

	// 描画処理
	void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)override = 0;

	// 終了処理
	void Finalize()override = 0;


	// 衝突処理
	void OnCollision(GameObject* pHitObject, ICollider* pHitCollider) override = 0;


	// 取得/設定
public:

	// 活動しているかどうか
	virtual bool IsActive() const = 0;

	// ゲームオブジェクトタグの取得
	virtual GameObjectTag GetTag() const = 0;

// 内部実装
private:


};
