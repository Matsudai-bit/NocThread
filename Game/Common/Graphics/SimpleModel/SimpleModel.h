/*****************************************************************//**
 * @file    SimpleModel.h
 * @brief   DirectX::Modelを使いやすくしたクラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/05/29
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// フレームワーク関連
#include "Game/Common/Framework/CommonResources/CommonResources.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================
/**
 * @brief モデルオブジェクト
 */
class SimpleModel
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	DirectX::Model m_model; ///< モデル

	DirectX::SimpleMath::Vector3 m_position; ///< 座標
	DirectX::SimpleMath::Vector3 m_rotation; ///< 回転
	float m_scale;	///< 拡大率

	DirectX::CommonStates* m_pStates;	///< 共通状態


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SimpleModel(DirectX::CommonStates* pStates);

	// デストラクタ
	~SimpleModel() = default;


// 操作
public:
	// 描画処理
	void Draw(CommonResources* pCommonResources, ID3D11DeviceContext* context, const DirectX::SimpleMath::Matrix& proj, const DirectX::SimpleMath::Matrix& view);


// 取得/設定
public:

	// モデルの設定
	void SetModel(const DirectX::Model& model) { m_model = model; };

	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }

	// 回転の設定
	void SetRotation(const DirectX::SimpleMath::Vector3& rotation) { m_rotation = rotation; }

	// 拡大率の設定
	void SetScale(const float& scale) { m_scale = scale; }




// 内部実装
private:


};
