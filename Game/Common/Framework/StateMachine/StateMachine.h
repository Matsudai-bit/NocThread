/*****************************************************************//**
 * @file    StateMachine.h
 * @brief   ステートマシーンに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/06/18
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <list>
#include <functional>
#include <typeinfo>

#include "Game/Common/Framework/StateMachine/StateBase/StateBase.h"
#include "Library/DebugHelper.h"

// クラスの前方宣言 ===================================================

// クラスの定義 ===============================================================

/**
 * @brief ステートマシーン
 * 
 * 状態を管理するクラス
 * 
 * @tparam OwnerType 状態の所持者の型
 */
template<typename OwnerType>
class StateMachine
{
// クラス定数の宣言 -------------------------------------------------
public:

	// 変更命令関数の初期化値
	static constexpr void (*INITIAL_CHANGE_FUNC)() = []() {};
// データメンバの宣言 -----------------------------------------------
private:
	// 状態の持ち主のポインタ
	OwnerType* m_pOwner;

	// 今のステート
	std::list<std::unique_ptr<StateBase<OwnerType>>> m_stateList; 

	// ステートの変更命令を保存しておく関数オブジェクト
	std::function<void()> m_fnChangeState;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StateMachine(OwnerType* pOwner) 
		: m_pOwner{ pOwner }
	{
		// 変更関数を初期化する
		m_fnChangeState = INITIAL_CHANGE_FUNC;
	}

	// デストラクタ
	~StateMachine() = default;


// 操作
public:


	/**
	 * @brief 状態を変更する
	 * 
	 * ステートの変更処理は更新の直前に呼ぶ
	 * 
	 * @param[in] ...a_args　ステートのコンストラクタに渡す値
	 */
	template<typename StateType, typename...ArgType>
	void ChangeState(ArgType...a_args)
	{
		StateChanger<StateType>(true, a_args...);
	}


	/**
	 * @brief 状態を追加する
	 * 
	 * ステートの変更処理は更新の直前に呼ぶ
	 * 
	 * @param[in] ...a_args　ステートのコンストラクタに渡す値
	 */
	template<typename StateType, typename...ArgType> // 変更点(1) ステートを追加する関数を追加
	void PushState(ArgType...a_args)
	{
		StateChanger<StateType>(false, a_args...);
	}

	/**
	 * @brief 今のステートをポップする
	 */
	void PopState() 
	{
		m_fnChangeState =
			[&]()
			{
				// 一番後ろのステートを取得
				// 無ければreturn
				StateBase<OwnerType>* pNowState = GetNowState();
				if (pNowState == nullptr)
				{
					return;
				}

				// 終了関数を呼び、ポップする
				pNowState->CallExit();
				m_stateList.pop_back();
			};
	}


	/**
	 * @brief 現在のステージを全てクリアする
	 * 
	 * 状態を終了させリストのデータを全てクリアする
	 * 
	 */
	void ClearState()
	{
		m_fnChangeState =
			[&]()
			{
				StateBase<OwnerType>* pNowState = GetNowState();
				if (pNowState == nullptr)
				{
					return;
				}
				pNowState->CallExit();
				m_stateList.clear();
			};
	}


	/**
	 * @brief 状態の更新
	 * 
	 */
	void Update(float deltaTime)
	{
		// ステートの変更命令があれば処理する
		m_fnChangeState();
		m_fnChangeState = INITIAL_CHANGE_FUNC;

		StateBase<OwnerType>* pBackState = GetNowState();
		if (pBackState != nullptr)
		{
			pBackState->CallUpdate(deltaTime);
		}

	}

	/**
	 * @brief 状態の描画
	 */
	void Draw()
	{

		StateBase<OwnerType>* pBackState = GetNowState();
		if (pBackState != nullptr)
		{
			pBackState->CallDraw();
		}

	}

	// 現在有効なステートを取得

	/**
	 * @brief 現在動いているステートを取得
	 * 
	 * @return 現在動いているステート
	 */
	StateBase<OwnerType>* GetNowState() 
	{
		if (m_stateList.empty())
		{
			return nullptr;
		}
		StateBase<OwnerType>* pBackState = m_stateList.back().get();
		if (pBackState == nullptr)
		{
			return nullptr;
		}

		return pBackState;
	}

// 内部実装
private:

	/**
	 * @brief 状態の変更
	 * 
	 * @param[in] a_isPop	ポップするかどうか
	 * @param[in] ...a_args	変更するステートのコンストラクタ値
	 */
	template<typename StateType, typename...ArgType>
	void StateChanger(bool a_isPop, ArgType...a_args)
	{
		// ステートの変更命令を格納する
		m_fnChangeState = [&,this, a_isPop]()
			{
				// オーナーがセットされてなければ何もしない
				if (this->m_pOwner == nullptr)
				{
					return;
				}

				StateBase<OwnerType>* pBackState = GetNowState();


				// もしすでにステートがセットされてたら終了する
				if (pBackState != nullptr)
				{
					pBackState->CallExit();

					if (a_isPop)
					{
						this->m_stateList.pop_back();
					}
				}

				// 新しいステートを作成
				std::unique_ptr<StateBase<OwnerType>> newState = std::make_unique<StateType>(a_args...);
				if (newState == nullptr)
				{
					return;
				}
		
				// 新しいステートにこのマシーンをセット
				newState->SetMachine(this);
				newState->SetOwner(this->m_pOwner);
			
				// ステートの開始
				newState->CallStart();


				// 作成したステートをリストに追加
				this->m_stateList.emplace_back(std::move(newState));

			};
	}



};
