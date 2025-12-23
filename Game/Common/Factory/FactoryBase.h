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

struct DefaultSpawnDesc {

};

// クラスの定義 ===============================================================
/**
 * @brief 生成器の基底クラス
 * @tparam T 生成するオブジェクトの型
 * @tparam TDesc 生成に必要な設定データ（Description）の型
 */
template <typename T, typename TDesc = DefaultSpawnDesc >
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
	virtual std::unique_ptr<T> Create(const TDesc& desc)
	{
		std::unique_ptr<T> instance = std::make_unique<T>();

		// 具体的な組み立てを派生先に任せる
		Assemble(instance.get(), desc);

		return std::move(instance);
	}


	// 内部実装
protected:
	// 生成したオブジェクトを組み立てる
	virtual void Assemble(T* instance, const TDesc& desc) = 0;

};
