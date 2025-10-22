/*****************************************************************//**
 * @file    BuildingManager.h
 * @brief   建物管理に関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/15
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <vector>

#include "Game/GameObjects/Prop/Building/Building.h"
#include "Game/Common/ResourceManager/ResourceManager.h"

// クラスの前方宣言 ===================================================
class CollisionManager;
class CommonResources;

// クラスの定義 ===============================================================
/**
 * @brief 建物管理
 */
class BuildingManager
{
// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr int MAX_INSTANCE = 1024;

// 構造体
private:

	struct  InstanceBuffer
	{
		DirectX::SimpleMath::Matrix world;
	};

	// 定数バッファ
	struct ConstantBuffer
	{
		DirectX::SimpleMath::Matrix viewProjMatrix;
	};

// データメンバの宣言 -----------------------------------------------
private:
	
	std::vector<std::unique_ptr<Building>> m_buildings; ///< 建物

	nlohmann::json m_stageJson;


	const CommonResources* m_pCommonResources;

	// インスタンシング描画用
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;		///< インスタンシング用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;		///< インスタンシング用バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;		///< 定数バッファ
	InstanceBuffer*  m_pInstanceData;		///< インスタンシング用バッファ

	// 頂点シェーダ
	Microsoft::WRL::ComPtr <ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> m_ps;
	 
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;	///< インプットレイアウト

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	BuildingManager();

	// デストラクタ
	~BuildingManager();


// 操作
public:
	// 初期化処理
	void Initialize(const CommonResources* pCommonResources);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);

	// 終了処理
	void Finalize();

	bool RequestCreate(
		CollisionManager* pCollisionManager,
		const CommonResources* pCommonResources);
	void Save();

private:
	void CreateBuilding(
		const DirectX::SimpleMath::Vector3& position,
		const DirectX::SimpleMath::Vector3& scale,
		CollisionManager* pCollisionManager,
		const CommonResources* pCommonResources);




// 取得/設定
public:


// 内部実装
private:


};
