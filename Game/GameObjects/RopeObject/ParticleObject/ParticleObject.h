/*****************************************************************//**
 * @file    ParticleObject.h
 * @brief   パーティクルオブジェクトに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/05/14
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief パーティクルオブジェクト
 */
class ParticleObject 
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	float m_mass;								///< 質量
	DirectX::SimpleMath::Vector3 m_position;	///< 座標
	DirectX::SimpleMath::Vector3 m_velocity;	///< 速度

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ParticleObject();

	// デストラクタ
	~ParticleObject();


// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();


// 取得/設定
public:

	// 座標を設定する
	void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }

	// 速度を設定する
	void SetVelocity(const DirectX::SimpleMath::Vector3& velocity) { m_velocity = velocity; }

	// 質量の設定
	void SetMass(const float settingMass) { m_mass = settingMass; }

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 速度を取得する
	DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }

	// 質量の取得
	float GetMass() const { return m_mass; }

// 内部実装
private:


};
