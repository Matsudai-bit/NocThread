/*****************************************************************//**
 * @file    IConstraint.h
 * @brief   制約のインターフェースに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/08
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================


// クラスの前方宣言 ===================================================


/**
 * @brief 制約
 */
struct ConstraintParam
{
	// λ = -α~^{-1} C(x) でCに対応
	float λ;	///< ラグランジュ乗数 (制約反復で使う)
	float α;	///< 柔らかさ	(柔軟性、0に近いほど剛性が高い)

	float startDistance;	///< 初期距離（制約の自然長）: 維持すべき距離
};

// クラスの定義 ===============================================================
/**
 * @brief 制約のインターフェース
 */
class IConstraint
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:



// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	IConstraint() = default;

	// デストラクタ
	virtual ~IConstraint() = default;


// 操作
public:
    // 制約の現在の違反度を計算する
    virtual float EvaluateConstraint() const = 0;
    // ラグランジュ乗数の更新量 Δλ を計算する
    virtual float ComputeLambdaCorrection(float dt, float C) const = 0;
    // 位置の補正ベクトル Δx を計算する
    virtual DirectX::SimpleMath::Vector3 ComputeDeltaX(float deltaLambda) const = 0;
    // 関連するパーティクルへのポインタを取得 (必要に応じて)
    // virtual std::vector<SimParticle*> GetParticles() = 0;

    // ConstraintParam に相当する情報を取得・設定するメソッドも必要になるかもしれません
    virtual void ResetConstraintParam(float flexibility) = 0;
    virtual const ConstraintParam& GetConstraintParam() const = 0;
    virtual void SetConstraintParam(const ConstraintParam& param) = 0;

    // 位置補正を適用する
    virtual void ApplyPositionCorrection(float deltaLambda) = 0;

// 取得/設定
public:


};
