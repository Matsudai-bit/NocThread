/**
 * @file  SpriteAnimator.cpp
 *
 * @brief  スプライトのアニメーションに関するソースファイル
 *
 * @author 松下大暉
 *
 * @date   2025/08/03
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SpriteAnimator.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief デフォルトコンストラクタ
 *
 * @param[in] なし
 */
SpriteAnimator::SpriteAnimator()
	: m_sprite{ nullptr }
	, m_animationClip{ }
	, m_elapsedTime{ 0 }
	, m_frameIndex{ 0 }
	, m_isPlaying{ false }
{
}



/**
 * @brief デストラクタ
 */
SpriteAnimator::~SpriteAnimator()
{
	
}



/**
 * @brief 初期化処理
 *
 * @param[in] sprite        アニメーション対象のプライト
 * @param[in] animationClip アニメーションのフレームシーケンス
 *
 * @return なし
 */
void SpriteAnimator::Initialize(Sprite* sprite, AnimationClip& animationClip)
{
	m_sprite = sprite;
	m_animationClip = animationClip;
	m_elapsedTime = 0.0f;
	m_frameIndex = 0;
	m_isPlaying = false;
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void SpriteAnimator::Update(float elapsedTime)
{
	// 再生時の処理
	if (m_isPlaying)
	{
		// アニメーション終了時の処理
		if (m_elapsedTime >= m_animationClip.duration)
		{
			if (m_animationClip.isLoop)
			{
				m_elapsedTime = 0.0f;
				m_sprite->SetRegion(m_animationClip.startRegion);

			}
			else
			{
				m_isPlaying = false;


				return;
			}
		}

		// 時間経過による描画フレームの設定
		int lastFrame = m_animationClip.numFrames;
		m_frameIndex = static_cast<int>(m_animationClip.numFrames *  m_elapsedTime / m_animationClip.duration);

		if (m_frameIndex != lastFrame)
		{
			int row = m_frameIndex / m_animationClip.numColumns;
			int column = m_frameIndex % m_animationClip.numColumns;

			Sprite::Region sourceRegion = m_animationClip.startRegion;
			sourceRegion.topLeftX += sourceRegion.width * column;
			sourceRegion.topLeftY += sourceRegion.height * row;

			m_sprite->SetRegion(sourceRegion);
		}

		m_elapsedTime += elapsedTime;
	}
}



/**
 * @brief アニメーションの再生
 *
 * @param[in] なし
 *
 * @return なし
 */
void SpriteAnimator::Play()
{
	// アニメーション開始の場合
	if (m_elapsedTime == 0)
	{
		m_sprite->SetRegion(m_animationClip.startRegion);
	}

	// 再生中に設定
	m_isPlaying = true;
}



/**
 * @brief アニメーションの一時停止
 *
 * @param[in] なし
 *
 * @return なし
 */
void SpriteAnimator::Pause()
{
	// 停止中に設定
	m_isPlaying = false;
}



/**
 * @brief アニメーションの停止
 *
 * @param[in] なし
 *
 * @return なし
 */
void SpriteAnimator::Stop()
{
	// 停止中に設定
	m_isPlaying = false;

	// 経過時間のリセット
	m_elapsedTime = 0;
}



/**
 * @brief アニメーションが再生中かどうかの確認
 *
 * @param[in] なし
 *
 * @retval true  再生中
 * @retval false 停止中(一時停止を含む)
 */
bool SpriteAnimator::IsPlaying() const
{
	return m_isPlaying;
}



/**
 * @brief アニメーションが終了しているかどうかの確認
 *
 * @param[in] なし
 *
 * @retval true  終了
 * @retval false 未終了
 */
bool SpriteAnimator::IsFinished() const
{
	if ((m_isPlaying == false) && (m_elapsedTime == m_animationClip.duration))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief アニメーションの変更
 * 
 * @param[out]	 pAnimationClip		　アニメーションの変更するデータ
 * @param[in]	 newAnimationClip　アニメーションの変更データ
 */
void SpriteAnimator::ChangeAnimation( const AnimationClip& newAnimationClip)
{

	// アニメーションの変更
	AnimationClip changeAnimationClip = newAnimationClip;

	// 誤差の調節
	changeAnimationClip.startRegion.topLeftY +=1;

	// アニメーションの変更がない場合

	if (IsPlaying())
	{
		if (changeAnimationClip.startRegion.topLeftX == m_animationClip.startRegion.topLeftX &&
			changeAnimationClip.startRegion.topLeftY == m_animationClip.startRegion.topLeftY &&
			changeAnimationClip.numColumns			 == m_animationClip.numColumns &&
			changeAnimationClip.numFrames			 == m_animationClip.numFrames &&
			changeAnimationClip.duration			 == m_animationClip.duration &&
			changeAnimationClip.isLoop				 == m_animationClip.isLoop)
			return;
	}

	// アニメーションの変更
	m_animationClip = changeAnimationClip;
	
	m_elapsedTime = 0;
	m_frameIndex = (m_animationClip.numFrames == 1)? 1 : 0;


}

/**
 * @brief スプライトの設定
 * 
 * @param[in] pSprite
 */
void SpriteAnimator::SetSprite(Sprite* pSprite)
{
	m_sprite = pSprite;
}

/**
 * @brief アニメーションクリップの設定
 * 
 * @param[in] animationClip
 */
void SpriteAnimator::SetAnimationClip(AnimationClip animationClip)
{
	m_animationClip = animationClip;
}
