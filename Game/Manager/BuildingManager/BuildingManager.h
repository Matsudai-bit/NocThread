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

	/**
	 * @brief カメラのフラスタムのバッファ
	 */
	struct CameraFrustumConstantBuffer
	{
		DirectX::SimpleMath::Vector4 planes[6];
		int maxIndexCount;
		float padding[3];


	};

	//今回使うパーティクルの資料例
	struct ParticleCompute 
	{

		float radius{};
		DirectX::SimpleMath::Vector3 position{};
	};

	struct ResultCompute
	{
		int index{};
	};

	static constexpr int DEFAULT_BUFFER_SIZE = 500;

// データメンバの宣言 -----------------------------------------------
private:
	
	std::vector<std::unique_ptr<Building>> m_buildings; ///< 建物

	const CommonResources* m_pCommonResources;

	// コンピュートシェーダー関連
	// コンピュートシェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;
	// パーティクル
	std::vector<ParticleCompute> m_particles;
	// バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_resultBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_stagingBuffer;
	// SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_particleSRV;
	// UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_resultUAV;

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


	void SetBuildings(std::unique_ptr<std::vector<std::unique_ptr<Building>>> buildings);


private:


// 取得/設定
public:

	// 建物を探す
	bool FindBuilding(const int& tileNumber, const Building*& outBuilding) const;

// 内部実装
private:




	void DrawDefault(const Camera& camera);
	void DrawFrustumCulling(const Camera& camera);
	void DrawFrustumCullingCS(const Camera& camera);
};
