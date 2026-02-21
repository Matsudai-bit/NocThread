/*****************************************************************//**
 * @file    CheckpointObjectController.h
 * @brief   チェックポイントオブジェクトに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/12/18
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <unordered_map>
#include <memory>

// ライブラリ関連
#include "Library/MyLib/DirectXMyToolKit/ModelPart/ModelPart.h"

// フレームワーク関連
#include "Game/Common/Framework/EventSystem/EventSystem.h"

// ゲームオブジェクト関連
#include "Game/GameObjects/Common/Transform/Transform.h"


// クラスの前方宣言 ===================================================
class ResourceManager;
class Camera;

enum class CheckpointControllEventID
{
	LOOK_AT_HELICOPTER
};

// クラスの定義 ===============================================================
/**
 * @brief チェックポイントオブジェクト
 */
class CheckpointObjectController
	: public EventSystem< CheckpointControllEventID>
	
{

// 列挙型の宣言 ----------------------------------------------------

	/**
	 * @brief モデルパーツ識別子
	 */
	enum class ModelPartID
	{
		ROOT,
		BODY,
		NECK,
		HEAD
	};

	enum class State
	{
		IDLING,
		LOOK_AT_HELICOPTER
	};

// クラス定数の宣言 -------------------------------------------------
public:

	static constexpr float DEFAULT_ROTATION_Y_DEGREE = 180.0f;


// データメンバの宣言 -----------------------------------------------
private:

	Transform m_transform;

	std::unordered_map<ModelPartID, std::unique_ptr<MyLib::ModelPart>>	m_modelParts;
	std::unordered_map<ModelPartID, std::unique_ptr<DirectX::Model>>	m_models;

	float m_rotateAngle;

	State m_state;

	DirectX::SimpleMath::Quaternion m_neckRotation;
	DirectX::SimpleMath::Quaternion m_startNeckRotation;
	DirectX::SimpleMath::Quaternion m_targetNeckRotation;

	DirectX::SimpleMath::Quaternion m_headRotation;
	DirectX::SimpleMath::Quaternion m_startHeadRotation;
	DirectX::SimpleMath::Quaternion m_targetHeadRotation;
	float m_lerpT;

	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CheckpointObjectController();

	// デストラクタ
	~CheckpointObjectController();


// 操作
public:
	// 初期化処理
	void Initialize(ResourceManager* pResourceManager, ID3D11Device* device);

	// 更新処理
	void Update(float deltaTime);

	// 描画処理
	void Draw(ID3D11DeviceContext1* context, DirectX::CommonStates* states, const Camera& camera);

	// 終了処理
	void Finalize();



// 取得/設定
public:

	// トランスフォームの取得
	Transform* GetTransform()  { return &m_transform; }

// 内部実装
private:


	// ヘリコプターを見るよう要求
	void LookAtHelicopter();
};
