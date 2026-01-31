/**
 * @file   ResultScene.h
 *
 * @brief  タイトルシーンに関するヘッダファイル
 *
 * @author 松下大暉
 *
 * @date   2025/09/04
 */

// 多重インクルードの防止 =====================================================
#pragma once

#include <memory>


// ヘッダファイルの読み込み ===================================================
#include "Game/Scene/SceneManager.h"
#include "Game/Common/Framework/StateMachine/StateMachine.h"

#include "Game/Common/Framework/Input/InputSystem/InputSystem.h"
#include "Game/Common/Framework/Input/InputActionType/InputActionType.h"

// クラスの宣言 ===============================================================
class CommonResources;  // 共通リソース
class Canvas;			// キャンバス
class Sprite;			// スプライト
struct InputEventData;	// 入力イベントデータ


// クラスの定義 ===============================================================
/**
 * @brief タイトルシーン
 */
class ResultScene : public MyLib::Scene<CommonResources>
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	std::unique_ptr<Canvas> m_canvas; ///< UI表示キャンバス

	// オブジェクト関連

	// UI
	std::unique_ptr<Sprite>				m_backgroundSprite;	///< 背景スプライト
	std::unique_ptr<Sprite>				m_backgroundAlphaFilterSprite;	///< 背景スプライトの透過フィルター

	// ステートマシーン
	std::unique_ptr<StateMachine<ResultScene>> m_stateMachine; ///< ステートマシーン

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ResultScene();

	// デストラクタ
	~ResultScene();


// 操作
public:
	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(float deltaTime) override;

	// 描画処理
	void Render() override;

	// 終了処理
	void Finalize() override;


// 取得/設定
public:

	// キャンバスの取得
	Canvas* GetCanvas()const;

// 内部実装
private:
	// 入力のコールバックの登録
	void RegisterBindCallbackToInput();
	// 紐づけの解除をする
	void UnBindCallbackToInput();

	// 出る
	void OnInputExit(const InputEventData& data);

	// 中心の算出
	DirectX::SimpleMath::Vector2 CalcCenterOrigin(ID3D11ShaderResourceView* pTexture);

};
