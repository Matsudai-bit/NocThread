/*****************************************************************//**
 * @file    Collision.h
 * @brief   衝突判定に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/01/27
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once


#include <memory>


// ヘッダファイルの読み込み ===================================================

// ライブラリ関連
#include "Library/MyLib/MathUtils/MathUtils.h"
#include "Library/DirectXFramework/DeviceResources.h"

// ユーティリティ関連
#include "Game/Common/Utillities/Collision/ICollider.h"






// クラスの前方宣言 ===================================================
class Sphere;
class Plane;
class Triangle;
class Segment;
class Box2D;
class AABB;
class Cylinder;
class Capsule;


// クラスの定義 ===============================================================
/**
 * @brief 球
 */
class Sphere :
	public ICollider
{
// データメンバの宣言 -----------------------------------------------
private:

	DirectX::SimpleMath::Vector3 m_pos;	///< 座標
	DirectX::SimpleMath::Vector3 m_rotate;	///< 回転角
	float m_radius;						///< 半径


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Sphere(const DirectX::SimpleMath::Vector3& pos, const float& radius);
	Sphere();

	// デストラクタ
	~Sphere() = default;


// 操作
public:

	// 座標の設定
	void Transform(DirectX::SimpleMath::Vector3 translation);

	// 回転
	void Rotate(DirectX::SimpleMath::Vector3 rotation);

	// ワールド行列の取得
	DirectX::SimpleMath::Matrix GetWorldMatrix() const;

// 衝突判定
public:

	// 平面との衝突判定
	bool CheckHit(const Plane& plane) const;
	// 線分との衝突判定
	bool CheckHit(const Segment& segment) const;
	// 三角形との衝突判定
	bool CheckHit(const Triangle& triangle) const;
	// 球との衝突判定
	bool CheckHit(const Sphere& sphere) const;
	// 四角形との衝突判定
	bool CheckHit(const Box2D& box) const;
	// AABBとの衝突判定
	bool CheckHit(const AABB& aabb) const;
	// カプセルとの衝突判定
	bool CheckHit(const Capsule& capsule) const;


// 取得/設定
public:
	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const;


	// クローンの取得
	std::unique_ptr<ICollider> GetClone() const override;

	// 半径の取得
	float GetRadius() const;
	// 半径の設定
	void SetRadius(const float& radius);

	// コライダーの種類の取得
	ColliderType GetColliderType() const override;
};


/**
 * @brief 平面
 */
class Plane :
	public ICollider
{
// データメンバの宣言 -----------------------------------------------
private:

	DirectX::SimpleMath::Vector4 m_vec4;	///< 平面情報


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:

	// コンストラクタ

	Plane();
	
	Plane(DirectX::SimpleMath::Vector3 normal, const DirectX::SimpleMath::Vector3& point);

	Plane(DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB, DirectX::SimpleMath::Vector3 pointC);
	Plane(const DirectX::SimpleMath::Vector4& vec);

	// デストラクタ
	~Plane() = default;

// 衝突判定
public:

	// 球との衝突判定
	bool CheckHit(const Sphere& sphere) const;

	// 線分との衝突
	bool CheckHit(const Segment& segment) const;

// 取得/設定
public:


	// クローンの取得
	std::unique_ptr<ICollider> GetClone() const override;

	// 平面情報の取得
	DirectX::SimpleMath::Vector4 GetVector4() const;

	// 平面との距離の算出
	float CalcLength(DirectX::SimpleMath::Vector3 pos) const;

	// 平面の設定
	void Set(DirectX::SimpleMath::Vector3 normal, const DirectX::SimpleMath::Vector3& point);
	void Set(DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB, DirectX::SimpleMath::Vector3 pointC);
	void Set(const DirectX::SimpleMath::Vector4& vec);


	// コライダーの種類の取得
	ColliderType GetColliderType() const override;

};

/**
 * @brief 三角形
 */
class Triangle :
	public ICollider
{
// 定数
	static constexpr int VERTEX_NUM = 3;

	static constexpr DirectX::XMFLOAT4 COLOR = DirectX::XMFLOAT4(255.0f, 0.0f, 0.0f, 255.0f);

	// 四角形のインデックスデータ
	static constexpr uint16_t INDEXES[VERTEX_NUM] =
	{
		0, 1, 2,
	};
// データメンバの宣言
private:
	std::vector<DirectX::SimpleMath::Vector3>	m_pos;			///< 三点座標
	std::vector<DirectX::SimpleMath::Vector3>	m_parentPos;	///< 親となる三点座標
	Plane										m_plane;	///< 平面


// コンストラクタ
public:

	// 三頂点を指定して生成
	Triangle(
		const DirectX::SimpleMath::Vector3& posA,
		const DirectX::SimpleMath::Vector3& posB,
		const DirectX::SimpleMath::Vector3& posC);
	


// 操作
public:

	// 任意の点が三角形内に存在するかどうか
	bool CheckInner(const DirectX::SimpleMath::Vector3& pos) const;

	// 三角形のエッジ上で指定点に最も近い点を求める
	DirectX::SimpleMath::Vector3 GetClosestPointVertex(const DirectX::SimpleMath::Vector3& target) const;

	// 三角形上のの指定点に最も近い点を求める
	static DirectX::SimpleMath::Vector3 CalcClosestPoint(const DirectX::SimpleMath::Vector3& point, const Triangle& triangle) ;


// 衝突判定
public:
	// 球との衝突判定
	bool CheckHit(const Sphere& sphere) const;

	// 線分との衝突判定
	bool CheckHit(const Segment& segment) const;

// 取得・設定
public :

	// クローンの取得
	std::unique_ptr<ICollider> GetClone() const override;

	// コライダーの種類の取得
	ColliderType GetColliderType() const override;
	
	// 三頂点座標の設定
	void Set(
		const DirectX::SimpleMath::Vector3& posA,
		const DirectX::SimpleMath::Vector3& posB,
		const DirectX::SimpleMath::Vector3& posC);

	// 座標の取得
	const std::vector<DirectX::SimpleMath::Vector3>& GetPosition() const;

	// 平面情報の取得
	const Plane& GetPlane() const;

	// 回転角を設定
	void Rotate (DirectX::SimpleMath::Vector3 rotate);

	// コンテキストの設定
	//void SetContext(ID3D11DeviceContext1* context);


};

/**
 * @brief 矩形
 */
class Box2D :
	public ICollider
{
	// 定数
public:

	static constexpr int TRIANGLE_NUM = 2;

	// データメンバの宣言
private:

	Triangle m_triangles[TRIANGLE_NUM];					///< 二つの三角形
	
	DirectX::SimpleMath::Vector3 m_rotate;	///< 回転

	// コンストラクタ ・　デストラクタ
public:


	// 頂点を指定して生成
	Box2D(
		const DirectX::SimpleMath::Vector3& posA,
		const DirectX::SimpleMath::Vector3& posB,
		const DirectX::SimpleMath::Vector3& posC,
		const DirectX::SimpleMath::Vector3& posD);

	~Box2D() = default;


	// 操作
public:

	// 任意の点が四角形内に存在するかどうか
	bool CheckInner(const DirectX::SimpleMath::Vector3& pos) const;

	// 衝突判定
public:

	// 球との衝突判定
	bool CheckHit(const Sphere& sphere) const;

	// 線分との衝突判定
	bool CheckHit(const Segment& segment) const;
	// 取得・設定
public:

	// クローンの取得
	std::unique_ptr<ICollider> GetClone() const override;

	// コライダーの種類の取得
	ColliderType GetColliderType() const override;

	// 頂点を設定
	void Set(const DirectX::SimpleMath::Vector3& posA,
			const DirectX::SimpleMath::Vector3& posB,
			const DirectX::SimpleMath::Vector3& posC,
			const DirectX::SimpleMath::Vector3& posD);

	// 三角形の取得
	const std::array<Triangle, TRIANGLE_NUM>& GetTriangle() const;

	// 平面情報の取得
	const Plane& GetPlane() const;

	// 回転角を設定
	void Rotate(DirectX::SimpleMath::Vector3 rotate);

};

/**
 * @brief 線分
 */
class Segment :
	public ICollider
{
// データメンバの宣言
private:
	DirectX::SimpleMath::Vector3 m_pos;			///< 始点座標
	DirectX::SimpleMath::Vector3 m_vec;			///< ベクトル

// コンストラクタ
public:

	// コンストラクタ
	Segment(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& vec , bool useEndPoint = false);
	Segment();

	// 線分上の target に最も近い点を求める	
	DirectX::SimpleMath::Vector3 ClosestPoint(const DirectX::SimpleMath::Vector3& target);


	// 衝突判定
public:
	// 球との衝突判定
	bool CheckHit(const Sphere& sphere) const;
	// 三角形との衝突判定
	bool CheckHit(const Triangle& triangle) const;
	// 四角形との衝突判定
	bool CheckHit(const Box2D& box) const;
	// 平面との衝突判定
	bool CheckHit(const Plane& plane) const;
	// AABBとの衝突反転
	bool CheckHit(const AABB& box) const;

	// 線分の再近点
	static DirectX::SimpleMath::Vector3 GetClosestPoint(const DirectX::SimpleMath::Vector3& point, const DirectX::SimpleMath::Vector3& segStart, const DirectX::SimpleMath::Vector3& segEnd);


	


// 設定・取得
public:

	// クローンの取得
	std::unique_ptr<ICollider> GetClone() const override;

	// コライダーの種類の取得
	ColliderType GetColliderType() const override;

	// 設定
	void Set(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& vec, bool useEndPoint = false);

	// 始点座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const;

	// 終点座標の取得
	DirectX::SimpleMath::Vector3 GetEndPosition() const;

	// ベクトルの取得
	DirectX::SimpleMath::Vector3 GetVector() const;


};

/**
 * @brief AABB
 */
class AABB
	: public ICollider
{
// データメンバの宣言
private:
	DirectX::SimpleMath::Vector3 m_center;			///< 始点座標
	DirectX::SimpleMath::Vector3 m_extend;		///< ベクトル

	// コンストラクタ
public:

	// コンストラクタ
	AABB(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& extend);

// 衝突判定
public:

	// 球との衝突判定
	bool CheckHit(const Sphere& sphere) const;
	// AABBとの衝突判定
	bool CheckHit(const AABB& box) const;
	// 線分との衝突判定
	bool CheckHit(const Segment& segment)const;
	// 筒との衝突判定
	bool CheckHit(const Cylinder& cylinder)const;
	// カプセルとの衝突判定
	bool CheckHit(const Capsule& capsule)const;

public:

	// クローンの取得
	std::unique_ptr<ICollider> GetClone() const override;
	// コライダーの種類の取得
	ColliderType GetColliderType() const override;

	// 中心座標の設定
	void SetCenter(const DirectX::SimpleMath::Vector3& center);
	// 拡張の設定
	void SetExtend(const DirectX::SimpleMath::Vector3& extend);

	// 中心座標の取得
	DirectX::SimpleMath::Vector3 GetCenter() const;
	
	// 拡張の取得
	DirectX::SimpleMath::Vector3 GetExtend() const;

	// 最近点の取得
	DirectX::SimpleMath::Vector3 CalcClosestPoint(DirectX::SimpleMath::Vector3 targetPosition) const;

	DirectX::SimpleMath::Vector4 GetCollisionNormal(const AABB& aabb, const Sphere& sphere)const;

	DirectX::SimpleMath::Vector3 CalcClosestPointOnSurface(const DirectX::SimpleMath::Vector3& point)const;

	// 内部に点が位置するかどうか
	bool IsPointInside(const DirectX::SimpleMath::Vector3& point) const;

};

/**
 * @brief 筒
 */
class Cylinder
	: public ICollider
{
// データメンバの宣言
private:
	float m_length;		///< 長さ
	DirectX::SimpleMath::Vector3 m_axis;		///< 軸(方向)
	float m_radius;	///< 半径
	DirectX::SimpleMath::Vector3 m_position;	///< 座標


public :
	// コンストラクタ
	Cylinder(
		const DirectX::SimpleMath::Vector3& axis, 
		const float& length,
		const DirectX::SimpleMath::Vector3& position, 
		const float& radius);

	Cylinder() ;

	// デストラクタ
	~Cylinder() = default;

// メンバ関数

// 衝突判定
public:

	// AABBとの衝突判定
	bool CheckHit(const AABB& aabb)const;


public:

	// クローンの取得
	std::unique_ptr<ICollider> GetClone() const override;

	// コライダーの種類の取得
	ColliderType GetColliderType() const override;

	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	// 軸の設定
	void SetAxis(const DirectX::SimpleMath::Vector3& axis);
	// 長さの設定
	void SetLength(const float& length);
	// 半径の設定
	void SetRadius(const float& radius);

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const;
	// 軸の取得
	DirectX::SimpleMath::Vector3 GetAxis() const;
	// 長さの取得
	float GetLength() const;
	// 半径の取得
	float GetRadius() const;

	void Draw(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);

	
};


/**
 * @brief カプセルコライダー
 */
class Capsule
	: public ICollider
{
	// データメンバの宣言
private:
	float							m_length;	///< 軸の長さ
	DirectX::SimpleMath::Vector3	m_axis;		///< 軸(方向)
	float							m_radius;	///< 半球の半径
	DirectX::SimpleMath::Vector3	m_position;	///< 軸の中心座標


public:
	// コンストラクタ
	Capsule(
		const DirectX::SimpleMath::Vector3& axis,
		const float& length,
		const DirectX::SimpleMath::Vector3& position,
		const float& radius);

	Capsule();

	// デストラクタ
	~Capsule() = default;

	// メンバ関数

	// 衝突判定
public:

	// AABBとの衝突判定
	bool CheckHit(const AABB& aabb) const;
	bool CheckHit(const Sphere& sphere) const;


public:
	// クローンの取得
	std::unique_ptr<ICollider> GetClone() const override;
	// コライダーの種類の取得
	ColliderType GetColliderType() const override;

	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	// 軸の設定
	void SetAxis(const DirectX::SimpleMath::Vector3& axis);
	// 長さの設定
	void SetLength(const float& length);
	// 半径の設定
	void SetRadius(const float& radius);

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const;
	// 軸の取得
	DirectX::SimpleMath::Vector3 GetAxis() const;
	// 長さの取得
	float GetLength() const;
	// 半径の取得
	float GetRadius() const;

	void Draw(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);



};


// **** 衝突判定 *****

// 平面と球の衝突判定
bool IsHit(const Plane& plane,		const Sphere& sphere);
bool IsHit(const Sphere& sphereA,	const Sphere& sphereB);
bool IsHit(const Sphere& sphere,	const Triangle& triangle);
bool IsHit(const Segment& segment,	const Sphere& sphere, DirectX::SimpleMath::Vector3* pHitPos = nullptr);
bool IsHit(const Triangle& triangle, const Segment& segment);
bool IsHit(const Box2D& box, const Sphere& sphere);
bool IsHit(const Box2D& box, const Segment& segment);
bool IsHit(const Plane& plane, const Segment& segment, DirectX::SimpleMath::Vector3* pHitPos = nullptr);
bool IsHit(const AABB& boxA, const AABB& boxB);
bool IsHit(const AABB& box, const Sphere& sphere);
bool IsHit(const AABB& box, const Segment& segment);
bool IsHit(const Cylinder& cylinder, const AABB& aabb);
bool IsHit(const Capsule& capsule, const AABB& aabb);
bool IsHit(const Capsule& capsule, const Sphere& sphere);

// **** 交差点の取得 *****
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Plane&	 plane,		const Segment& segment);
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Triangle& trianle,	const Segment& segment);
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Box2D&	 box,		const Segment& segment);
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Sphere&	 sphere,	const Segment& segment);
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Sphere&	 sphere,	const Segment& segment);
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const AABB&	 aabb,		const Segment& segment);

bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const AABB&	 aabb,		const Capsule& capsule);

// 
bool GetIntersectionPointOnSurface(const AABB&	 aabb,	const Capsule& capsule, DirectX::SimpleMath::Vector3* pOutEnter = nullptr, DirectX::SimpleMath::Vector3* pOutExit = nullptr);

bool GetIntersectionPointOnSurface(const Sphere& sphere,	const Capsule& capsule, DirectX::SimpleMath::Vector3* pOutEnter = nullptr, DirectX::SimpleMath::Vector3* pOutExit = nullptr);

// ユーティリティ関数（レイと球の交差を計算）
inline int GetSphereIntersection(const DirectX::SimpleMath::Vector3& rayOrigin, const DirectX::SimpleMath::Vector3& rayDir, const DirectX::SimpleMath::Vector3& sphereCenter, float sphereRadius, float* pT0, float* pT1);


// *** とりあえずの場所（後でしっかりとしたクラスを作る） ****
// 押し出しの計算
DirectX::SimpleMath::Vector3 CalcOverlap(const Plane& plane, const Sphere& sphere);

// 押し出しの計算
DirectX::SimpleMath::Vector3 CalcOverlap(const Triangle& triangle, const Sphere& sphere);

// 押し出しの計算
DirectX::SimpleMath::Vector3 CalcOverlap(const Box2D& box2D, const Sphere& sphere);
DirectX::SimpleMath::Vector3 CalcOverlap(const Sphere& sphereA, const Sphere& sphereB);

DirectX::SimpleMath::Vector3 CalcOverlap(const AABB& box, const Sphere& sphere);
DirectX::SimpleMath::Vector3 CalcOverlap(const AABB& aabb, const Sphere& sphere, const DirectX::SimpleMath::Vector3& overlapDirection);

DirectX::SimpleMath::Vector3 CalcOverlapNormal(const AABB& box, const Sphere& sphere);

// 押し出し量の計算
float CalcOverlapValue(const Plane& plane, const Sphere& sphere);

// 接触座標の算出
DirectX::SimpleMath::Vector3 CalcContactPoint(const Triangle& triangle, const Sphere& sphere);

// **** 最近点 *****

// 線分と点の最近点を求める
DirectX::SimpleMath::Vector3 CalcClosestPointOnSegment(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, const DirectX::SimpleMath::Vector3& point);

// 点とAABBの最近点を求める
DirectX::SimpleMath::Vector3 CalcClosestPointOnAABB(const AABB& aabb, const DirectX::SimpleMath::Vector3& point);
DirectX::SimpleMath::Vector3 CalcClosestPointOnAABBSurface(const AABB& aabb, const DirectX::SimpleMath::Vector3& point);



