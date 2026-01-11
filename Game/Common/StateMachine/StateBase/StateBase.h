/*****************************************************************//**
 * @file    StateBase.h
 * @brief   状態の基底クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================

// クラスの前方宣言 ===================================================
template<typename OwnerType>
class StateMachine;

// クラスの定義 ===============================================================
/**
 * @brief 状態の基底クラス
 * 
 * @tparam OwnerType 状態の所持者の型
 */
template<typename OwnerType>
class StateBase
{
// クラス定数の宣言 -------------------------------------------------
public:



// データメンバの宣言 -----------------------------------------------
private:

	OwnerType* m_pOwner;				///< 所有者

	StateMachine<OwnerType>* m_pMachine; ////< このステートを管理しているステートマシン


// クラスの設定
protected:

	// ステートマシン以外から呼び出し関数などにアクセスできないようにする
	friend class StateMachine<OwnerType>; 


	

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StateBase()
		: m_pOwner{ nullptr }
		, m_pMachine{ nullptr }
	{}
		

	// デストラクタ
	virtual ~StateBase() = default;

// 操作
protected:
	// 状態開始時に呼ばれる
	virtual void OnStartState() {};

	// 更新処理
	virtual void OnUpdate(float deltaTime) {
		UNREFERENCED_PARAMETER(deltaTime);
	};

	// 描画処理
	virtual void OnDraw() {};

	// 終了処理
	virtual void OnExitState() {};

private:

	// この状態を管理しているステートマシーンをセット
	void SetMachine(StateMachine<OwnerType>* pMachine)
	{
		m_pMachine = pMachine;
	}

	// この状態を管理しているステートマシーンをセット
	void SetOwner(OwnerType* pOwner)
	{
		m_pOwner = pOwner;
	}


	// 開始関数をマシンから呼ぶための関数
	void CallStart() // 変更点(4) 仮想関数をマシンが安全に呼ぶための関数を追加
	{

		OnStartState();
	}

	// 更新関数をマシンから呼ぶための関数
	void CallUpdate(float deltaTime)
	{
		OnUpdate(deltaTime);
	}

	// 更新関数をマシンから呼ぶための関数
	void CallDraw()
	{

		OnDraw();
	}

	// 終了関数をマシンから呼ぶための関数
	void CallExit()
	{

		OnExitState();
	}


// 取得/設定
protected:

	// 所有者の取得
	OwnerType* GetOwner() { return m_pOwner; }

	// ステートマシーンの取得
	StateMachine<OwnerType>* GetStateMachine() { return m_pMachine; }



// 内部実装
private:


};
