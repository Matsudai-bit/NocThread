/*****************************************************************//**
 * @file    IProp.h
 * @brief   背景オブジェクトのインターフェースに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/08/25
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================
class CommonResources;
class CollisionManager;

// クラスの定義 ===============================================================
/**
 * @brief 背景オブジェクトのインターフェース
 */
class IProp
{

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	IProp() = default;

	// デストラクタ
	virtual ~IProp() = default;


// 操作
public:
	// 初期化処理
	virtual void Initialize(const CommonResources* pCommonResources, CollisionManager* pCollisionManager) = 0;

	// 更新処理
	virtual void Update(float deltaTime) = 0;

	// 描画処理
	virtual void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) = 0;

	// 終了処理
	virtual void Finalize() = 0;



};
