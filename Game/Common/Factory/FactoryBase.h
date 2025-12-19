/*****************************************************************//**
 * @file    FactoryBase.h
 * @brief   生成器の基底クラスに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/12/19
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <memory>

// クラスの定義 ===============================================================
/**
 * @brief 生成器の基底クラス
 */
template <typename T>
class FactoryBase
{

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	FactoryBase() = default;

	// デストラクタ
	virtual ~FactoryBase() = default;
	
// 操作
public:

	// 作成する
	std::unique_ptr<T> Create()
	{
		std::unique_ptr<T> instance = std::make_unique<T>();

		// 具体的な組み立てを派生先に任せる
		Assemble(instance.get());

		return std::move(instance);
	}


// 内部実装
protected:
	// 生成したオブジェクトを組み立てる
	virtual void Assemble(T* instance) = 0;

};
