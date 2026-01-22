/*****************************************************************//**
 * @file    Wall.h
 * @brief   足場に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/01/26
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>

// ライブラリ関連
#include "Library/DirectXFramework/DeviceResources.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Common/GameObject.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/Collision.h"


// クラスの前方宣言 ===================================================
class CollisionManager;	// 衝突判定管理



// クラスの定義 ==============================================================
/**
 * @brief 壁
 */
class Wall :
	public GameObject
{
// 列挙型の定義
public:

	// 設置方向
	enum class PlaceDirection
	{
		UP,		// 上
		RIGHT,	// 右
		DOWN,	// 下
		LEFT	// 左
	};

// 構造体の定義
	/**
 * @brief 壁データ
 */
	struct WallData
	{
		bool facadeFace = true;
		bool backFace = true;
		bool rightFace = true;
		bool leftFace = true;

		bool generation = false;
	};

// クラス定数の宣言 -------------------------------------------------
public:

	// サイズ
	// 横幅(X)
	static constexpr float WIDTH = 20.0f;
	// 縦幅(Y)
	static constexpr float HEIGHT = 10.0f;
	// 奥行(Z)
	static constexpr float DEPH = 3.0f;

	// 座標からのの距離
	static constexpr float DISTANCE = 0.5f;
	// 定数
	static constexpr int VERTEX_NUM = 6 * 6 ; // 頂点数
	static constexpr int SIDE_NUM = 5;

	// 四角形の頂点データ(位置)
	 DirectX::VertexPositionNormalTexture m_parentVertexes[VERTEX_NUM] =
	 {
	   // 前面
	   { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 0.0f,1.0f), DirectX::SimpleMath::Vector2(1.0f, 1.0f)},	// 0
	   { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 0.0f,1.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f)},	// 1
	   { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 0.0f,1.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f)},	// 2
	   { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 0.0f,1.0f), DirectX::SimpleMath::Vector2(1.0f, 0.0f)},  // 3

		// 後面
	   { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f), DirectX::SimpleMath::Vector2(1.0f, 1.0f)}, // 4
	   { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f)}, // 5
	   { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f)}, // 6
	   { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f), DirectX::SimpleMath::Vector2(1.0f, 0.0f)}, // 7

	   // 左面
	  { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 1.0f)},	// 8
	  { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f)},	// 9
	  { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f)},	// 10
	  { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 0.0f)},	// 11

	  // 右面
	 { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 1.0f)},	// 12
	 { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f)},	// 13
	 { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f)},	// 14
	 { DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 0.0f)},	// 15

	 // 上面
	{ DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 1.0f)},	// 16
	{ DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f)},	// 17
	{ DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f)},	// 18
	{ DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 0.0f)},	// 19

	// 下面
	{ DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f,-1.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 1.0f)},	// 20
	{ DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f,-1.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f)},	// 21
	{ DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f,-1.0f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f)},	// 22
	{ DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0.0f,-1.0f, 0.0f), DirectX::SimpleMath::Vector2(1.0f, 0.0f)},	// 23

	};

	// 四角形のインデックスデータ
	static constexpr uint16_t INDEXES[VERTEX_NUM] =
	{
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,


		20, 21, 22,
		22, 23, 20,
	};

// データメンバの宣言 -----------------------------------------------
private:


	// 頂点データ
	DirectX::VertexPositionNormalTexture m_vertexes[VERTEX_NUM];


	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>> m_primitiveBatch;	///< プリミティブバッチ

	std::vector<std::unique_ptr<Box2D>> m_collider;							///< 四角形コライダー 

	DirectX::SimpleMath::Vector3 m_rotation;					///< 回転

	
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			m_inputLayout;	///< 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_texture;	///< テクスチャハンドル

	std::unique_ptr<DirectX::BasicEffect> m_effect;					///< ベーシックエフェクト


	// テスト
	float m_height;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Wall();

	// デストラクタ
	~Wall();


// 操作
public:
	// 初期化処理
	void Initialize(
		const DirectX::SimpleMath::Vector3& pos,
		const PlaceDirection& placeDirection,
		const WallData& wallData,
		CommonResources* pCommonResources,
		CollisionManager*	pCollisionManager,
		std::string fileName,
		float height);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(DirectX::SimpleMath::Matrix* pView, DirectX::SimpleMath::Matrix* pProj);

	// 終了処理
	void Finalize();

	// 回転する
	void Rotate(DirectX::SimpleMath::Vector3 rotation);

	// 衝突処理
	void OnCollision(const CollisionInfo& info) override;


	// 取得/設定
public:

	// 活動しているかどうか
	bool IsActive() const override { return true; }

	// ゲームオブジェクトタグの取得
	GameObjectTag GetTag() const override { return GameObjectTag::WALL; }



// 内部実装
private:

	// 置く方向に設置する
	void PutPlaceDirection(const PlaceDirection& placeDirection);


};
