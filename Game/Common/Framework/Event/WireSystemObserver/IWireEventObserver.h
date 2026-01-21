/*****************************************************************//**
 * @file    IWireEventObserver.h
 * @brief   ワイヤーイベントの観察者インターフェースに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/06/25
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
struct WireEventData;

// クラスの前方宣言 ===================================================
namespace DirectX
{
	namespace SimpleMath
	{
		struct Vector3;
	}
}

// クラスの定義 ===============================================================
/**
 * @brief ワイヤーが影響を及ぼすイベントに共通のインターフェース
 */
class IWireEventObserver
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:



// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	IWireEventObserver() = default;

	// デストラクタ
	virtual ~IWireEventObserver() {}


// 操作
public:

	//  ワイヤーに掴まれたときの挙動
	virtual void OnWireGrab(const WireEventData& eventData) = 0;

	// ワイヤーから離れたとき
	virtual void OnWireRelease(const WireEventData& eventData) = 0;

	// ワイヤーで引き寄せられるとき
	virtual void OnWirePull(const WireEventData& eventData) = 0;

	// ワイヤーで投げられるとき
	virtual void OnWireThrow(const WireEventData& eventData, const DirectX::SimpleMath::Vector3& throwImpulse) = 0;

	// ワイヤーに掴まれている時
	virtual void OnWireHover(const WireEventData& eventData) = 0;



// 取得/設定
public:


// 内部実装
private:


};
