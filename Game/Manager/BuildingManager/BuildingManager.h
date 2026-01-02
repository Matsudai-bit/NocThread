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
#include "Game/Common/TaskManager/TaskManager.h"


// クラスの前方宣言 ===================================================
class CollisionManager;
class CommonResources;


// クラスの定義 ===============================================================
/**
 * @brief 建物管理
 */
class BuildingManager
	: public Task
{
// クラス定数の宣言 -------------------------------------------------
public:


	//今回使うパーティクルの資料例
	struct ParticleCompute {
		DirectX::SimpleMath::Vector4 planes[6];

		float radius;
		DirectX::SimpleMath::Vector3 position;
	};

	struct ResultCompute
	{
		int index;
	};

	static constexpr int DEFAULT_BUFFER_SIZE = sizeof(ParticleCompute) * 1000;

// データメンバの宣言 -----------------------------------------------
private:
	
	std::vector<std::unique_ptr<Building>> m_buildings; ///< 建物

	nlohmann::json m_stageJson;

	const CommonResources* m_pCommonResources;

	// コンピュートシェーダー関連


	// コンピュートシェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;
	// パーティクル
	std::vector<ParticleCompute> m_particles;
	// バッファ
	ID3D11Buffer* m_pParticleBuffer;
	ID3D11Buffer* m_pResultBuffer;
	// SRV
	ID3D11ShaderResourceView* m_pParticleSRV;
	// UAV
	ID3D11UnorderedAccessView* m_pResultUAV;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	BuildingManager(const CommonResources* pCommonResources);

	// デストラクタ
	~BuildingManager();


// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	bool UpdateTask(float deltaTime) override;

	// 描画処理
	void DrawTask(const Camera& camera) override;

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
		const int& tileNumber,
		CollisionManager* pCollisionManager,
		const CommonResources* pCommonResources);




// 取得/設定
public:

	// 建物を探す
	bool FindBuilding(const int& tileNumber, const Building*& outBuilding) const;

// 内部実装
private:

	void CreateStructuredBuffer(UINT BindFlags, D3D11_USAGE Usage, UINT elementSize, UINT count, ID3D11Buffer** ppBuffer, ID3D11Device1* device);
	void CreateBufferSRV(ID3D11Buffer* pSourceBuffer, ID3D11ShaderResourceView** ppSRView, ID3D11Device1* device);
	void CreateBufferUAV(ID3D11Buffer* pSourceBuffer, ID3D11UnorderedAccessView** ppUAView, ID3D11Device1* device);

	ID3D11Buffer* CreateAndCopyToBuffer(ID3D11Buffer* pSourceBuffer, ID3D11Device1* device, ID3D11DeviceContext* context);

	void DrawDefault(const Camera& camera);
	void DrawCS(const Camera& camera);
};
