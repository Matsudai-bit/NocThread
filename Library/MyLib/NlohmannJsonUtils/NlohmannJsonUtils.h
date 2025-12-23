/*****************************************************************//**
 * @file    NlohmannJsonUtils.h
 * @brief   汎用的なnlohmann.jsonユーティリティに関するヘッダーファイル
 *
 * @author  松下大暉
 * @date    2025/07/05
 *********************************************************************/

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <cmath>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

// クラスの前方宣言 ===================================================

/**
 * @brief 自作ライブラリ
 */
namespace MyLib
{
	namespace  NlohmannUtils
	{
		

		/**
		 * @brief　Jsonの読み込みを試す
		 * 
		 * @param[in] filePath	ファイルパス
		 * @param[out] outJson	jsonの結果先
		 * 
		 * @returns true	成功
		 * @returns false	失敗
		 */
		inline bool TryLoadJson(const std::string& filePath, nlohmann::json* outJson)
		{
			if (outJson == nullptr) { return false; }

			// ファイルの読み込み
			std::ifstream ifs{ filePath };
			if (!ifs.is_open())
			{
				return false;
			}
			try
			{
				// nlohmann::jsonへ変換
				ifs >> *outJson;
			}
			catch (nlohmann::json::exception&)
			{
				return false;
			}

			return true;
		}

		/**
		 * @brief nolohmann::jsonを任意の型へ変換する
		 * 
		 * @param[in] json	変換元json
		 * @param[out] outT	変換後の出力先
		 * 
		 * @returns true	成功
		 * @returns false	失敗		 
		 */
		template<typename T>
		inline bool TryConvertJson(const nlohmann::json& json, T* outT)
		{
			if (outT == nullptr) { return false; }
			try 
			{
				// 変換
				*outT = json.get<T>();
			}
			catch (nlohmann::json::exception&)
			{
				return false;
			}

			return true;
		}

		/**
		 * @brief 読み込みと変換を同時に行う
		 * 
		 * @param[in] filePath ファイルパス
		 * @param[out] outT		変換後の出力先
		 * 
		 * @returns true	成功
		 * @returns false	失敗			 
		 */
		template<typename T>
		inline bool TryLoadAndConvertJson(const std::string& filePath, T* outT)
		{
			nlohmann::json json;
			if (!TryLoadJson(filePath, &json)) { return false; }
			return TryConvertJson(json, outT);
		}

	}


};