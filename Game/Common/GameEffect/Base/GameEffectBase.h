/*****************************************************************//**
 * @file    GameEffectBase.h
 * @brief   ゲームエフェクト基底に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/11
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/GameEffect/Base/IGameEffect.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief ゲームエフェクト基底
 */
class GameEffectBase
	: public IGameEffect
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:



// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	GameEffectBase();

	// デストラクタ
	virtual ~GameEffectBase();


// 操作
public:

	// 再生
	virtual void Play() = 0;
	// 更新処理
	virtual void Update(float deltaTime) = 0;
	// 描画処理
	virtual void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) = 0;

	// 再生してるかどうか
	virtual bool IsPlaying() const = 0;


// 取得/設定
public:

// 取得/設定
public:


// 内部実装
private:


};
