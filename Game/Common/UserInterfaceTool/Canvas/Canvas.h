/*****************************************************************//**
 * @file    Canvas.h
 * @brief   キャンバスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/04/29
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// フレームワーク関連
#include "Game/Common/Framework/TaskManager/TaskManager.h"

// クラスの前方宣言 ===================================================
class ISprite2D;	// スプライト
class Camera;		// カメラ


// クラスの定義 ===============================================================
/**
 * @brief キャンバス
 */
class Canvas
	: public Task
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;	///< スプライトバッチ

	std::vector<ISprite2D*> m_sprites;						///< スプライト群

	const DirectX::CommonStates* m_pCommonStates;			///< 共通状態


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Canvas(ID3D11DeviceContext* context, const DirectX::CommonStates* pCommonStates);

	// デストラクタ
	~Canvas();


// 操作
public:


	// 描画処理
	void DrawTask(const Camera& camera) override;
	void DrawContents();

	// 終了処理
	void Finalize();

	// スプライトの追加
	void AddSprite(ISprite2D* addSprite);

	// 削除する
	void RemoveSprite(ISprite2D* removeSprite);


// 取得/設定
public:


// 内部実装
private:


};
