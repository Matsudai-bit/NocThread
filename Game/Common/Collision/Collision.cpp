/*****************************************************************//**
 * @file    Collision.h
 * @brief   衝突判定に関するソースファイル
 *
 * @author  松下大暉
 * @date    2025/01/27
 *********************************************************************/

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

using namespace DirectX;

#include "Collision.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pos		座標
 * @param[in] radius	半径
 */
Sphere::Sphere(const SimpleMath::Vector3& pos, const float& radius)
	: m_pos		{pos}
	, m_radius	{radius}
{

}

Sphere::Sphere()
	: m_pos{}
	, m_radius{}
{
}



/**
 * @brief 座標の設定
 * 
 * @param[in] translation
 */
void Sphere::Transform(DirectX::SimpleMath::Vector3 translation)
{
	m_pos = translation;
}

/**
 * @brief 回転
 * 
 * @param[in] rotation (ラジアン角)
 */
void Sphere::Rotate(DirectX::SimpleMath::Vector3 rotation)
{
	m_rotate = rotation;

}

/**
 * @brief ワールド行列の取得
 * 
 * @return ワールド行列
 */
DirectX::SimpleMath::Matrix Sphere::GetWorldMatrix() const
{
	// 行列の計算
		// 行列の計算
	SimpleMath::Matrix rotationZ = SimpleMath::Matrix::CreateRotationZ(m_rotate.z);
	SimpleMath::Matrix rotationY = SimpleMath::Matrix::CreateRotationY(m_rotate.y);
	SimpleMath::Matrix rotationX = SimpleMath::Matrix::CreateRotationX(m_rotate.x);

	SimpleMath::Matrix transform = SimpleMath::Matrix::CreateTranslation(m_pos);

	SimpleMath::Matrix world = transform * rotationZ * rotationY * rotationX ;
	return world;
}

/**
 * @brief 平面との衝突判定
 * 
 * @param[in] plane　平面
 * 
 * @return true 衝突している
 */
bool Sphere::CheckHit(const Plane& plane) const
{
	return IsHit(plane, *this);
}

/**
 * @brief 線分との衝突判定
 * 
 * @param[in] segment　線分
 * 
 * @return true 衝突している
 */
bool Sphere::CheckHit(const Segment& segment) const
{
	return IsHit(segment, *this);
}

/**
 * @brief 三角形のとの衝突判定
 * 
 * @param[in] triangle　三角形
 * 
 * @return true 衝突している
 */
bool Sphere::CheckHit(const Triangle& triangle) const
{
	return IsHit(*this, triangle);
}

/**
 * @brief 球との衝突判定
 * 
 * @param[in] sphere 球
 * 
 * @return true 衝突している
 */
bool Sphere::CheckHit(const Sphere& sphere) const
{
	return IsHit(*this, sphere);
}

/**
 * @brief 四角形との衝突判定
 * 
 * @param[in] box 四角形
 * 
 * @return true 衝突している
 */
bool Sphere::CheckHit(const Box2D& box) const
{
	return IsHit(box, *this);
}

/**
 * @brief AABBとの衝突判定
 * 
 * @param[in] aabb　
 * 
 * @return true 衝突している
 */
bool Sphere::CheckHit(const AABB& aabb) const
{
	return IsHit(aabb, *this);
}

/**
 * @brief カプセルとの衝突判定
 * 
 * @param[in] capsule　カプセル
 * 
 * @return true 衝突している
 */
bool Sphere::CheckHit(const Capsule& capsule) const
{
	return IsHit(capsule, *this);
}



/**
 * @brief 座標の取得
 * 
 * @return 座標
 */
DirectX::SimpleMath::Vector3 Sphere::GetPosition() const
{
	return m_pos;
}

/**
 * @brief 半径の取得
 * 
 * @return 半径
 */
float Sphere::GetRadius() const
{
	return m_radius;
}

/**
 * @brief 半径の設定
 * 
 * @param[in] radius　半径
 */
void Sphere::SetRadius(const float& radius)
{
	m_radius = radius;
}

void Sphere::Draw(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	std::unique_ptr<GeometricPrimitive>  a = GeometricPrimitive::CreateSphere(context, m_radius * 2.0f);
	a->Draw(SimpleMath::Matrix::CreateTranslation(m_pos), view, proj, Colors::White, nullptr ,true);
}

/**
 * @brief コライダーの種類の取得
 * 
 * @return コライダーの種類
 */
ColliderType Sphere::GetColliderType() const
{
	return ColliderType::Sphere;
}

// 平面 --------------------------------------------------------------------

Plane::Plane()
	: m_vec4{}
{
}

/**
 * @brief コンストラクタ
 * 
 * @param[in] normal	法線ベクトル
 * @param[in] point		平面上の座標
 */
Plane::Plane(DirectX::SimpleMath::Vector3 normal, const DirectX::SimpleMath::Vector3& point)
{
	Set(normal, point);
}

/**
 * @brief コンストラクタ
 * 
 * @param[in] pointA 座標A
 * @param[in] pointB 座標B
 * @param[in] pointC 座標C
 */
Plane::Plane(DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB, DirectX::SimpleMath::Vector3 pointC)
{
	Set(pointA, pointB, pointC);
}

/**
 * @brief 球との衝突判定
 * 
 * @param[in] sphere 球
 * 
 * @return true 衝突している
 */
bool Plane::CheckHit(const Sphere& shere) const
{
	return IsHit(*this, shere);
}

/**
 * @brief 線分との衝突判定
 *
 * @param[in] segment　線分
 *
 * @return true 衝突している
 */
bool Plane::CheckHit(const Segment& segment) const
{
	return IsHit(*this, segment);
}

/**
 * @brief 平面情報の取得
 * 
 * @return 平面情報
 */
DirectX::SimpleMath::Vector4 Plane::GetVector4() const
{
	return m_vec4;
}

/**
 * @brief 平面との距離の算出
 * 
 * @param[in] pos 座標
 * 
 * @return 距離
 */
float Plane::CalcLength(DirectX::SimpleMath::Vector3 pos) const
{
	return std::abs(m_vec4.x * pos.x + m_vec4.y * pos.y + m_vec4.z * pos.z + m_vec4.w);
}

/**
 * @brief 平面の設定
 * 
 * @param[in] normal
 * @param[in] point
 */
void Plane::Set(DirectX::SimpleMath::Vector3 normal, const DirectX::SimpleMath::Vector3& point)
{
	// 法線ベクトルを正規化
	DirectX::SimpleMath::Vector3 n = normal;
	n.Normalize();

	// 平面の設定
	DirectX::SimpleMath::Vector4 r;
	r.x = n.x;
	r.y = n.y;
	r.z = n.z;

	// 平面のwを設定
	r.w = -((n.x * point.x) + (n.y * point.y) + (n.z * point.z));

	m_vec4 = r;
}

void Plane::Set(DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB, DirectX::SimpleMath::Vector3 pointC)
{
	DirectX::SimpleMath::Plane plane = { pointA, pointB, pointC };


	Set(plane.Normal(), pointA);
}

/**
 * @brief コライダーの種類の取得
 * 
 * @return コライダーの種類
 */
ColliderType Plane::GetColliderType() const
{
	return ColliderType::Plane;
}

// *********************** 三角形  **************************



/**
 * @brief コンストラクタ
 * 
 * @param[in] posA	座標A
 * @param[in] posB	座標B
 * @param[in] posC	座標C
 */
Triangle::Triangle(
	const DirectX::SimpleMath::Vector3& posA,
	const DirectX::SimpleMath::Vector3& posB,
	const DirectX::SimpleMath::Vector3& posC,
	DX::DeviceResources* pDeviceResources)
	: m_pos{ 3 }
	, m_plane{posA, posB, posC}
	, m_vertexes{}
	, m_primitiveBatch{pDeviceResources->GetD3DDeviceContext()}
	, m_inputLayout{}
	, m_basicEffect{pDeviceResources->GetD3DDevice()}
	, m_pDeviceResources{pDeviceResources}

{

	// ライト(OFF)
	m_basicEffect.SetLightingEnabled(false);
	// 頂点カラー(ON)
	m_basicEffect.SetVertexColorEnabled(true);
	// テクスチャ(OFF)
	m_basicEffect.SetTextureEnabled(false);

	// 入力レイアウト作成
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(
			pDeviceResources->GetD3DDevice(), &m_basicEffect, m_inputLayout.ReleaseAndGetAddressOf())
	);

	// 3頂点座標の設定
	Set(posA, posB, posC);


}


/**
 * @brief 任意の点が三角形内に存在するかどうか
 * 
 * @param[in] pos
 * 
 * @return true する
 */
bool Triangle::CheckInner(const SimpleMath::Vector3& pos) const
{
	// **** 三角形の法線ベクトルを取得 ****
	SimpleMath::Vector3 normal = (m_pos[1] - m_pos[0]).Cross(m_pos[2] - m_pos[0]);
	normal.Normalize();

	// **** 点 pos が三角形の平面上にあるか確認 ****
	float dist = normal.Dot(pos - m_pos[0]);
	const float EPSILON = 1e-5f; // 許容誤差
	if (std::abs(dist) > EPSILON) return false;

	// **** 三角形の辺ベクトルの作成 ****
	SimpleMath::Vector3 v[3];
	v[0] = m_pos[1] - m_pos[0];
	v[1] = m_pos[2] - m_pos[1];
	v[2] = m_pos[0] - m_pos[2];

	// **** 任意点から各頂点へのベクトル作成 ****
	SimpleMath::Vector3 vt[3];
	vt[0] = pos - m_pos[0];
	vt[1] = pos - m_pos[1];
	vt[2] = pos - m_pos[2];

	// **** 各外積を求める ****
	SimpleMath::Vector3 c[3];
	c[0] = v[0].Cross(vt[0]);
	c[1] = v[1].Cross(vt[1]);
	c[2] = v[2].Cross(vt[2]);

	// **** 外積の向きが揃っているかチェック（内積を利用） ****
	float dot0 = c[0].Dot(normal);
	float dot1 = c[1].Dot(normal);
	float dot2 = c[2].Dot(normal);

	// **** すべての符号が一致しているかをチェック ****
	if ((dot0 >= -EPSILON && dot1 >= -EPSILON && dot2 >= -EPSILON) ||
		(dot0 <= EPSILON && dot1 <= EPSILON && dot2 <= EPSILON))
	{
		return true;
	}
	return false;

}

/**
 * @brief 三角形のエッジ上で指定点に最も近い点を求める
 * 
 * @param[in] target 探したい点 
 * 
 * @return	三角形のエッジでtargetに最も近い点
 */
DirectX::SimpleMath::Vector3 Triangle::GetClosestPointVertex(const DirectX::SimpleMath::Vector3& target) const
{
	// 三角形の3つの頂点
	const std::vector<SimpleMath::Vector3>& triPos = GetPosition();

	// ① もし target が三角形の内部なら、そのまま返す
	if (CheckInner(target)) {
		return target;
	}

	// ② 各エッジ上の最近接点を求める
	SimpleMath::Vector3 closest = target;
	float minDistanceSq = FLT_MAX;  // 最小距離の二乗（比較用）

	for (size_t i = 0; i < 3; i++) {
		// エッジの始点と終点
		SimpleMath::Vector3 p1 = triPos[i];
		SimpleMath::Vector3 p2 = triPos[(i + 1) % 3];

		Segment seg{ p1, p2 , true};

		// エッジ上の最近接点を求める
		SimpleMath::Vector3 closestOnEdge = seg.ClosestPoint(target);

		// target との距離を計算
		float distSq = (closestOnEdge - target).LengthSquared();

		// 最小距離なら更新
		if (distSq < minDistanceSq) {
			minDistanceSq = distSq;
			closest = closestOnEdge;
		}
	}

	return closest;
}
/**
 * @brief 指定した点で三角形上の最も近い点を取得
 *
 * @param[in] target 探したい点
 *
 * @return	三角形上でtargetに最も近い点
 */
DirectX::SimpleMath::Vector3 Triangle::CalcClosestPoint(const DirectX::SimpleMath::Vector3& point, const Triangle& triangle) 
{
	using namespace DirectX;
	std::vector<SimpleMath::Vector3> triPos = triangle.GetPosition(); // triPos[0], triPos[1], triPos[2]

	// 1. 点の平面への射影を計算
	// まず、点から三角形を含む無限平面への最も近い点を求めます。
	SimpleMath::Vector4 planeVec4 = triangle.GetPlane().GetVector4(); // 平面 (A, B, C, D)
	SimpleMath::Vector3 planeNormal(planeVec4.x, planeVec4.y, planeVec4.z); // 法線ベクトル (A,B,C)
	// planeNormal が正規化されていることを前提とします。

	float signedDistance = point.Dot(planeNormal) + planeVec4.w;
	SimpleMath::Vector3 projectedPoint = point - signedDistance * planeNormal;

	// 2. 射影点が三角形の内側にあるかチェック (重心座標などを使用)
	// このチェックは「点 inside 三角形」の判定です。
	// Barycentric Coordinates (重心座標) を使用するのが一般的です。
	SimpleMath::Vector3 v0 = triPos[1] - triPos[0];
	SimpleMath::Vector3 v1 = triPos[2] - triPos[0];
	SimpleMath::Vector3 v2 = projectedPoint - triPos[0];

	float dot00 = v0.Dot(v0);
	float dot01 = v0.Dot(v1);
	float dot11 = v1.Dot(v1);
	float dot02 = v0.Dot(v2);
	float dot12 = v1.Dot(v2);

	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// 射影点が三角形内 (または辺上) にある場合
	if (u >= 0 && v >= 0 && (u + v) <= 1)
	{
		return projectedPoint;
	}

	// 3. 射影点が三角形の外にある場合、最も近い点は辺または頂点上にある
	// 各辺に対して最も近い点を計算し、その中で球の中心に近いものを選ぶ。
	SimpleMath::Vector3 closest = projectedPoint; // 初期値として射影点を使うが、実際には最も遠い候補
	float minSqDist = FLT_MAX; // 最小二乗距離

	// 辺 (triPos[0], triPos[1])
	SimpleMath::Vector3 p01 = Segment::GetClosestPoint(point, triPos[0], triPos[1]);
	float sqDist01 = SimpleMath::Vector3::DistanceSquared(point, p01);
	if (sqDist01 < minSqDist) {
		minSqDist = sqDist01;
		closest = p01;
	}

	// 辺 (triPos[1], triPos[2])
	SimpleMath::Vector3 p12 = Segment::GetClosestPoint(point, triPos[1], triPos[2]);
	float sqDist12 = SimpleMath::Vector3::DistanceSquared(point, p12);
	if (sqDist12 < minSqDist) {
		minSqDist = sqDist12;
		closest = p12;
	}

	// 辺 (triPos[2], triPos[0])
	SimpleMath::Vector3 p20 = Segment::GetClosestPoint(point, triPos[2], triPos[0]);
	float sqDist20 = SimpleMath::Vector3::DistanceSquared(point, p20);
	if (sqDist20 < minSqDist) {
		minSqDist = sqDist20;
		closest = p20;
	}

	return closest;
}

/**
 * @brief 球との衝突判定
 * 
 * @param[in] sphere 球
 * 
 * @return true 衝突している
 */
bool Triangle::CheckHit(const Sphere& sphere) const
{
	return IsHit(sphere , *this);
}

/**
 * @brief 線分との衝突判定
 * 
 * @param[in] segment 線分
 * 
 * @return true 衝突している
 */
bool Triangle::CheckHit(const Segment& segment) const
{
	return IsHit(*this, segment);
}

/**
 * @brief コライダーの種類の取得
 * 
 * @return コライダーの種類
 */
ColliderType Triangle::GetColliderType() const
{
	return ColliderType::Triangle;
}

/**
 * @brief 3頂点座標の設定
 * 
 * @param[in] posA	座標A
 * @param[in] posB	座標B
 * @param[in] posC	座標C
 */
void Triangle::Set(
	const DirectX::SimpleMath::Vector3& posA,
	const DirectX::SimpleMath::Vector3& posB, 
	const DirectX::SimpleMath::Vector3& posC)
{



	// 座標の設定
	m_pos[0] = posA;
	m_pos[1] = posB;
	m_pos[2] = posC;

	for (int i = 0; i < VERTEX_NUM; i++)
	{
		m_vertexes[i].position	= m_pos[i];
		m_vertexes[i].color = COLOR;
	}
	

	// 平面の設定
	m_plane.Set(posA, posB, posC);

	// 親座標の設定
	m_parentPos = m_pos;
	
}

/**
 * @brief 座標の取得
 * 
 * @return ３点座標
 */
const std::vector<DirectX::SimpleMath::Vector3>& Triangle::GetPosition() const
{
	return m_pos;
}

/**
 * @brief 平面情報の取得
 * 
 * @return 平面
 */
const Plane& Triangle::GetPlane() const
{
	return m_plane;
}

/**
 * @brief 回転する
 * 
 * @param[in] rotate 回転量
 */
void Triangle::Rotate(DirectX::SimpleMath::Vector3 rotate)
{
	// 回転行列の作成

	SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateFromYawPitchRoll(rotate);

	// **** 各座標を回転させる ****
	m_pos[0] = SimpleMath::Vector3::TransformNormal(m_parentPos[0], rotationMatrix);
	m_pos[1] = SimpleMath::Vector3::TransformNormal(m_parentPos[1], rotationMatrix);
	m_pos[2] = SimpleMath::Vector3::TransformNormal(m_parentPos[2], rotationMatrix);

	for (int i = 0; i < VERTEX_NUM; i++)
	{
		m_vertexes[i].position = m_pos[i];
	}

	// 平面ベクトルの設定
	m_plane.Set(m_pos[0], m_pos[1], m_pos[2]);
}




/**
 * @brief 描画処理
 * 
 */
void Triangle::Draw(const SimpleMath::Matrix& view, const SimpleMath::Matrix& proj)
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();

	// 入力レイアウト
	context->IASetInputLayout(m_inputLayout.Get());

	m_basicEffect.SetWorld(SimpleMath::Matrix::Identity);

	m_basicEffect.SetView(view);
	m_basicEffect.SetProjection(proj);

	// 適用
	m_basicEffect.Apply(context);

	// プリミティブバッチの描画
	m_primitiveBatch.Begin();

	// 三角形の描画
	m_primitiveBatch.DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, INDEXES, 3, m_vertexes, 3);

	m_primitiveBatch.End();
}

// *************** 線分 ****************************

/**
 * @brief コンストラクタ
 *
 * @param[in] pos 始点座標
 * @param[in] vec ベクトル
 * @param[in] useEndPoint 終点座標を指定するかどうか
 */
Segment::Segment(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& vec, bool useEndPoint)
	: m_pos{ pos }
	, m_vec{ vec }
{
	Set(pos, vec, useEndPoint);
}

Segment::Segment()
	: m_pos{  }
	, m_vec{ }
{
}

/**
 * @brief 線分上の target に最も近い点を求める
 * 
 * @param[in] target 探したい点
 * 
 * @return 線分上でtargetに最も近い点
 */
DirectX::SimpleMath::Vector3 Segment::ClosestPoint(const DirectX::SimpleMath::Vector3& target)
{
	// p1 からtargetへのベクトル
	SimpleMath::Vector3  toTarget = target - m_pos;

	float edgeLengthSq = m_vec.LengthSquared();  // 線分の長さの二乗

	// エッジがゼロ長なら（頂点が同じなら）、p1 を返す
	if (edgeLengthSq == 0.0f) {
		return m_pos;
	}

	// target が線分上のどこにあるかを計算
	float t = toTarget.Dot(m_vec) / edgeLengthSq;

	// t を 0～1 に制限
	t = std::max(0.0f, std::min(1.0f, t));
	// 線分上の最近接点

	return m_pos + m_vec * t;
}

/**
 * @brief 球との衝突判定
 * 
 * @param[in] sphere 球
 * 
 * @return true 衝突している
 */
bool Segment::CheckHit(const Sphere& sphere) const
{
	return IsHit(*this, sphere);
}

/**
 * @brief 三角形との衝突判定
 *
 * @param[in] triangle 三角形
 *
 * @return true 衝突している
 */
bool Segment::CheckHit(const Triangle& triangle) const
{
	return IsHit(triangle, *this);
}

/**
 * @brief 四角形との衝突判定
 * 
 * @param[in] box 四角形
 * 
 * @return true 衝突している
 */
bool Segment::CheckHit(const Box2D& box) const
{
	return IsHit(box, *this);
}

/**
 * @brief 平面との衝突判定
 *
 * @param[in] plane 平面
 *
 * @return true 衝突している
 */
bool Segment::CheckHit(const Plane& plane) const
{
	return IsHit(plane, *this);
}

/**
 * @brief AABBとの衝突判定
 * 
 * @param[in] box　ボックス
 * @return 　true 衝突している
 */
bool Segment::CheckHit(const AABB& box) const
{
	return IsHit(box, *this);
}

/**
 * @brief 線分との最も近い点
 * 
 * @param[in] point
 * @param[in] segStart
 * @param[in] segEnd
 * @return 
 */
SimpleMath::Vector3 Segment::GetClosestPoint(const SimpleMath::Vector3& point, const SimpleMath::Vector3& segStart, const SimpleMath::Vector3& segEnd)
{
	return CalcClosestPointOnSegment(segStart, segEnd, point);
}


/**
 * @brief コライダーの種類の取得
 * 
 * @return コライダーの種類
 */
ColliderType Segment::GetColliderType() const
{
	return ColliderType::Segment;
}

/**
 * @brief 設定
 *
 * @param[in] pos
 * @param[in] vec
 */
void Segment::Set(
	const DirectX::SimpleMath::Vector3& pos,
	const DirectX::SimpleMath::Vector3& vec,
	bool useEndPoint)
{

	m_pos = pos;

	if (useEndPoint)
	{
		m_vec = vec - pos;	// 終点 - 始点
	}
	else
	{
		m_vec = vec;
	}

}

/**
 * @brief 始点座標の取得
 *
 * @return 始点座標
 */
DirectX::SimpleMath::Vector3 Segment::GetPosition() const
{
	return m_pos;
}

/**
 * @brief 終点座標の取得
 * 
 * @return 終点座標
 */
DirectX::SimpleMath::Vector3 Segment::GetEndPosition() const
{
	return m_pos + m_vec;
}

/**
 * @brief ベクトルの取得
 *
 * @return ベクトル
 */
DirectX::SimpleMath::Vector3 Segment::GetVector() const
{
	return m_vec;
}


/**
 * @brief 平面と球の衝突
 * 
 * @param[in] plane 平面
 * @param[in] sphere球
 * 
 * @return  true 衝突
 */
bool IsHit(const Plane& plane, const Sphere& sphere)
{
	// 重要: plane.GetVector4() は、平面の方程式 (A, B, C, D) が正規化されている
	// (つまり、法線ベクトル (A, B, C) の大きさが 1 である) ことを前提とします。
	SimpleMath::Vector4 planeVec4 = plane.GetVector4(); // 正規化されていると仮定: |(A,B,C)| = 1
	SimpleMath::Vector3 spherePos = sphere.GetPosition();

	// 球の中心から平面までの符号付き距離を計算します。
	// 平面が Ax + By + Cz + D = 0 で定義されている場合、
	// (A,B,C) が単位法線ベクトルであれば、この 'signedDistance' は符号付き距離になります。
	float signedDistance = planeVec4.x * spherePos.x + planeVec4.y * spherePos.y + planeVec4.z * spherePos.z + planeVec4.w;

	// 符号付き距離の絶対値が球の半径以下であれば衝突です。
	// これにより、球の中心が平面のどちら側にあっても正しく衝突を判定できます。
	if (std::abs(signedDistance) <= sphere.GetRadius())
	{
		return true; // 衝突を検出
	}
	else
	{
		return false; // 衝突なし
	}
}

/**
 * @brief 球と球の衝突判定
 * 
 * @param[in] sphereA
 * @param[in] sphereB
 * @return 
 */
bool IsHit(const Sphere& sphereA, const Sphere& sphereB)
{
	float distanceSqr = SimpleMath::Vector3::DistanceSquared(sphereA.GetPosition(), sphereB.GetPosition());

	return (distanceSqr < sphereA.GetRadius() + sphereB.GetRadius());
	
}

/**
 * @brief 球と三角形の衝突判定
 * 
 * @param[in] sphere
 * @param[in] triangle
 * 
 * @returns true　衝突
 */
bool IsHit(const Sphere& sphere, const Triangle& triangle)
{
	// 1. 無限平面との衝突判定 (ブロードフェーズ)
	// 前回の修正版 IsHit(Plane, Sphere) を使用
	if (IsHit(triangle.GetPlane(), sphere) == false)
	{
		return false;
	}

	// 2. 球の中心から三角形上の最も近い点を特定
	SimpleMath::Vector3 closestPointOnTriangle = Triangle::CalcClosestPoint(sphere.GetPosition(), triangle);

	// 3. 最も近い点と球の中心の距離を計算し、半径と比較
	float distanceSq = SimpleMath::Vector3::DistanceSquared(sphere.GetPosition(), closestPointOnTriangle);
	float radiusSq = sphere.GetRadius() * sphere.GetRadius();

	return distanceSq <= radiusSq;
	//// 球と平面の衝突判定
	//if (IsHit(triangle.GetPlane(), sphere) == false) return false;

	//// 三角形の三点座標の取得
	//std::vector<SimpleMath::Vector3> triPos = triangle.GetPosition();

	//// 三角形の平面ベクトルの取得
	//SimpleMath::Vector4 planeNormal = triangle.GetPlane().GetVector4();

	//// 三角形の辺が球を貫通するか
	//SimpleMath::Vector3 vec;

	//vec = triPos[1] - triPos[0];
	//if (IsHit(Segment(triPos[0], vec), sphere)) return true;

	//vec = triPos[2] - triPos[1];
	//if (IsHit(Segment(triPos[1], vec), sphere)) return true;

	//vec = triPos[0] - triPos[2];
	//if (IsHit(Segment(triPos[2], vec), sphere)) return true;

	//// **** 球が三角形の中央にめり込むか ****
	//float penetrationDepth = sphere.GetPosition().Dot(SimpleMath::Vector3(planeNormal.x, planeNormal.y, planeNormal.z)) - planeNormal.w;

	//SimpleMath::Vector4 signedNormal = (penetrationDepth > 0.0f) ? planeNormal : -planeNormal;

	//// 球の中心から平面への垂線として平面の法線を利用する
	//vec = SimpleMath::Vector3(-signedNormal.x * sphere.GetRadius(), -signedNormal.y * sphere.GetRadius(), -signedNormal.z * sphere.GetRadius());

	//return IsHit(triangle, Segment(sphere.GetPosition(), vec));
}

/**
 * @brief 線分と球の衝突判定
 *
 * @param[in] segment
 * @param[in] sphere
 * @param[out] pHitPos 衝突地点
 *
 * @returns true　衝突
 */
bool IsHit(const Segment& segment, const Sphere& sphere, SimpleMath::Vector3* pHitPos )
{
	// **** 事前準備 ****
	// 線分の始点座標の取得
	const SimpleMath::Vector3 segPos = segment.GetPosition();
	// 線分のベクトルの取得
	const SimpleMath::Vector3 segVec = segment.GetVector();
	// 球の中心座標の取得
	const SimpleMath::Vector3 sphPos = sphere.GetPosition();
	// 球の半径の取得
	const float radius = sphere.GetRadius();

	// **** 2次方程式における a, b, c を算出していく ****
	// ax^2 + bx + c = 0 の形に変形する
	SimpleMath::Vector3 diff = segPos - sphPos;
	float a = (segVec.x * segVec.x) + (segVec.y * segVec.y) + (segVec.z * segVec.z);
	float b = 2.0f * (segVec.x * diff.x + segVec.y * diff.y + segVec.z * diff.z);
	float c = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z) - (radius * radius);

	// **** 判別式 d の計算 ****
	float d = (b * b) - (4.0f * a * c);

	// 二次方程式が解けない（解なし）場合は衝突していないとみなす
	if (d < 0.0f) return false;

	// **** 解の公式を解く ****
	// 先に d を平方根にする
	d = sqrtf(d);
	float t0 = (-b - d) / (2.0f * a);
	float t1 = (-b + d) / (2.0f * a);

	// **** 線分の内部にいるかどうかを確認する ****
	// c <= 0 の場合、線分の始点が球の内部にあるため衝突とみなす
	if (c <= 0.0f)
	{
		if (pHitPos) *pHitPos = segPos;
		return true;
	}

	// t が線分の範囲 [0, 1] にあるかをチェック
	//if ((t0 >= 0.0f && t0 <= 1.0f) || (t1 >= 0.0f && t1 <= 1.0f)) {
	//	return true;
	//}

	// 線分内の最寄りのtを探る
	float t = 2.0f;
	if ((t0 >= 0.0f) && (t0 <= 1.0f) && (t0 <= t)) t = t0;
	if ((t1 >= 0.0f) && (t1 <= 1.0f) && (t1 <= t)) t = t1;
	if (t > 1.0f) return false;

	// 交点を算出
	if (pHitPos) *pHitPos = segPos + segVec * t;

	// **** 衝突なし ****
	return true;
}

/**
 * @brief 三角形と線分の衝突判定
 *
 * @param[in] triangle　三角形
 * @param[in] segment	線分
 * @return
 */
bool IsHit(const Triangle& triangle, const Segment& segment)
{
	// 衝突座標
	SimpleMath::Vector3 hitTemp;

	// 線分と平面の衝突判定
	if (IsHit(triangle.GetPlane(), segment, &hitTemp) == false)
		return false;

	// 線分が三角形中に存在しているかどうか
	return (triangle.CheckInner(hitTemp));
}

/**
 * @brief 四角形と球の衝突判定
 *
 * @param[in] box
 * @param[in] sphere
 * @return
 */
bool IsHit(const Box2D& box, const Sphere& sphere)
{
	return (
		box.GetTriangle()[0]->CheckHit(sphere) || 
		box.GetTriangle()[1]->CheckHit(sphere));
}

/**
 * @brief 四角形と線分の衝突判定
 * 
 * @param[in] box
 * @param[in] segment
 * @return 
 */
bool IsHit(const Box2D& box, const Segment& segment)
{
	return (
		box.GetTriangle()[0]->CheckHit(segment) ||
		box.GetTriangle()[1]->CheckHit(segment));
}

/**
 * @brief 平面と線分の衝突判定
 * 
 * @param[in]	plane	平面
 * @param[in]	segment	線分
 * @param[out]	hitPos	衝突位置	
 * 
 * @return 
 */
bool IsHit(const Plane& plane, const Segment& segment, SimpleMath::Vector3* pHitPos)
{
	if (pHitPos)
		*pHitPos = SimpleMath::Vector3::Zero;

	// **** 事前準備 ****

	// 線分の始点座標を取得
	SimpleMath::Vector3 segPos = segment.GetPosition();
	// 線分のベクトルを取得
	SimpleMath::Vector3 segVec = segment.GetVector();
	// 平面ベクトルを取得
	SimpleMath::Vector4 planeVec = plane.GetVector4();

	// **** 線分始点と平面の距離を算出 ****
	float length = plane.CalcLength(segPos);

	// **** 線分ベクトルの長さを算出 **** 
	// √ を回避するために長さの二乗を利用
	float vectorLength2 = segVec.LengthSquared();

	// 距離比較し、平面に絶対に届かない場合は抜ける
	if (vectorLength2 < (length * length)) return false;

	// 線分ベクトルの長さ
	float segLength = std::sqrtf( vectorLength2);

	// **** cosθを内積を利用して求める ****

	// 1/cosθを線分ベクトルに掛けることで正規化
	SimpleMath::Vector3 segNormal = segVec / segLength;

	// 正規化された線分ベクトルと平面法線の反転を利用して内積を求める
	SimpleMath::Vector3 planeVec3 = { planeVec.x, planeVec.y, planeVec.z };
	float dot = segNormal.Dot(-planeVec3);	// 表裏を区別する時はこっちを使う
	//float dot = std::abs(segNormal.Dot(planeVec3));

	// *** ゼロ除算防止（平面と平行な場合）***
	if (std::abs(dot) < 1e-6f) return false;

	// *** 線分が平面に届く距離を算出し、平面に届くか判断する ***
	float planeLengthScaler = length / std::abs(dot);

	if (planeLengthScaler > segLength) return false;

	// *** 平面との交点を算出 ***
	if (pHitPos)
	{
		// 正規化した線分ベクトルの長さをちょうど平面に届く距離にする
		*pHitPos = segPos + (segNormal * planeLengthScaler);
	}

	return true;
}

/**
 * @brief AABBとAABBの衝突判定
 * 
 * @param[in] boxA
 * @param[in] boxB
 * @return 
 */
bool IsHit(const AABB& boxA, const AABB& boxB)
{
	using namespace SimpleMath;

	Vector3  centerA = boxA.GetCenter();
	Vector3  centerB = boxB.GetCenter();

	Vector3 extendA = boxA.GetExtend();
	Vector3 extendB = boxB.GetExtend();

	// 自身のAABBの最小点と最大点を計算
	DirectX::SimpleMath::Vector3 minA = centerA - extendA / 2.0f;
	DirectX::SimpleMath::Vector3 maxA = centerA + extendA / 2.0f;

	// 相手のAABBの最小点と最大点を計算
	DirectX::SimpleMath::Vector3 minB = centerB - extendB / 2.0f;
	DirectX::SimpleMath::Vector3 maxB = centerB + extendB / 2.0f;

	// どの軸でも重なっていないかチェック
	// どちらかのAABBがもう一方のAABBの左にある
	if ((maxA.x < minB.x) || (minA.x > maxB.x)) { return false; }

	// どちらかのAABBがもう一方のAABBの下にある
	if ((maxA.y < minB.y) || (minA.y > maxB.y)) {return false;}

	// どちらかのAABBがもう一方のAABBの手前にある
	if ((maxA.z < minB.z) || (minA.z > maxB.z)) { return false; }


	// どの軸でも分離していないので、衝突している
	return true;
}

/**
 * @brief AABBと球の衝突判定
 * 
 * @param[in] box     
 * @param[in] sphere
 * @return 
 */
bool IsHit(const AABB& box, const Sphere& sphere)
{

	SimpleMath::Vector3 closestPoint = box.CalcClosestPoint(sphere.GetPosition());

	// 球の中心とAABBの最も近い点の距離の二乗を計算
	float distanceSq = DirectX::SimpleMath::Vector3::DistanceSquared(sphere.GetPosition(), closestPoint);

	// 距離の二乗が半径の二乗より小さいかチェック
	return distanceSq < (sphere.GetRadius() * sphere.GetRadius());
}

/**
 * @brief 線分とボックス
 * 
 * @param[in] box
 * @param[in] segment
 * @return 
 */
bool IsHit(const AABB& box, const Segment& segment)
{
	using namespace SimpleMath;

	// EPS for "parallel" checks
	const double EPS = 1e-8;

	// compute aabb min/max (assume box.GetExtend() is full size)
	Vector3 aabbMin = box.GetCenter() - box.GetExtend() * 0.5f;
	Vector3 aabbMax = box.GetCenter() + box.GetExtend() * 0.5f;

	// ---- get segment endpoints robustly ----
	// We MUST use start and end explicitly.
	// If your Segment API provides Start and End getters, use them.
	// If not, assume segment.GetPosition() == start and segment.GetVector() == (end - start).
	Vector3 start = segment.GetPosition();
	Vector3 vec = segment.GetVector();
	Vector3 end = start + vec; // safe regardless: if GetVector() already is (end-start) then end is correct.

	// direction from start to end and segment parameter t in [0,1]
	Vector3 d = end - start;

	// Use double for param math to reduce numerical errors
	double tmin = 0.0;
	double tmax = 1.0;

	// For each axis (x=0,y=1,z=2)
	for (int i = 0; i < 3; ++i)
	{
		double s = (&start.x)[i];
		double di = (&d.x)[i];
		double mn = (&aabbMin.x)[i];
		double mx = (&aabbMax.x)[i];

		if (std::abs(di) < EPS)
		{
			// segment is parallel to slab planes for this axis:
			// if start coordinate not within slab, no intersection.
			if (s < mn - EPS || s > mx + EPS) return false;
			// else this axis imposes no constraint on t (all t allowed)
		}
		else
		{
			double inv = 1.0 / di;
			double t1 = (mn - s) * inv;
			double t2 = (mx - s) * inv;
			if (t1 > t2) std::swap(t1, t2);
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			if (tmin > tmax) return false; // no overlap
		}
	}

	// Final check: intersection interval must overlap the segment parameter range [0,1]
	if (tmax < 0.0 || tmin > 1.0) return false;

	return true;

}

/**
 * @brief 筒とAABBとの衝突判定
 * 
 * @param[in] cylinder　筒
 * @param[in] aabb		AABB
 * 
 * @return true 衝突している
 */
bool IsHit(const Cylinder& cylinder, const AABB& aabb)
{
	using namespace SimpleMath;

	// 円筒の軸を線分として定義
	Vector3 cylinderAxisStart = cylinder.GetPosition() - cylinder.GetAxis() * cylinder.GetLength() / 2.0f;
	Vector3 cylinderAxisEnd = cylinder.GetPosition() + cylinder.GetAxis() * cylinder.GetLength() / 2.0f;

	// キューブの中心
	Vector3 aabbCenter = aabb.GetCenter();

	// 線分と点との最近点を求める
	Vector3 closestPointOnAxis = CalcClosestPointOnSegment(cylinderAxisStart, cylinderAxisEnd, aabbCenter);

	// closestPointOnAxis からキューブの中心までのベクトル
	Vector3 toCubeCenter = aabbCenter - closestPointOnAxis;

	// キューブの中心から円筒の軸への最近点
	Vector3 closestPointOnCube = CalcClosestPointOnAABB(aabb, closestPointOnAxis);

	// 2つの最近点間の距離を計算
	float distanceSq = Vector3::DistanceSquared(closestPointOnAxis, closestPointOnCube);

	// 距離が円筒の半径の2乗より小さいかチェック
	if (distanceSq <= (cylinder.GetRadius() * cylinder.GetRadius())) {
		return true;
	}

	return false;
}

/**
 * @brief カプセルとAABBの衝突判定
 * 
 * @param[in] capsule　カプセル
 * @param[in] aabb		AABB
 * @return 
 */
bool IsHit(const Capsule& capsule, const AABB& aabb)
{
	using namespace DirectX::SimpleMath;

	// ------------------------------------------------------------------
	// 1. カプセルの軸線の両端点を計算
	//    m_position を中心に m_axis * (m_length / 2) を足し引き
	// ------------------------------------------------------------------
	Vector3 capsuleStart = capsule.GetPosition() - capsule.GetAxis() * (capsule.GetLength() * 0.5f);
	Vector3 capsuleEnd = capsule.GetPosition() + capsule.GetAxis() * (capsule.GetLength() * 0.5f);

	// 線分方向ベクトル
	Vector3 p0 = capsuleStart;
	Vector3 p1 = capsuleEnd;
	Vector3 dir = p1 - p0;
	float len = dir.Length();

	// 軸長がほぼ0の場合は衝突しない
	if (len < 1e-6f)
		return false;

	dir /= len; // 正規化

	// ------------------------------------------------------------------
	// 2. カプセル半径分だけ膨張したAABBを作る
	//    これにより軸線のみで衝突判定可能
	// ------------------------------------------------------------------
	Vector3 aabbMin = aabb.GetCenter() - aabb.GetExtend() * 0.5f - Vector3(capsule.GetRadius());
	Vector3 aabbMax = aabb.GetCenter() + aabb.GetExtend() * 0.5f + Vector3(capsule.GetRadius());

	float tmin = 0.0f;
	float tmax = len;

	// ------------------------------------------------------------------
	// 3. スラブ法で線分 vs AABB の交差判定
	//    各軸ごとに線分がAABBに入る t の範囲を求め、総合的に判定
	// ------------------------------------------------------------------
	for (int i = 0; i < 3; ++i)
	{
		float p0i = (&p0.x)[i];
		float di = (&dir.x)[i];
		float minVal = (&aabbMin.x)[i];
		float maxVal = (&aabbMax.x)[i];

		if (fabs(di) < 1e-6f)
		{
			// 軸に平行 → 線分がAABB範囲内にあるか確認
			if (p0i < minVal || p0i > maxVal)
				return false; // 外れていれば衝突なし
		}
		else
		{
			float ood = 1.0f / di;
			float t1 = (minVal - p0i) * ood;
			float t2 = (maxVal - p0i) * ood;
			if (t1 > t2) std::swap(t1, t2); // t1 <= t2 に整列
			if (t1 > tmin) tmin = t1;        // 線分全体の交差開始を更新
			if (t2 < tmax) tmax = t2;        // 線分全体の交差終了を更新
			if (tmin > tmax)
				return false; // 交差範囲がなくなった → 不衝突
		}
	}

	// 線分上に交差範囲があるか確認
	if (tmin < 0.0f || tmin > len)
		return false;

	// 衝突していれば true を返す
	return true;
}

/**
 * @brief 球とカプセルの衝突判定を行います。
 *
 * @param[in] capsule  判定対象のカプセル
 * @param[in] sphere   判定対象の球
 * @return true: 衝突している / false: 衝突していない
 */
bool IsHit(const Capsule& capsule, const Sphere& sphere)
{
	using namespace SimpleMath;

	// カプセル軸の線分を定義
	const Vector3 capsuleStart = capsule.GetPosition() - capsule.GetAxis() * (capsule.GetLength() * 0.5f);
	const Vector3 capsuleEnd = capsule.GetPosition() + capsule.GetAxis() * (capsule.GetLength() * 0.5f);

	
	// カプセル軸線分上での最も近い点を計算
	const Vector3 closestPointOnCapsuleAxis = CalcClosestPointOnSegment(capsuleStart, capsuleEnd, sphere.GetPosition());

	// 球の中心と最も近い点との距離の二乗を計算
	const float sqrDistance = Vector3::DistanceSquared(sphere.GetPosition(), closestPointOnCapsuleAxis);

	// 衝突判定
	const float totalRadius = sphere.GetRadius() + capsule.GetRadius();
	const float totalRadiusSq = totalRadius * totalRadius;

	return sqrDistance <= totalRadiusSq;
}

/**
 * @brief 交差点の取得
 * 
 * @param[out] pHitPos	衝突座標
 * @param[in] plane		平面
 * @param[in] segment	線分
 * 
 * @return 衝突したかどうか
 */
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Plane& plane, const Segment& segment)
{
	return IsHit(plane, segment, pHitPos);
}

/**
 * @brief 交差点の取得
 *
 * @param[out] pHitPos	衝突座標
 * @param[in] triangle	三角形
 * @param[in] segment	線分
 *
 * @return 衝突したかどうか
 */
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Triangle& triangle, const Segment& segment)
{	
	return IsHit(triangle.GetPlane(), segment, pHitPos);
}

/**
 * @brief 交差点の取得
 *
 * @param[out] pHitPos	衝突座標
 * @param[in] box		矩形
 * @param[in] segment	線分
 *
 * @return 衝突したかどうか
 */
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Box2D& box, const Segment& segment)
{
	return IsHit(box.GetPlane(), segment, pHitPos);
}

/**
 * @brief 交点の取得
 * 
 * @param[in] pHitPos	衝突座標
 * @param[out] sphere	球
 * @param[in] segment	線分
 * 
 * @return 衝突したかどうか
 */
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const Sphere& sphere, const Segment& segment)
{
	return IsHit(segment, sphere, pHitPos );
}

/**
 * @brief 交差点の取得
 * 
 * @param[in] pHitPos	衝突座標
 * @param[in] box		ボックス
 * @param[in] segment	線分
 * @return 
 */
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const AABB& aabb, const Segment& segment)
{
	using namespace SimpleMath;

	// EPS for "parallel" checks
	const double EPS = 1e-8;

	// compute aabb min/max (assume box.GetExtend() is full size)
	Vector3 aabbMin = aabb.GetCenter() - aabb.GetExtend() * 0.5f;
	Vector3 aabbMax = aabb.GetCenter() + aabb.GetExtend() * 0.5f;

	// ---- get segment endpoints robustly ----
	// We MUST use start and end explicitly.
	// If your Segment API provides Start and End getters, use them.
	// If not, assume segment.GetPosition() == start and segment.GetVector() == (end - start).
	Vector3 start = segment.GetPosition();
	Vector3 vec = segment.GetVector();
	Vector3 end = start + vec; // safe regardless: if GetVector() already is (end-start) then end is correct.

	// direction from start to end and segment parameter t in [0,1]
	Vector3 d = end - start;

	// Use double for param math to reduce numerical errors
	double tmin = 0.0;
	double tmax = 1.0;

	// For each axis (x=0,y=1,z=2)
	for (int i = 0; i < 3; ++i)
	{
		double s = (&start.x)[i];
		double di = (&d.x)[i];
		double mn = (&aabbMin.x)[i];
		double mx = (&aabbMax.x)[i];

		if (std::abs(di) < EPS)
		{
			// segment is parallel to slab planes for this axis:
			// if start coordinate not within slab, no intersection.
			if (s < mn - EPS || s > mx + EPS) return false;
			// else this axis imposes no constraint on t (all t allowed)
		}
		else
		{
			double inv = 1.0 / di;
			double t1 = (mn - s) * inv;
			double t2 = (mx - s) * inv;
			if (t1 > t2) std::swap(t1, t2);
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			if (tmin > tmax) 
				return false; // no overlap
		}
	}

	// Final check: intersection interval must overlap the segment parameter range [0,1]
	if (tmax < 0.0 || tmin > 1.0) return false;

	// 衝突が成立した場合、交点を計算してポインタに格納
	if (pHitPos)
	{
		*pHitPos = start + vec * static_cast<float>(tmin);
	}
	// すべての軸で重なりが確認できたので衝突している
	return true;
}

/**
 * @brief カプセルとAABBの貫通点を求める
 * 
 * @param[in] capsule カプセル
 * @param[in] aabb AABB
 * @param[out] pHitPos 貫通点（結果を格納）
 * @return 衝突し、貫通点が見つかった場合はtrue
 */
bool GetIntersectionPoint(DirectX::SimpleMath::Vector3* pHitPos, const AABB& aabb, const Capsule& capsule)
{
	if (IsHit(capsule, aabb) == false) return false;

	if (pHitPos == nullptr) return true;

	using namespace SimpleMath;
	// カプセルの軸（線分）の始点と終点を計算
	Vector3 capsuleAxisStart = capsule.GetPosition() - capsule.GetAxis() * capsule.GetLength() / 2.0f;
	Vector3 capsuleAxisEnd = capsule.GetPosition() + capsule.GetAxis() * capsule.GetLength() / 2.0f;

	// AABBの中心に最も近い、カプセルの軸上の点を取得
	Vector3 closestPointOnCapsuleAxis = CalcClosestPointOnSegment(capsuleAxisStart, capsuleAxisEnd, aabb.GetCenter());

	// カプセルの軸上の最近点に最も近い、AABB上の点を取得
	Vector3 closestPointOnAABB = CalcClosestPointOnAABB(aabb, closestPointOnCapsuleAxis);

	// 押し出しベクトルを計算（AABBからカプセル軸への方向）
	Vector3 separationVector = closestPointOnCapsuleAxis - closestPointOnAABB;
	float distance = separationVector.Length();

	// 貫通している距離を計算
	float penetrationDepth = capsule.GetRadius() - distance;

	// 押し出しベクトルを正規化し、貫通深度を掛ける
	if (distance > 1e-6f) {
		separationVector.Normalize();
		// 貫通点は、AABB上の最も近い点から押し出しベクトルを逆方向に伸ばした位置
		*pHitPos = closestPointOnAABB + separationVector * penetrationDepth;
	}
	else {
		// 2つの点が重なっている場合、貫通点をAABBの中心に設定
		*pHitPos = aabb.GetCenter();
	}

	return true;
}

/**
 * @brief 表面上の交差点の取得
 *
 * @param[in] capsule カプセル
 * @param[in] aabb    AABB
 * @param[out] outEnter 入り点
 * @param[out] outExit  出点
 * 
 * @return true = 衝突あり, false = 衝突なし
 */
bool GetIntersectionPointOnSurface(const AABB& aabb, const Capsule& capsule, DirectX::SimpleMath::Vector3* pOutEnter, DirectX::SimpleMath::Vector3* pOutExit)
{
	using namespace DirectX::SimpleMath;
	// pHitPos が nullptr なら交差の有無だけを判定
	if (IsHit(capsule, aabb) == false)
		return false; // そもそも衝突していなければ false を返す

	if (pOutEnter == nullptr && pOutExit == nullptr)
	{
		return true; // 衝突はしているが、交点位置は不要なら true を返す
	}

	// 最初はレイとの衝突地点を使う


	// 1. カプセルの軸線両端点を取得
	//    m_position を中心に、軸方向 m_axis の長さ m_length / 2 を足し引き
	// カプセルの軸（線分）
	Vector3 start = capsule.GetPosition() - capsule.GetAxis() * capsule.GetLength() * 0.5f;
	Vector3 end = capsule.GetPosition() + capsule.GetAxis() * capsule.GetLength() * 0.5f;

	if (GetIntersectionPoint(pOutEnter, aabb, Segment(start, end, true)))
	{
		return true;
	}

	
	// 2. カプセル半径ぶん拡張したAABBを作る
	//    これにより、線分のみで判定してもカプセル体積分を考慮できる
	Vector3 aabbMin = aabb.GetCenter() - aabb.GetExtend() * 0.5f - Vector3(capsule.GetRadius());
	Vector3 aabbMax = aabb.GetCenter() + aabb.GetExtend() * 0.5f + Vector3(capsule.GetRadius());



	Vector3 dir = end - start;

	// カプセル半径ぶん拡張したAABB
	Vector3 r(capsule.GetRadius());
	Vector3 minE = aabbMin - r;
	Vector3 maxE = aabbMax + r;

	float tmin = 0.0f;
	float tmax = 1.0f;

	// --- スラブ法で判定 ---
	for (int i = 0; i < 3; ++i)
	{
		float s = (&start.x)[i];
		float d = (&dir.x)[i];

		if (fabs(d) < 1e-6f)
		{
			// 線分がスラブと平行 → 範囲外なら交差なし
			if (s < (&minE.x)[i] || s > (&maxE.x)[i])
				return false;
		}
		else
		{
			float ood = 1.0f / d;
			float t1 = ((&minE.x)[i] - s) * ood;
			float t2 = ((&maxE.x)[i] - s) * ood;
			if (t1 > t2) std::swap(t1, t2);

			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);

			if (tmin > tmax)
				return false; // 交差しない
		}
	}

	// --- 交差あり ---
	if (tmin < 0.0f && tmax < 0.0f)
		return false; // 完全に過去

	// 入り点
	if (pOutEnter)
	{
		if (tmin <= 0.0f)
		{
			*pOutEnter = start; // 始点が内部
		}
		else
		{
			*pOutEnter = start + dir * tmin;
		}
	}

	if (pOutExit)
	{
		// 出点
		*pOutExit = start + dir * tmax;
	}


	return true;
}


/**
 * @brief 表面上の交差点の取得
 *
 * @param[in] capsule カプセル
 * @param[in] sphere    球
 * @param[out] outEnter 入り点
 * @param[out] outExit  出点
 *
 * @return true = 衝突あり, false = 衝突なし
 */
bool GetIntersectionPointOnSurface(const Sphere& sphere, const Capsule& capsule, DirectX::SimpleMath::Vector3* pOutEnter, DirectX::SimpleMath::Vector3* pOutExit)
{
	using namespace SimpleMath;

	// 仮想的なカプセルの半径 = 球の半径 + カプセルの半径
	const float totalRadius = sphere.GetRadius() + capsule.GetRadius();

	// カプセル軸の線分を定義
	const Vector3 capsuleStart = capsule.GetPosition() - capsule.GetAxis() * (capsule.GetLength() * 0.5f);
	const Vector3 capsuleEnd = capsule.GetPosition() + capsule.GetAxis() * (capsule.GetLength() * 0.5f);

	// レイ（球の中心と移動方向）と仮想カプセルの交差を計算
	const Vector3 rayOrigin = sphere.GetPosition();

	// A. レイと「無限円柱」（カプセルの側面）の交差判定
	Vector3 rayDir =  capsuleStart - capsuleEnd ;
	rayDir.Normalize();
	const Vector3 m = rayDir - capsule.GetAxis() * rayDir.Dot( capsule.GetAxis());
	const Vector3 n = (rayOrigin - capsuleStart) - capsule.GetAxis() * (rayOrigin - capsuleStart).Dot(capsule.GetAxis());

	const float a = m.Dot(m);
	const float b = 2.0f * m.Dot( n);
	const float c = n.Dot( n) - totalRadius * totalRadius;

	float t0{}, t1{};
	int nroots = 0;
	std::vector<float> hits;

	if (fabs(a) < 1e-6f) {
		if (fabs(b) >= 1e-6f) {
			t0 = t1 = -c / b;
			nroots = 1;
		}
	}
	else {
		float disc = b * b - 4.0f * a * c;
		if (disc >= 0.0f) {
			float sqrtDisc = sqrtf(disc);
			t0 = (-b - sqrtDisc) / (2.0f * a);
			t1 = (-b + sqrtDisc) / (2.0f * a);
			nroots = (disc > 1e-6f) ? 2 : 1;
		}
	}

	// 円柱範囲内にある交差点を hits に追加
	auto onFiniteCylinder = [&](float t) -> bool {
		Vector3 p = rayOrigin + rayDir * t;
		float s = (p - capsuleStart).Dot(capsule.GetAxis());
		return (s >= 0.0f && s <= capsule.GetLength());
		};

	if (nroots >= 1) {
		if (t0 >= 0.0f && onFiniteCylinder(t0)) hits.push_back(t0);
		if (nroots == 2 && t1 >= 0.0f && onFiniteCylinder(t1)) hits.push_back(t1);
	}

	// B. レイと「端点の半球」（カプセルのキャップ）の交差判定
	// 半球の中心はカプセル軸の始点と終点
	float sphereToStart_t0, sphereToStart_t1;
	nroots = GetSphereIntersection(rayOrigin, rayDir, capsuleStart, totalRadius, &sphereToStart_t0, &sphereToStart_t1);
	if (nroots >= 1) {
		if (sphereToStart_t0 >= 0.0f) hits.push_back(sphereToStart_t0);
		if (nroots == 2 && sphereToStart_t1 >= 0.0f) hits.push_back(sphereToStart_t1);
	}
	float sphereToEnd_t0, sphereToEnd_t1;
	nroots = GetSphereIntersection(rayOrigin, rayDir, capsuleEnd, totalRadius, &sphereToEnd_t0, &sphereToEnd_t1);
	if (nroots >= 1) {
		if (sphereToEnd_t0 >= 0.0f) hits.push_back(sphereToEnd_t0);
		if (nroots == 2 && sphereToEnd_t1 >= 0.0f) hits.push_back(sphereToEnd_t1);
	}

	// 衝突点が見つからなかった、または球が既にカプセル内部にいる
	if (hits.empty()) {
		const Vector3 closestPointOnAxis = capsuleStart + capsule.GetAxis() * std::max(0.0f, std::min(capsule.GetLength(), (sphere.GetPosition() - capsuleStart).Dot( capsule.GetAxis())));
		const float distSq = Vector3::DistanceSquared(sphere.GetPosition(), closestPointOnAxis);
		const float totalRadiusSq = totalRadius * totalRadius;
		if (distSq <= totalRadiusSq) {
			// 始点がすでに内部にあるケース
			if (pOutEnter) *pOutEnter = sphere.GetPosition();
			return true;
		}
		return false;
	}

	// C. 交差パラメータ（t）を昇順にソートし、入りと出を決定
	std::sort(hits.begin(), hits.end());

	if (pOutEnter) {
		*pOutEnter = rayOrigin + rayDir * hits.front();
	}
	if (pOutExit && hits.size() >= 2) {
		*pOutExit = rayOrigin + rayDir * hits.back();
	}

	return true;
}

inline int GetSphereIntersection(const DirectX::SimpleMath::Vector3& rayOrigin, const DirectX::SimpleMath::Vector3& rayDir, const DirectX::SimpleMath::Vector3& sphereCenter, float sphereRadius, float* pT0, float* pT1)
{
	using namespace SimpleMath;
	const Vector3 oc = rayOrigin - sphereCenter;
	const float a = rayDir.Dot(rayDir);
	const float b = 2.0f * oc.Dot( rayDir);
	const float c = oc.Dot( oc) - sphereRadius * sphereRadius;
	const float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0) {
		return 0;
	}
	else if (discriminant == 0) {
		*pT0 = *pT1 = -b / (2.0f * a);
		return 1;
	}
	else {
		const float sqrtDisc = sqrtf(discriminant);
		*pT0 = (-b - sqrtDisc) / (2.0f * a);
		*pT1 = (-b + sqrtDisc) / (2.0f * a);
		return 2;
	}
}

/**
 * @brief 押し出しの計算
 * 
 * @param[in] plane		平面
 * @param[in] sphere	球
 * @return 
 */
DirectX::SimpleMath::Vector3 CalcOverlap(const Plane& plane, const Sphere& sphere)
{
	//using namespace DirectX::SimpleMath;
	//

	//Vector3 normal = Vector3(plane.GetVector4().x, plane.GetVector4().y, plane.GetVector4().z); // 平面の法線
	//Vector3 center = sphere.GetPosition();

	//// 中心点から平面への signed 距離
	//float distance = plane.CalcLength(center); // 負なら中にめり込んでる

	//// めり込み量（正数でなければ押し出さない）
	//float penetration = sphere.GetRadius() - distance;
	//if (penetration <= 0.0f)
	//	return Vector3::Zero;

	//float w = (plane.GetVector4().x * sphere.GetPosition().x) + (plane.GetVector4().y * sphere.GetPosition().y) + (plane.GetVector4().z * sphere.GetPosition().z) + plane.GetVector4().w;

	//if (w < 0.0f)
	//	normal = -normal;

	//normal.Normalize();

	//// 押し出しベクトル = 法線 × めり込み量
	//return normal * penetration;

	using namespace DirectX::SimpleMath;

	Vector3 normal = Vector3(plane.GetVector4().x, plane.GetVector4().y, plane.GetVector4().z); // 平面の法線
	Vector3 center = sphere.GetPosition();

	// 中心点から平面への signed 距離
	float signedDistance = plane.CalcLength(center); // 負なら中にめり込んでる

	// めり込み量（正数でなければ押し出さない）
	float penetration = sphere.GetRadius() - signedDistance;
	if (penetration <= 0.0f)
		return Vector3::Zero;



	// 法線の向きを調整（球が平面の裏にいたら逆に）
	float dot = normal.Dot(center) + plane.GetVector4().w;
	if (dot < 0.0f)
		normal = -normal;

	// 最近接点 = 中心 - 法線 * 距離
	Vector3 closestPoint = center - normal * signedDistance;

	// 押し出し方向 = 球の中心から最近接点へのベクトル
	Vector3 pushDirection = center - closestPoint;
	pushDirection.Normalize();

	// 押し出しベクトル = 押し出し方向 * めり込み量
	return pushDirection * penetration;
	// 押し出しベクトル = 平面の法線 * めり込み量
//	return normal * penetration;
}

/**
 * @brief 押し出し計算
 * 
 * @param[in] triangle	三角形
 * @param[in] sphere	球
 * @return 
 */
DirectX::SimpleMath::Vector3 CalcOverlap(const Triangle& triangle, const Sphere& sphere)
{
	using namespace DirectX::SimpleMath;

	Vector3 center = sphere.GetPosition();

	// 三角形に対する最近接点を求める
	Vector3 closest = Triangle::CalcClosestPoint(center, triangle);

	Vector3 toCenter = center - closest;
	float distSqr = toCenter.LengthSquared();
	float radius = sphere.GetRadius();

	// めり込んでいなければ押し出し不要
	if (distSqr >= radius * radius)
		return Vector3::Zero;

	float dist = std::sqrt(distSqr);
	float penetration = radius - dist;

	// 距離が0だと正規化できないので適当な法線にする

	Vector3 direction;
	if (dist > 0.0001f)
	{
		direction = toCenter;
		direction.Normalize();
	}
	else
	{
		Vector4 planeNormal = triangle.GetPlane().GetVector4();
		direction = Vector3(planeNormal.x, planeNormal.y, planeNormal.z); // 三角形の法線方向
	}


	return direction * penetration;
}

/**
 * @brief 長方形と球の押し出し処理
 * 
 * @param[in] box2D		長方形
 * @param[in] sphere	球
 * @return 
 */
DirectX::SimpleMath::Vector3 CalcOverlap(const Box2D& box2D, const Sphere& sphere)
{
	// 衝突した三角形を取得
	const Triangle* pHitTri = (box2D.GetTriangle()[0]->CheckHit(sphere)) ? box2D.GetTriangle()[0].get() : box2D.GetTriangle()[1].get();

	// 三角形の成分の取得
	const auto& triPos = pHitTri->GetPosition();

	// 三角形の辺が球を貫通するか
	if (IsHit(Segment(triPos[0], triPos[1], true), sphere))
		return CalcOverlap(*pHitTri, sphere);

	if (IsHit(Segment(triPos[1], triPos[2], true), sphere))
		return CalcOverlap(*pHitTri, sphere);

	if (IsHit(Segment(triPos[2], triPos[0], true), sphere))
		return CalcOverlap(*pHitTri, sphere);

	// 三角形の平面との押し出し
	return CalcOverlap(pHitTri->GetPlane(), sphere);

	//return CalcOverlap(*pHitTri, sphere);
}

/**
 * @brief 押し出し量の算出
 * 
 * @param[in] plane
 * @param[in] sphere
 * @return 
 */
float CalcOverlapValue(const Plane& plane, const Sphere& sphere)
{
	using namespace DirectX::SimpleMath;


	Vector3 normal = Vector3(plane.GetVector4().x, plane.GetVector4().y, plane.GetVector4().z); // 平面の法線
	Vector3 center = sphere.GetPosition();

	// 中心点から平面への signed 距離
	float distance = plane.CalcLength(center); // 負なら中にめり込んでる

	// めり込み量（正数でなければ押し出さない）
	float penetration = sphere.GetRadius() - distance;
	if (penetration <= 0.0f)
		penetration = 0.0f;
		
		return penetration;
}

DirectX::SimpleMath::Vector3 CalcOverlap(const Sphere& sphereA, const Sphere& sphereB)
{
	using namespace DirectX::SimpleMath;

	Vector3 posA = sphereA.GetPosition();
	Vector3 posB = sphereB.GetPosition();
	float radiusA = sphereA.GetRadius();
	float radiusB = sphereB.GetRadius();

	Vector3 dir = posB - posA;
	float distSq = dir.LengthSquared();
	float radiusSum = radiusA + radiusB;

	// 同一点や極端に近い場合のゼロ除算防止
	if (distSq < 1e-8f)
		return Vector3::Zero;

	float dist = std::sqrt(distSq);

	// めり込んでいなければ押し出し不要
	if (dist >= radiusSum)
		return Vector3::Zero;

	// 押し出し量
	float penetration = radiusSum - dist;
	dir.Normalize();

	// BをAから押し出すベクトル
	return dir * penetration;
}

/**
 * @brief 押し出しの算出
 * 
 * @param[in] box  ボックス
 * @param[in] sphere　球
 * @return 
 */
DirectX::SimpleMath::Vector3 CalcOverlap(const AABB& box, const Sphere& sphere)
{
	using namespace DirectX::SimpleMath;

	const Vector3 spherePos = sphere.GetPosition();
	const float sphereRadius = sphere.GetRadius();

	// 1. AABB上の球の中心に最も近い点を計算
	// CalcClosestPointOnSurface は通常不要。CalcClosestPointのみで十分。
	Vector3 closestPoint = box.CalcClosestPoint(spherePos);

	// 2. 球の中心と最も近い点との距離ベクトルを計算
	Vector3 displacementVector = spherePos - closestPoint;
	float distance = displacementVector.Length();

	// 3. めり込み判定
	float overlapDistance = sphereRadius - distance;

	// めり込んでいない、またはAABBの表面と接している場合は押し出し不要
	if (overlapDistance <= 0.0f) {
		return Vector3::Zero;
	}

	// 4. めり込みベクトルの計算
	// 球の中心がAABBの内部にある場合、closestPointは球の中心と一致する
	// その結果、distanceは0になり、normalVectorが計算できない
	if (distance < 1e-6f) {
		// この場合、球は完全にAABBの内部にある
		// 押し出し方向は、球の現在の移動方向の逆、またはAABBの最も近い面から計算する必要がある。
		// ここでは、AABBの中心から球の中心へのベクトルを方向として使用
		Vector3 direction = spherePos - box.GetCenter();
		if (direction.LengthSquared() < 1e-6f) {
			// 球がAABBの中心と完全に一致する場合のフォールバック
			return Vector3(0.0f, overlapDistance, 0.0f); // 例えばY軸方向に押し出す
		}
		direction.Normalize();
		return overlapDistance * direction;
	}

	// 通常の押し出しベクトルの計算
	displacementVector.Normalize();
	return overlapDistance * displacementVector;

}

/**
 * @brief 指定された方向への、AABBからの押し出し量を算出します。
 * 
 * @param[in] box       判定対象のAABB
 * @param[in] sphere    判定対象の球
 * @param[in] overlapDirection 押し出し方向 (単位ベクトル)
 * 
 * @return 押し出しベクトル (Vector3)。球がAABBの外部にある場合は0を返します。
 */
DirectX::SimpleMath::Vector3 CalcOverlap(const AABB& aabb, const Sphere& sphere, const DirectX::SimpleMath::Vector3& overlapDirection)
{
	using namespace DirectX::SimpleMath;

	const Vector3 spherePos = sphere.GetPosition();
	const float sphereRadius = sphere.GetRadius();

	// 1. 衝突していなければ押し出し不要
	if (!aabb.CheckHit(sphere)) {
		return Vector3::Zero;
	}

	// 2. 球の半径ぶん膨張したAABBを用意
	Vector3 aabbMin = aabb.GetCenter() - aabb.GetExtend() * 0.5f - Vector3(sphereRadius);
	Vector3 aabbMax = aabb.GetCenter() + aabb.GetExtend() * 0.5f + Vector3(sphereRadius);

	// 3. スラブ法でレイ vs AABB を計算
	float tmin = -FLT_MAX;
	float tmax = FLT_MAX;

	for (int i = 0; i < 3; ++i)
	{
		float origin = (&spherePos.x)[i];
		float dir = (&overlapDirection.x)[i];
		float minB = (&aabbMin.x)[i];
		float maxB = (&aabbMax.x)[i];

		if (fabs(dir) < 1e-6f)
		{
			// 方向がこの軸に平行 → 原点が範囲外なら衝突しない
			if (origin < minB || origin > maxB)
				return Vector3::Zero;
		}
		else
		{
			float ood = 1.0f / dir;
			float t1 = (minB - origin) * ood;
			float t2 = (maxB - origin) * ood;
			if (t1 > t2) std::swap(t1, t2);

			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);

			if (tmin > tmax) return Vector3::Zero; // 外れている
		}
	}

	// 4. tmin が正ならその距離だけ押し出せば外に出る
	if (tmin >= 0.0f) {
		return overlapDirection * tmin;
	}

	// 5. 始点が内部にいる場合（tmin < 0）
	// → すでに内部から出る必要があるので出口側 tmax を使う
	return overlapDirection * tmax;
}

DirectX::SimpleMath::Vector3 CalcOverlapNormal(const AABB& box, const Sphere& sphere)
{
	using namespace DirectX::SimpleMath;

	// AABBの最小点と最大点を計算
	const Vector3 aabbMin = box.GetCenter() - box.GetExtend() / 2.0f;
	const Vector3 aabbMax = box.GetCenter() + box.GetExtend() / 2.0f;

	const Vector3 spherePos = sphere.GetPosition();

	// 球の中心とAABBの各面の距離を計算
	// 各軸方向で、球の中心と最も近いAABBの面との距離を求める
	float distSqX = 0.0f;
	if (spherePos.x < aabbMin.x) {
		distSqX = (spherePos.x - aabbMin.x) * (spherePos.x - aabbMin.x);
	}
	else if (spherePos.x > aabbMax.x) {
		distSqX = (spherePos.x - aabbMax.x) * (spherePos.x - aabbMax.x);
	}

	float distSqY = 0.0f;
	if (spherePos.y < aabbMin.y) {
		distSqY = (spherePos.y - aabbMin.y) * (spherePos.y - aabbMin.y);
	}
	else if (spherePos.y > aabbMax.y) {
		distSqY = (spherePos.y - aabbMax.y) * (spherePos.y - aabbMax.y);
	}

	float distSqZ = 0.0f;
	if (spherePos.z < aabbMin.z) {
		distSqZ = (spherePos.z - aabbMin.z) * (spherePos.z - aabbMin.z);
	}
	else if (spherePos.z > aabbMax.z) {
		distSqZ = (spherePos.z - aabbMax.z) * (spherePos.z - aabbMax.z);
	}

	// 衝突していない場合
	if (distSqX + distSqY + distSqZ >= sphere.GetRadius() * sphere.GetRadius()) {
		return Vector3::Zero;
	}

	// 最もめり込んでいる軸を特定
	if (distSqX > distSqY && distSqX > distSqZ) {
		float overlap = sphere.GetRadius() - std::sqrt(distSqX);
		Vector3 normal = (spherePos.x < aabbMin.x) ? Vector3(-1, 0, 0) : Vector3(1, 0, 0);
		return normal * overlap;
	}
	else if (distSqY > distSqZ) {
		float overlap = sphere.GetRadius() - std::sqrt(distSqY);
		Vector3 normal = (spherePos.y < aabbMin.y) ? Vector3(0, -1, 0) : Vector3(0, 1, 0);
		return normal * overlap;
	}
	else {
		float overlap = sphere.GetRadius() - std::sqrt(distSqZ);
		Vector3 normal = (spherePos.z < aabbMin.z) ? Vector3(0, 0, -1) : Vector3(0, 0, 1);
		return normal * overlap;
	}
}
/**
 * @brief 接触座標の算出
 * 
 * @param[in] triangle	三角形
 * @param[in] sphere	球
 * 
 * @return 接触座標
 */
SimpleMath::Vector3 CalcContactPoint(const Triangle& triangle, const Sphere& sphere)
{
	// **** 事前準備 ****
	// 三角形の三点座標の取得
	std::vector<SimpleMath::Vector3> triPos = triangle.GetPosition();

	// 三角形の平面情報を取得
	SimpleMath::Vector4 planeData = triangle.GetPlane().GetVector4();

	// 球の中心座標
	SimpleMath::Vector3 spherePos = sphere.GetPosition();

	// 平面の法線ベクトル
	SimpleMath::Vector3 planeNormal(planeData.x, planeData.y, planeData.z);

	float radius = sphere.GetRadius();


	// 球の中心と平面の距離を計算
	float distanceToPlane = planeNormal.Dot( spherePos) + planeData.w;

	// 球が法線の反対側にある場合、法線の向きを反転
	if (distanceToPlane < 0.0f) {
		planeNormal = -planeNormal;
	}
	else
		planeNormal = planeNormal;


	// 球の中心を平面上に投影
	SimpleMath::Vector3 projectedPoint = spherePos - planeNormal * std::abs(distanceToPlane);

	// 投影点が三角形の内部にあるなら、そのまま使う
	if (triangle.CheckInner(projectedPoint)) {
		if (fabs(distanceToPlane) <= radius) {
			return projectedPoint; // **内部の接触点**
		}
	}

	// 投影点が外なら、三角形のエッジ上の最近接点を探す
	SimpleMath::Vector3 closestPoint = triangle.GetClosestPointVertex(projectedPoint);

	// 球の中心から最近接点へのベクトル
	SimpleMath::Vector3 dirToClosest = closestPoint - spherePos;
	float lengthSq = dirToClosest.LengthSquared();

	// **球の内部にいる場合、最も近い接触点を求める**
	if (lengthSq < radius * radius) {
		// 球の中心から最近接点へ向かう線分を作成
		float length = sqrt(lengthSq);

		if (length > 0.0f) {
			dirToClosest /= length; // 正規化
			return spherePos + dirToClosest * (radius - (radius - length)); // 内部の接触点
		}

		return spherePos; // 球の中心そのもの
	}

	// 通常の処理（球の表面の接触点）
	dirToClosest.Normalize();
	return spherePos + dirToClosest * radius;
}

/**
 * @brief 線分と点との最近点を求める
 * 
 * @param[in] start　始点
 * @param[in] end	 終点
 * @param[in] point	 点
 * 
 * @return 最近点
 */
DirectX::SimpleMath::Vector3 CalcClosestPointOnSegment(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, const DirectX::SimpleMath::Vector3& point)
{
	SimpleMath::Vector3 edge = end - start;
	float t = (point - start).Dot(edge) / edge.Dot(edge);
	t = std::max(0.0f, std::min(1.0f, t)); // tを0-1の範囲にクランプ
	return start + t * edge;
}

/**
 * @brief 点とAABBの最近点を求める
 * 
 * @param[in] aabb AABB
 * @param[in] point 点
 * 
 * @return 最近点
 */
DirectX::SimpleMath::Vector3 CalcClosestPointOnAABB(const AABB& aabb, const DirectX::SimpleMath::Vector3& point)
{
	using namespace SimpleMath;


	Vector3 closestPoint;
	// 自身のAABBの最小点と最大点を計算
	DirectX::SimpleMath::Vector3 aabbMin =  aabb.GetCenter() - aabb.GetExtend()/ 2.0f;
	DirectX::SimpleMath::Vector3 aabbMax = aabb.GetCenter() + aabb.GetExtend() / 2.0f;

	closestPoint.x = std::max(aabbMin.x, std::min(point.x, aabbMax.x));
	closestPoint.y = std::max(aabbMin.y, std::min(point.y, aabbMax.y));
	closestPoint.z = std::max(aabbMin.z, std::min(point.z, aabbMax.z));

	return closestPoint;
}

/**
 * @brief 面上の最近点を算出
 *
 * @param[in] point　座標
 *
 * @return 最近点
 */
DirectX::SimpleMath::Vector3 CalcClosestPointOnAABBSurface(const AABB& aabb, const DirectX::SimpleMath::Vector3& point)
{
	using namespace DirectX::SimpleMath;

	const Vector3 aabbMin = aabb.GetCenter() - aabb.GetExtend() / 2.0f;
	const Vector3 aabbMax = aabb.GetCenter() + aabb.GetExtend() / 2.0f;

	// 1. 点がAABBの内部にあるかを判定
	bool isInside = (point.x >= aabbMin.x && point.x <= aabbMax.x) &&
		(point.y >= aabbMin.y && point.y <= aabbMax.y) &&
		(point.z >= aabbMin.z && point.z <= aabbMax.z);

	if (isInside)
	{
		// 2. 内部にある場合、各面までの距離を計算
		Vector3 distances = Vector3::Zero;
		distances.x = std::min(std::abs(point.x - aabbMin.x), std::abs(point.x - aabbMax.x));
		distances.y = std::min(std::abs(point.y - aabbMin.y), std::abs(point.y - aabbMax.y));
		distances.z = std::min(std::abs(point.z - aabbMin.z), std::abs(point.z - aabbMax.z));

		// 3. 最も近い面を特定し、その面に点をクランプ
		if (distances.x < distances.y && distances.x < distances.z)
		{
			// X軸の面に最も近い
			return Vector3((point.x < aabb.GetCenter().x) ? aabbMin.x : aabbMax.x, point.y, point.z);
		}
		else if (distances.y < distances.z)
		{
			// Y軸の面に最も近い
			return Vector3(point.x, (point.y < aabb.GetCenter().y) ? aabbMin.y : aabbMax.y, point.z);
		}
		else
		{
			// Z軸の面に最も近い
			return Vector3(point.x, point.y, (point.z < aabb.GetCenter().z) ? aabbMin.z : aabbMax.z);
		}
	}
	else
	{
		// 外部にある場合はクランプで内部最近点を求める
		Vector3 closestPoint;
		closestPoint.x = std::max(aabbMin.x, std::min(point.x, aabbMax.x));
		closestPoint.y = std::max(aabbMin.y, std::min(point.y, aabbMax.y));
		closestPoint.z = std::max(aabbMin.z, std::min(point.z, aabbMax.z));

		// もし内部点になってしまったら → 一番近い面に押し出す
		if ((closestPoint.x > aabbMin.x && closestPoint.x < aabbMax.x) &&
			(closestPoint.y > aabbMin.y && closestPoint.y < aabbMax.y) &&
			(closestPoint.z > aabbMin.z && closestPoint.z < aabbMax.z))
		{
			Vector3 distances;
			distances.x = std::min(std::abs(closestPoint.x - aabbMin.x), std::abs(closestPoint.x - aabbMax.x));
			distances.y = std::min(std::abs(closestPoint.y - aabbMin.y), std::abs(closestPoint.y - aabbMax.y));
			distances.z = std::min(std::abs(closestPoint.z - aabbMin.z), std::abs(closestPoint.z - aabbMax.z));

			if (distances.x < distances.y && distances.x < distances.z)
				closestPoint.x = (closestPoint.x < aabb.GetCenter().x) ? aabbMin.x : aabbMax.x;
			else if (distances.y < distances.z)
				closestPoint.y = (closestPoint.y < aabb.GetCenter().y) ? aabbMin.y : aabbMax.y;
			else
				closestPoint.z = (closestPoint.z < aabb.GetCenter().z) ? aabbMin.z : aabbMax.z;
		}

		return closestPoint;
	}
	
}

/**
 * @brief コンストラクタ　頂点を指定
 * 
 * @param[in] posA	頂点A
 * @param[in] posB	頂点B
 * @param[in] posC	頂点C
 * @param[in] posD	頂点D
 */
Box2D::Box2D(
	const DirectX::SimpleMath::Vector3& posA, 
	const DirectX::SimpleMath::Vector3& posB,
	const DirectX::SimpleMath::Vector3& posC, 
	const DirectX::SimpleMath::Vector3& posD,
	DX::DeviceResources* pDeviceResources)
	: m_triangles{2}
{
	m_triangles[0] = std::make_unique<Triangle>(posA, posB, posC, pDeviceResources);
	m_triangles[1] = std::make_unique<Triangle>(posC, posD, posA, pDeviceResources);

	Set(posA, posB, posC, posD);
}

/**
 * @brief 任意点が四角形に存在するかどうか
 * 
 * @param[in] pos
 * 
 * @return true 存在している
 */
bool Box2D::CheckInner(const DirectX::SimpleMath::Vector3& pos) const
{
	return (m_triangles[0]->CheckInner(pos) || m_triangles[1]->CheckInner(pos));
}

/**
 * @brief 球との衝突判定
 * 
 * @param[in] sphere
 * 
 * @return true 衝突している
 */
bool Box2D::CheckHit(const Sphere& sphere) const
{
	return IsHit(*this, sphere);
}

/**
 * @brief 線分との衝突判定
 * 
 * @param[in] segment
 * 
 * @return true 衝突している
 */
bool Box2D::CheckHit(const Segment& segment) const
{
	return IsHit(*this, segment);
}

/**
 * @brief コライダーの種類の取得
 * 
 * @return コライダーの種類
 */
ColliderType Box2D::GetColliderType() const
{
	return ColliderType::BOX2D;
}



/**
 * @brief 頂点を設定
 * 
 * @param[in] posA	頂点A
 * @param[in] posB	頂点B
 * @param[in] posC	頂点C
 * @param[in] posD	頂点D
 */
void Box2D::Set(
	const DirectX::SimpleMath::Vector3& posA, 
	const DirectX::SimpleMath::Vector3& posB, 
	const DirectX::SimpleMath::Vector3& posC,
	const DirectX::SimpleMath::Vector3& posD)
{
	m_triangles[0]->Set(posA, posB, posC);
	m_triangles[1]->Set(posC, posD, posA);

}

/**
 * @brief 三角形の取得
 * 
 * @return 三角形
 */
const std::vector<std::unique_ptr<Triangle>>& Box2D::GetTriangle() const
{

	return m_triangles;
}

/**
 * @brief 平面情報の取得
 * 
 * @return 
 */
const Plane& Box2D::GetPlane() const
{
	return m_triangles[0]->GetPlane();
}

void Box2D::Rotate(DirectX::SimpleMath::Vector3 rotate)
{
	m_triangles[0]->Rotate(rotate);
	m_triangles[1]->Rotate(rotate);
}

/**
 * @brief 描画処理
 * 
 */
void Box2D::Draw(const SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	for (auto& triangle : m_triangles)
	{
		triangle->Draw(view, proj);
	}
}

/**
 * @brief コンストラクタ
 * 
 * @param[in] center
 * @param[in] scale
 */
AABB::AABB(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& extend)
	: m_center{ center }
	, m_extend{ extend }
{
}



bool AABB::CheckHit(const Sphere& sphere) const
{
	return IsHit(*this, sphere);
}

bool AABB::CheckHit(const AABB& box) const
{
	return IsHit(*this, box);
}

bool AABB::CheckHit(const Segment& segment) const
{
	return IsHit(*this, segment);
}

bool AABB::CheckHit(const Cylinder& cylinder) const
{
	return IsHit(cylinder, *this);
}

bool AABB::CheckHit(const Capsule& capsule) const
{
	return IsHit(capsule, *this);
}

ColliderType AABB::GetColliderType() const
{
	return ColliderType::AABB;
}

/**
 * @brief 中心座標の設定
 * 
 * @param[in] center　中心座標
 */
void AABB::SetCenter(const DirectX::SimpleMath::Vector3& center)
{
	m_center = center;
}

/**
 * @brief 拡張の設定
 * 
 * @param[in] extend　拡張
 */
void AABB::SetExtend(const DirectX::SimpleMath::Vector3& extend)
{
	m_extend = extend;
}

DirectX::SimpleMath::Vector3 AABB::GetCenter() const
{
	return m_center;
}

DirectX::SimpleMath::Vector3 AABB::GetExtend() const
{
	return m_extend;
}

/**
 * @brief 最近点の取得
 * 
 * @param[in] targetPosition
 * @return 
 */
DirectX::SimpleMath::Vector3 AABB::CalcClosestPoint(DirectX::SimpleMath::Vector3 targetPosition) const
{

	// AABBの最小点と最大点を計算
	DirectX::SimpleMath::Vector3 minPos = m_center - m_extend / 2.0f;
	DirectX::SimpleMath::Vector3 maxPos = m_center + m_extend / 2.0f;

	// 各軸で最も近い点をクランプ（Clamp）して求める
	SimpleMath::Vector3 closestPoint;

	closestPoint.x = std::max(minPos.x, std::min(targetPosition.x, maxPos.x));
	closestPoint.y = std::max(minPos.y, std::min(targetPosition.y, maxPos.y));
	closestPoint.z = std::max(minPos.z, std::min(targetPosition.z, maxPos.z));

	return closestPoint;
}

DirectX::SimpleMath::Vector4 AABB::GetCollisionNormal(const AABB& aabb, const Sphere& sphere) const
{
	using namespace DirectX::SimpleMath;

	const Vector3 aabbMin = aabb.GetCenter() - aabb.GetExtend() / 2.0f;
	const Vector3 aabbMax = aabb.GetCenter() + aabb.GetExtend() / 2.0f;

	const Vector3 spherePos = sphere.GetPosition();

	// 球の中心からAABBに最も近い点を計算（衝突判定用）
	Vector3 closestPoint;
	closestPoint.x = std::max(aabbMin.x, std::min(spherePos.x, aabbMax.x));
	closestPoint.y = std::max(aabbMin.y, std::min(spherePos.y, aabbMax.y));
	closestPoint.z = std::max(aabbMin.z, std::min(spherePos.z, aabbMax.z));

	// 衝突していない場合はゼロベクトルを返す
	if (Vector3::DistanceSquared(spherePos, closestPoint) >= sphere.GetRadius() * sphere.GetRadius()) {
		return Vector4::Zero;
	}

	// --- ここから正確な法線を取得するロジック ---

	Vector3 normal = Vector3::Zero;
	float minAbsDistance = std::numeric_limits<float>::max();

	// 各面までの距離を計算し、最も近い面を特定
	float distToMinX = std::abs(spherePos.x - aabbMin.x);
	if (distToMinX < minAbsDistance) { minAbsDistance = distToMinX; normal = Vector3(-1, 0, 0); }

	float distToMaxX = std::abs(spherePos.x - aabbMax.x);
	if (distToMaxX < minAbsDistance) { minAbsDistance = distToMaxX; normal = Vector3(1, 0, 0); }

	float distToMinY = std::abs(spherePos.y - aabbMin.y);
	if (distToMinY < minAbsDistance) { minAbsDistance = distToMinY; normal = Vector3(0, -1, 0); }

	float distToMaxY = std::abs(spherePos.y - aabbMax.y);
	if (distToMaxY < minAbsDistance) { minAbsDistance = distToMaxY; normal = Vector3(0, 1, 0); }

	float distToMinZ = std::abs(spherePos.z - aabbMin.z);
	if (distToMinZ < minAbsDistance) { minAbsDistance = distToMinZ; normal = Vector3(0, 0, -1); }

	float distToMaxZ = std::abs(spherePos.z - aabbMax.z);
	if (distToMaxZ < minAbsDistance) { minAbsDistance = distToMaxZ; normal = Vector3(0, 0, 1); }

	// 平面上の点として、最も近い面と球の中心との交点を計算
	Vector3 pointOnPlane = spherePos - normal * (normal.Dot(spherePos - aabb.GetCenter()) - normal.Dot(aabb.GetCenter()));
	// もしくは、もっとシンプルに
	// pointOnPlane = closestPoint; // これだと角や辺のときにずれる

	// 正確な平面上の点を計算する
	Vector3 planePoint = aabb.GetCenter() + normal * (aabb.GetExtend().Dot(SimpleMath::Vector3(std::abs(normal.x), std::abs(normal.y), std::abs(normal.z))) / 2.0f);


	// 平面方程式のDを計算 (D = -N . P)
	float d = -normal.Dot(planePoint);

	return Vector4(normal.x, normal.y, normal.z, d);
}

/**
 * @brief 面上の最近点を算出
 * 
 * @param[in] point　座標
 * 
 * @return 最近点
 */
DirectX::SimpleMath::Vector3 AABB::CalcClosestPointOnSurface(const DirectX::SimpleMath::Vector3& point) const
{
	return CalcClosestPointOnAABBSurface(*this, point);
}

/**
 * @brief 内部に点が位置するかどうか
 * 
 * @param[in] point　判定対象の点
 * 
 * @return true: 点がAABBの内部にある / false: 点がAABBの外部にある
 */
bool AABB::IsPointInside(const SimpleMath::Vector3& point) const
{
	// AABBの中心とサイズから、minとmaxの座標を計算
	SimpleMath::Vector3 aabbMin = GetCenter() - GetExtend();
	SimpleMath::Vector3 aabbMax = GetCenter() + GetExtend();

	// 点の各座標が、AABBのminとmaxの範囲内にあるかチェック
	if (point.x < aabbMin.x || point.x > aabbMax.x)
	{
		return false;
	}
	if (point.y < aabbMin.y || point.y > aabbMax.y)
	{
		return false;
	}
	if (point.z < aabbMin.z || point.z > aabbMax.z)
	{
		return false;
	}

	// すべての軸で範囲内であれば、内部にあると判定
	return true;
}

void AABB::Draw(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	std::unique_ptr<GeometricPrimitive> box = GeometricPrimitive::CreateBox(context, m_extend);

	

	box->Draw(SimpleMath::Matrix::CreateTranslation(m_center), view, proj, Colors::Red, nullptr, true);
}

/**
 * @brief コンストラクタ
 * 
 * @param[in] axis　	軸
 * @param[in] length	長さ
 * @param[in] position	座標
 * @param[in] radius	半径
 */
Cylinder::Cylinder(const DirectX::SimpleMath::Vector3& axis, const float& length, const DirectX::SimpleMath::Vector3& position, const float& radius)
	: m_axis{ axis }
	, m_length	{ length }
	, m_position{ position }
	, m_radius	{ radius }
{
}

Cylinder::Cylinder()
	: m_axis	{}
	, m_length	{}
	, m_position{}
	, m_radius	{}
{
}


bool Cylinder::CheckHit(const AABB& aabb) const
{
	return IsHit(*this, aabb);
}

ColliderType Cylinder::GetColliderType() const
{
	return ColliderType::Cylinder;
}

/**
 * @brief 座標の設定
 * 
 * @param[in] position　座標
 */
void Cylinder::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

/**
 * @brief 軸の設定
 * 
 * @param[in] axis　軸
 */
void Cylinder::SetAxis(const DirectX::SimpleMath::Vector3& axis)
{
	m_axis = axis;
}

/**
 * @brief 長さの設定
 * 
 * @param[in] length　長さ
 */
void Cylinder::SetLength(const float& length)
{
	m_length = length;
}

/**
 * @brief 半径の設定
 * 
 * @param[in] radius　半径
 */
void Cylinder::SetRadius(const float& radius)
{
	m_radius = radius;
}

/**
 * @brief 座標の取得
 * 
 * @return 座標
 */
DirectX::SimpleMath::Vector3 Cylinder::GetPosition() const
{
	return m_position;
}

/**
 * @brief 軸の取得
 * 
 * @return 軸
 */
DirectX::SimpleMath::Vector3 Cylinder::GetAxis() const
{
	return m_axis;
}

/**
 * @brief 長さの取得
 * 
 * @return 長さ
 */
float Cylinder::GetLength() const
{
	return m_length;
}

/**
 * @brief 半径の取得
 * 
 * @return 半径
 */
float Cylinder::GetRadius() const
{
	return m_radius;
}

void Cylinder::Draw(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	using namespace SimpleMath;

		std::unique_ptr<GeometricPrimitive> cylinder = GeometricPrimitive::CreateCylinder(context, m_length, m_radius * 2.0f);
	// 筒の向きを表す軸ベクトル
	// このベクトルが向いてほしい方向です
	DirectX::SimpleMath::Vector3 axis = m_axis; 

	// 入力ベクトルが正規化されていることを確認
	axis.Normalize();

	// モデルのデフォルトの向き（Y軸）
	const DirectX::SimpleMath::Vector3 defaultUp = DirectX::SimpleMath::Vector3::Up;

	// 1. 回転軸を計算（外積）
	// defaultUpベクトルをaxisへと回転させるための軸を求める
	DirectX::SimpleMath::Vector3 rotationAxis = defaultUp.Cross(axis);

	// 2. 回転角を計算（内積とacos）
	// defaultUpとaxisがなす角度を求める
	float dot = defaultUp.Dot(axis);
	// 浮動小数点誤差を考慮して値をクランプする
	dot = std::min(1.0f, std::max(-1.0f, dot));
	float angle = std::acosf(dot);

	// 3. クォータニオンを生成
	// 回転軸と回転角を使ってクォータニオンを作成
	DirectX::SimpleMath::Quaternion quaternion = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(rotationAxis, angle);

	// ----------------------------------------------------
	// 特殊ケース：2つのベクトルがほぼ平行な場合
	// 外積がゼロになり、回転軸を計算できないため、個別に処理する
	// ----------------------------------------------------
	if (rotationAxis.LengthSquared() < 1e-6f) {
		if (dot > 0.999f) {
			// 同じ方向を向いている場合、回転は不要
			quaternion = DirectX::SimpleMath::Quaternion::Identity;
		} else {
			// 正反対の方向を向いている場合、任意の軸周りに180度回転
			quaternion = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Right, DirectX::XM_PI);
		}
	}


	Matrix world = Matrix::Identity;
	world *= Matrix::CreateTranslation(SimpleMath::Vector3(0.0f, m_length / 2.0f, 0.0f));

	world *= Matrix::CreateFromQuaternion(quaternion);
	world *= Matrix::CreateTranslation(m_position  + (-axis * (m_length / 2.0f)  ));

	cylinder->Draw(world, view, proj, Colors::Red);
}


/**
 * @brief コンストラクタ
 *
 * @param[in] axis　	軸
 * @param[in] length	長さ
 * @param[in] position	座標
 * @param[in] radius	半径
 */
Capsule::Capsule(const DirectX::SimpleMath::Vector3& axis, const float& length, const DirectX::SimpleMath::Vector3& position, const float& radius)
	: m_axis{ axis }
	, m_length{ length }
	, m_position{ position }
	, m_radius{ radius }
{
}

Capsule::Capsule()
	: m_axis{}
	, m_length{}
	, m_position{}
	, m_radius{}
{
}


bool Capsule::CheckHit(const AABB& aabb) const
{
	return IsHit(*this, aabb);
}

bool Capsule::CheckHit(const Sphere& sphere) const
{
	return IsHit(*this, sphere);
}

ColliderType Capsule::GetColliderType() const
{
	return ColliderType::Capsule;
}

/**
 * @brief 座標の設定
 *
 * @param[in] position　座標
 */
void Capsule::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

/**
 * @brief 軸の設定
 *
 * @param[in] axis　軸
 */
void Capsule::SetAxis(const DirectX::SimpleMath::Vector3& axis)
{
	m_axis = axis;
}

/**
 * @brief 長さの設定
 *
 * @param[in] length　長さ
 */
void Capsule::SetLength(const float& length)
{
	m_length = length;
}

/**
 * @brief 半径の設定
 *
 * @param[in] radius　半径
 */
void Capsule::SetRadius(const float& radius)
{
	m_radius = radius;
}

/**
 * @brief 座標の取得
 *
 * @return 座標
 */
DirectX::SimpleMath::Vector3 Capsule::GetPosition() const
{
	return m_position;
}

/**
 * @brief 軸の取得
 *
 * @return 軸
 */
DirectX::SimpleMath::Vector3 Capsule::GetAxis() const
{
	return m_axis;
}

/**
 * @brief 長さの取得
 *
 * @return 長さ
 */
float Capsule::GetLength() const
{
	return m_length;
}

/**
 * @brief 半径の取得
 *
 * @return 半径
 */
float Capsule::GetRadius() const
{
	return m_radius;
}

void Capsule::Draw(ID3D11DeviceContext1* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{

	using namespace SimpleMath;

	std::unique_ptr<GeometricPrimitive> cylinder = GeometricPrimitive::CreateCylinder(context, m_length, m_radius * 2.0f);
	std::unique_ptr<GeometricPrimitive> sphere = GeometricPrimitive::CreateSphere(context, m_radius * 2.0f);
	// 筒の向きを表す軸ベクトル
	// このベクトルが向いてほしい方向です
	DirectX::SimpleMath::Vector3 axis = m_axis;

	// 入力ベクトルが正規化されていることを確認
	axis.Normalize();

	// モデルのデフォルトの向き（Y軸）
	const DirectX::SimpleMath::Vector3 defaultUp = DirectX::SimpleMath::Vector3::Up;

	// 1. 回転軸を計算（外積）
	// defaultUpベクトルをaxisへと回転させるための軸を求める
	DirectX::SimpleMath::Vector3 rotationAxis = defaultUp.Cross(axis);

	// 2. 回転角を計算（内積とacos）
	// defaultUpとaxisがなす角度を求める
	float dot = defaultUp.Dot(axis);
	// 浮動小数点誤差を考慮して値をクランプする
	dot = std::min(1.0f, std::max(-1.0f, dot));
	float angle = std::acosf(dot);

	// 3. クォータニオンを生成
	// 回転軸と回転角を使ってクォータニオンを作成
	DirectX::SimpleMath::Quaternion quaternion = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(rotationAxis, angle);

	// ----------------------------------------------------
	// 特殊ケース：2つのベクトルがほぼ平行な場合
	// 外積がゼロになり、回転軸を計算できないため、個別に処理する
	// ----------------------------------------------------
	if (rotationAxis.LengthSquared() < 1e-6f) {
		if (dot > 0.999f) {
			// 同じ方向を向いている場合、回転は不要
			quaternion = DirectX::SimpleMath::Quaternion::Identity;
		}
		else {
			// 正反対の方向を向いている場合、任意の軸周りに180度回転
			quaternion = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Right, DirectX::XM_PI);
		}
	}


	Matrix world = Matrix::Identity;
	world *= Matrix::CreateTranslation(SimpleMath::Vector3(0.0f, m_length / 2.0f, 0.0f));

	world *= Matrix::CreateFromQuaternion(quaternion);
	world *= Matrix::CreateTranslation(m_position + (-axis * (m_length / 2.0f)));

	// 円柱の描画
	cylinder->Draw(world, view, proj, Colors::Red);
	// 級の描画
	sphere->Draw(Matrix::CreateTranslation(m_position + -m_axis * m_length / 2.0f), view, proj, Colors::Red);
	sphere->Draw(Matrix::CreateTranslation(m_position + m_axis * m_length / 2.0f), view, proj, Colors::Red);
}
