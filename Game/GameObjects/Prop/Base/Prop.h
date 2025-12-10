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
class CollisionInfo;   // 衝突情報

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
	int m_tileNumber; // タイル番号
	

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
	void Update(float deltaTime) override= 0; 

	// 描画処理
	void Draw(const Camera& camera)override = 0;

	// 終了処理
	void Finalize()override = 0;


	// 衝突処理
	void OnCollision(const CollisionInfo& info) override = 0;


	// 取得/設定
public:

	// 活動しているかどうか
	virtual bool IsActive() const = 0;

	// ゲームオブジェクトタグの取得
	virtual GameObjectTag GetTag() const = 0;

	// タイル番号のセッター・ゲッター
	void SetTileNumber(const int& tileNumber)	{ m_tileNumber = tileNumber; }
	int GetTileNumber() const					{ return m_tileNumber; }

// 内部実装
private:


};
