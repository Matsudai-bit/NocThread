/*****************************************************************//**
 * @file    IdPool.h
 * @brief   IDプールのヘッダーファイル（自作ライブラリ）
 * 
 * @author  松下大暉
 * @date    2026/2/01
 *********************************************************************/

#include <vector>

namespace MyLib
{
	/**
	* @brief 識別番号の生成器
	*/
	template <typename T>
	class IdPool
	{
	public:
		IdPool()
		{
			m_nextID = T();

			m_freeIDs.clear();
		}

	private:
		T m_nextID;
		std::vector<T> m_freeIDs;

	public:
		T operator() ()
		{
			// 再利用可能なIDプールをチェック
			if (!m_freeIDs.empty())
			{
				T reusedID = m_freeIDs.back();
				m_freeIDs.pop_back();
				return reusedID;
			}

			// プールが空であれば、新しい連番IDを発行
			if (m_nextID == 0)
			{
				m_nextID++; // 1から開始
			}
			int newID = m_nextID;

			m_nextID++;

			// プールに追加
			//m_freeIDs.emplace_back(newID);

			return newID;
		}

		// ID を返却する
		void RecycleID(T id)
		{
			if (id > 0)
			{
				// 破棄されたIDをキューの末尾に追加
				m_freeIDs.push_back(id);
			}
		}

	};
}
