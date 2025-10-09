/*****************************************************************//**
 * @file    ResultData.h
 * @brief   リザルトデータに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/09/16
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief リザルトデータ (シングルトンクラス)
 */
class ResultData final
{
// クラス定数の宣言 -------------------------------------------------
public:


// データメンバの宣言 -----------------------------------------------
private:

	static std::unique_ptr<ResultData> s_instance;	///< インスタンスオブジェクト
	
	float	m_gamePlayingTime;	///< プレイ時間(s)
	int		m_destroyedNum;		///< 撃破数
	bool	m_isSuccess;		///< 成功

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ
private:
	// コンストラクタ
	ResultData();

// デストラクタ
public:
	// デストラクタ
	~ResultData();

// 取得関連
public:

	// インスタンスの取得
	static ResultData* GetInstance();
	// 成功かどうか
	bool IsSuccess() const;
	// ゲーム時間の取得
	float GetGamePlayingTime() const;
	// 撃破数の取得
	int GetDestroyedNum() const;

// 操作
public:

	// リセット
	void Reset();
	// リザルトデータの設定
	void SetResultData(const float& gameTime, bool success );
	// 撃破数の加算
	void UpperDestroyedNum();
};
