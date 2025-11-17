/*****************************************************************//**
 * @file    Floor.h
 * @brief   足場に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/01/26
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include "Game/Common/DeviceResources.h"
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/Common/Collision/Collision.h"
// クラスの前方宣言 ===================================================
class Wall;				// 壁
class CollisionManager;	// 衝突判定管理
class CommonResources;	// 共通リソース

// クラスの定義 ===============================================================
/**
 * @brief 足場
 */
class Floor:
	public GameObject
{
// クラス定数の宣言 -------------------------------------------------
public:

	// 定数
	static constexpr int VERTEX_NUM = 6 ; // 頂点数

	//static constexpr float WIDTH = 10.0f;
	static constexpr float WIDTH = 1000.0f;

	// 四角形の頂点データ(位置)
	 DirectX::VertexPositionNormalTexture m_parentVertexes[VERTEX_NUM] =
	 {
	   // 上面
	   { DirectX::SimpleMath::Vector3(5.0f, 0.0f,5.0f), DirectX::SimpleMath::Vector3(0.0f, 1.0f,  0.0f) ,     DirectX::SimpleMath::Vector2(WIDTH / 10.0f, WIDTH / 10.0f)},	// 0
	   { DirectX::SimpleMath::Vector3(-5.0f,  0.0f, 5.0f), DirectX::SimpleMath::Vector3(0.0f, 1.0f,  0.0f) ,  DirectX::SimpleMath::Vector2(0.0f, WIDTH / 10.0f)},	// 1
	   { DirectX::SimpleMath::Vector3(-5.0f,  0.0f, -5.0f), DirectX::SimpleMath::Vector3(0.0f, 1.0f,  0.0f),  DirectX::SimpleMath::Vector2(0.0f, 0.0f)},	// 2
	   { DirectX::SimpleMath::Vector3(5.0f, 0.0f, -5.0f), DirectX::SimpleMath::Vector3(0.0f, 1.0f,  0.0f)  ,  DirectX::SimpleMath::Vector2(WIDTH / 10.0f, 0.0f)},   // 3
	};

	// 四角形のインデックスデータ
	static constexpr uint16_t INDEXES[VERTEX_NUM] =
	{
		0, 1, 2,
		2, 3, 0,
	};

// データメンバの宣言 -----------------------------------------------
private:

	DirectX::VertexPositionNormalTexture m_vertexes[VERTEX_NUM];


	DirectX::SimpleMath::Vector4 m_vec4;	///< 平面情報

	std::unique_ptr<DirectX::BasicEffect> m_effect;	///< ベーシックエフェクト
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>> m_primitiveBatch;	///< プリミティブバッチ

	
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;///< 入力レイアウト

	std::unique_ptr<AABB> m_collider;				///<　矩形コライダー 

	DirectX::SimpleMath::Vector3 m_rotation;				///< 回転

	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;	///< テクスチャハンドル


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Floor();

	// デストラクタ
	~Floor();


// 操作
public:
	// 初期化処理
	void Initialize(const DirectX::SimpleMath::Vector3& pos, const CommonResources* pCommonResources, CollisionManager* pCollisionManager);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);

	// 終了処理
	void Finalize();

	// 回転する
	void Rotate(const DirectX::SimpleMath::Vector3& rotation);

	void PreCollision() override;
	// 衝突処理
	void OnCollision(GameObject* pHitObject, ICollider* pHitCollider) override;


	// 取得/設定
public:

	// 活動しているかどうか
	bool IsActive() const override { return true; }

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::FLOOR; }

	// 回転の取得
	DirectX::SimpleMath::Vector3 GetRotate();
// 内部実装
private:


};
