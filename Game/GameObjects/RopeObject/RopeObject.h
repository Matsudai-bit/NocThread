/*****************************************************************//**
 * @file    RopeObject.h
 * @brief   ロープオブジェクトに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/05/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <vector>
#include <memory>
#include "Game/GameObjects/RopeObject/ParticleObject/ParticleObject.h"

// クラスの前方宣言 ===================================================
class CommonResources;	// 共通リソース
class Camera;			// カメラ

// クラスの定義 ===============================================================
/**
 * @brief ロープオブジェクト
 */
class RopeObject
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	std::vector<ParticleObject*> m_particles;	///< パーティクルデータ

	std::unique_ptr <DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;

	std::unique_ptr<DirectX::BasicEffect> m_effect;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>								m_inputLayout;

	const CommonResources* m_pCommonResources;	///< 共通リソース
	


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	RopeObject();

	// デストラクタ
	~RopeObject();


// 操作
public:
	// 初期化処理
	void Initialize(const CommonResources* pCommonResouces);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(const Camera& camera);

	// 終了処理
	void Finalize();

	// パーティクル情報のリセット
	void ResetParticle();

	// パーティクルの追加
	void AddParticle(ParticleObject* pParticle);

	// パーティクルデータの取得
	std::vector< ParticleObject*>* GetParticles() ;


// 取得/設定
public:


// 内部実装
private:


};
