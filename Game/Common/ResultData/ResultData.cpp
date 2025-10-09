/*****************************************************************//**
 * @file    ResultData.h
 * @brief   リザルトデータに関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/09/16
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ResultData.h"

// 静的変数の初期化
std::unique_ptr<ResultData> ResultData::s_instance = nullptr;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ResultData::ResultData()
	: m_isSuccess	{ false }
	, m_gamePlayingTime{}
	, m_destroyedNum{}
{
	Reset();
}



/**
 * @brief デストラクタ
 */
ResultData::~ResultData()
{
}

/**
 * @brief インスタンスの取得
 *
 * @return インスタンス
 */
ResultData* ResultData::GetInstance()
{
	if (s_instance.get() == nullptr)
	{
		s_instance.reset(new ResultData());
	}

	return s_instance.get();
}

/**
 * @brief 成功かどうか
 * 
 * @returns true 成功
 * @returns false 失敗
 */
bool ResultData::IsSuccess() const
{
	return m_isSuccess;
}

/**
 * @brief ゲームプレイ時間の取得
 * 
 * @return 時間（s）
 */
float ResultData::GetGamePlayingTime() const
{
	return m_gamePlayingTime;
}

/**
 * @brief 撃破数の取得
 * 
 * @return 撃破数
 */
int ResultData::GetDestroyedNum() const
{
	return m_destroyedNum;
}

/**
 * @brief リセット
 */
void ResultData::Reset()
{
	// 各種リセット
	m_isSuccess			= false;
	m_gamePlayingTime	= 0.0f;
	m_destroyedNum		= 0;
}

/**
 * @brief リザルトデータの設定
 *
 * @param[in] gameTime　ゲーム時間
 * @param[in] success   成功
 */
void ResultData::SetResultData(const float& gameTime, bool success)
{
	m_gamePlayingTime	= gameTime;
	m_isSuccess			= success;
}

/**
 * @brief 撃破数の加算
 * 
 */
void ResultData::UpperDestroyedNum()
{
	m_destroyedNum++;
}


