/*****************************************************************//**
 * @file    ColliderDrawer.h
 * @brief   XXXXに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    ××××/××/××
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "../Collision.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief ＸＸＸＸ
 */
class ColliderDrawer
{
// クラス定数の宣言 -------------------------------------------------
public:


	static constexpr int VERTEX_NUM = 3;
	// 三角形のインデックスデータ
	static constexpr uint16_t INDEXES[VERTEX_NUM] =
	{
		0, 1, 2,
	};

// データメンバの宣言 -----------------------------------------------
private:

	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>	m_primitiveBatch;			///< プリミティブバッチ
	DirectX::BasicEffect									m_basicEffect;				///< エフェクト
	Microsoft::WRL::ComPtr<ID3D11InputLayout>				m_inputLayout;				///< 入力
	DX::DeviceResources* m_pDeviceResources;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ColliderDrawer(DX::DeviceResources* pDeviceResources);

	// デストラクタ
	~ColliderDrawer();

	void DrawTriangle(const Triangle& triangle, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);
	void DrawBox2D(const Box2D& box2D, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);
	void DrawAABB(const AABB& aabb, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);
	void DrawSphere(const Sphere& sphere, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);

// 操作
public:


// 取得/設定
public:


// 内部実装
private:


};
