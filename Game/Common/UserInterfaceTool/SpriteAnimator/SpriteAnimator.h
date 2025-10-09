/**
 * @file   SpriteAnimator.h
 *
 * @brief  スプライトのアニメーションに関するヘッダファイル
 *
 * @author 松下大暉
 *
 * @date   2025/08/03
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "Game/Common/UserInterfaceTool/Sprite/Sprite.h"




// クラスの定義 ===============================================================
/**
 * @brief スプライトをアニメーションさせるためのクラス
 */
class SpriteAnimator
{
	// 型の定義 --------------------------------------------------------------
public:
	/**
	 * @brief アニメーションのフレームシーケンス
	 */
	struct AnimationClip
	{
		Sprite::Region startRegion;      ///< 開始フレームの領域[pixel]
		int            numFrames;        ///< 総フレーム数
		int            numColumns;       ///< 列数(１行当たりのフレーム数)
		float            duration;         ///< 再生時間[s]
		bool           isLoop;           ///< ループさせるかどうか[true:ループあり false:ループなし]
	};



	// データメンバの宣言 -----------------------------------------------
private:
	Sprite* m_sprite;           ///< アニメーション対象のスプライト
	 AnimationClip m_animationClip;    ///< 指定のアニメーションクリップ
	float                  m_elapsedTime;      ///< 経過時間[frames]
	int                  m_frameIndex;       ///< 現在のフレームのインデックス
	bool                 m_isPlaying;        ///< 再生中かどうか



	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SpriteAnimator();

	// デストラクタ
	~SpriteAnimator();


	// 操作
public:
	// 初期化処理
	void Initialize(Sprite* sprite, AnimationClip& animationClip);

	// 更新処理
	void Update(float elapsedTime);

	// アニメーションの再生
	void Play();

	// アニメーションの一時停止
	void Pause();

	// アニメーションの停止
	void Stop();

	// アニメーションが再生中かどうかの確認
	bool IsPlaying() const;

	// アニメーションが終了しているかどうかの確認
	bool IsFinished() const;

	// アニメーションの変更
	void ChangeAnimation( const AnimationClip& newAnimationClip);

	// 取得・設定
public:

	// スプライトの設定
	void SetSprite(Sprite* pSprite);

	// アニメーションクリップの設定
	void SetAnimationClip(AnimationClip animationClip);

};