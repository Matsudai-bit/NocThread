/*****************************************************************//**
 * @file    DxTween.h
 * @brief   DirectX環境における数値補間（Tween）アニメーション制御クラスに関するヘッダーファイル
 *			UnityのDoTweenを参考
 *
 * @author  松下大暉
 * @date    2026/01/29
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <functional>

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief アニメーション数値補間制御クラス
 */
template <typename T>
class DxTween
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	bool m_loop; ///< ループするかどうか

	T m_startValue;		///< 開始
	T m_endValue;		///< 終了
	T m_currentValue;	//< 現在
	float m_duration;	///< 目標時間
	float m_delay;		///< 遅延時間

	float m_rate;		///< 比率（進み具合）

	double m_elapsedTime; ///< 経過時間

	std::function<float(float)> m_calculateEasingValue;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	DxTween();

	// デストラクタ
	~DxTween() = default;


// 操作
public:
	// 初期化処理
	void Initialize(const T& startValue, const T& endValue, float duration);

	// 更新処理
	void Update(float deltaTime);

	// イージングの設定
	void SetEase(const std::function<float(float)>& ease) { m_calculateEasingValue = ease; }
	// 遅延時間の設定
	void SetDelay(float delay) { m_delay = delay; }

	// ループするかどうか
	void SetLoop(bool loop) { m_loop = loop; }

// 取得/設定
public:

	// 値の取得
	T GetValue() const { return m_currentValue; }

	bool IsEnd() const { return m_rate >= 1.0f; }

// 内部実装
private:


};

/**
 * @brief コンストラクタ
 */
template<typename T>
inline DxTween<T>::DxTween()
	: m_startValue	{}
	, m_endValue	{}
	, m_currentValue{}
	, m_elapsedTime	{}
	, m_duration	{}
	, m_delay		{}
	, m_loop		{ false }
	, m_rate		{}
{
	m_calculateEasingValue = [](float value) { return value; };
}

 
/**
 * @brief 初期化処理
 * 
 * @param[in] startValue　	開始の値
 * @param[in] endValue		終了の値
 * @param[in] duration		時間
 */
template<typename T>
inline void DxTween<T>::Initialize(const T& startValue, const T& endValue, float duration)
{
	m_startValue	= startValue;
	m_endValue		= endValue;
	m_duration		= duration;

	m_currentValue = T();

	m_loop = false;
}


/**
 * @brief 更新処理
 * 
 * @param[in] deltaTime フレーム間時間
 */
template<typename T>
inline void DxTween<T>::Update(float deltaTime)
{
	m_elapsedTime += static_cast<double>(deltaTime);

	m_rate = (m_elapsedTime - m_delay) / static_cast<float>(m_duration);
	m_rate = std::clamp(m_rate, 0.0f, 1.0f);

	if (m_loop && m_rate >= 0.9999f)
	{
		m_elapsedTime = m_delay;
	}

	float easeRate = m_calculateEasingValue(m_rate);
	m_currentValue = m_startValue + (m_endValue - m_startValue) * easeRate;
	

}
