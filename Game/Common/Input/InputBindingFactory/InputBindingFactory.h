/*****************************************************************//**
 * @file    InputBindingFactory.h
 * @brief   入力と動作の紐づけを作成するファクトリーに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/10/22
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include "Game/Common/Input/InputSystem/InputSystem.h"
#include "Game/Common/Input/InputActionType/InputActionType.h"
#include "Game/Common/Factory/FactoryBase.h"

// クラスの前方宣言 ===================================================

/**
 * @brief 入力と動作の紐づけを作成するファクトリー群
 */
namespace InputBindingFactory
{
	/**
	 * @brief プレイヤー入力と動作の紐づけを作成するファクトリー
	 */
	class PlayerInputFactory :
		public FactoryBase<InputSystem<InputActionType::PlyayerActionID>>
	{
	public:

		// コンストラクタ/デストラクタ
		PlayerInputFactory() = default;
		~PlayerInputFactory() = default;

		// 組み立てる
		void Assemble(InputSystem<InputActionType::PlyayerActionID>* instance, const DefaultSpawnDesc& desc ) override;
	};

	/**
	 * @brief UI入力と動作の紐づけを作成するファクトリー
	 */
	class UIInputFactory :
		public FactoryBase<InputSystem<InputActionType::UIActionID>>
	{
	public:

		// コンストラクタ/デストラクタ
		UIInputFactory() = default;
		~UIInputFactory() = default;

		// 組み立てる
		void Assemble(InputSystem<InputActionType::UIActionID>* instance, const DefaultSpawnDesc& desc) override;
	};

	/**
	 * @brief システム入力と動作の紐づけを作成するファクトリー
	 */
	class SystemInputFactory :
		public FactoryBase<InputSystem<InputActionType::SystemActionID>>
	{
	public:

		// コンストラクタ/デストラクタ
		SystemInputFactory() = default;
		~SystemInputFactory() = default;

		// 組み立てる
		void Assemble(InputSystem<InputActionType::SystemActionID>* instance, const DefaultSpawnDesc& desc) override;
	};

}