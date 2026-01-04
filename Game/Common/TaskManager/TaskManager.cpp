/*****************************************************************//**
 * @file    TaskManager.cpp
 * @brief   タスクを管理するクラス
 * 
 * @author  松下大暉
 * @date    2025/11/18
 *********************************************************************/
#include "pch.h"
#include "TaskManager.h"


// 子供タスクリストから指定タスクを削除する関数
bool TaskConnectInfo::DeleteChildList(Task* task)
{
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		if ((*it) == task)
		{
			m_children.erase(it);
			return true;
		}
	}
	return false;
}

// 指定したタスクの子供にする変更する関数
void Task::ChangeParent(Task* parent)
{
	// 現在の親タスクを取得
	Task* currentParent = m_connect.GetParent();

	// 親が変わらない場合は何もしない
	if (currentParent == parent) return;

	// 現在の親タスクの子供リストからタスクを削除
	currentParent->GetTaskConnectInfo().DeleteChildList(this);

	// 親タスクを設定
	GetTaskConnectInfo().SetParent(parent);

	// 親タスクの子供リストに追加
	parent->GetTaskConnectInfo().AddChild(this);
}

/**
 * @brief タスクを有効化する
 */
void Task::EnableTask()
{
	// 既に有効の場合は飛ばす
	if (m_isEnabled) { return; }

	m_isEnabled = true;
}

/**
 * @brief タスクを無効化する
 */
void Task::DisableTask()
{
	// 既に無効の場合は飛ばす
	if (!m_isEnabled) { return; }

	m_isEnabled = false;
}

/**
 * @brief 有効かどうか
 * 
 * @returns true 有効
 * @returns false無効
 */
bool Task::IsEnabled() const
{
	// 親タスクの取得
	Task* parent = m_connect.GetParent();

	// 親が有効でなければ無効とする
	if (parent && !parent->IsEnabled())
	{
		return false;
	}

	return m_isEnabled;
}

// 更新関数
void TaskManager::Update(float deltaTime)
{
	// 描画順序管理テーブルクリア
	m_ot.clear();
	// ルートタスクから子供タスクに向かって再起的に更新処理を実行する
	ChildTaskUpdate(m_rootTask.get(), deltaTime);

	// 実行中のタスクをルートタスクに設定
	m_currentTask = m_rootTask.get();
}

// 描画関数
void TaskManager::Render(const Camera& camera)
{
	// 描画順序管理テーブルに従ってタスクの描画関数を呼び出す
	for (auto it = m_ot.begin(); it != m_ot.end(); it++)
	{
		// 有効でなければ描画しない
		if (!(*it)->IsEnabled()) { continue; }

		// 実行中のタスクを設定
		m_currentTask = (*it);

		(*it)->DrawTask(camera);
	}



	// 実行中のタスクをルートタスクに設定
	m_currentTask = m_rootTask.get();
}

void TaskManager::AddTask(Task* task)
{
	// タスクの生成関数（実行中のタスクの子供になる）

		// タスク管理オブジェクトを設定
	task->GetTaskConnectInfo().SetTaskManager(this);

	// 親を設定
	task->GetTaskConnectInfo().SetParent(m_currentTask);

	// 親の子供リストに追加
	m_currentTask->GetTaskConnectInfo().AddChild(task);

	// タスクに名前が付いていない場合は仮で名前を付ける
	if (task->GetName().empty())
	{
		const std::type_info& typeInfo = typeid(*task);

		// type_info::name() メンバー関数で型名文字列を取得する
		// ただし、返される文字列はコンパイラ依存で装飾されている場合がある (後述の注意点参照)
		std::string className = typeInfo.name();

		const std::string prefix = "class ";

		// 文字列が "class " で始まるかチェック
		if (className.rfind(prefix, 0) == 0)
		{
			// "class " の長さ分だけ文字列を切り詰める
			className = className.substr(prefix.length());
		}

		std::ostringstream ostr;
		ostr << className  << " : " << m_totalTaskCnt;
		task->SetName(ostr.str());
	}

	// タスク生成数を加算
	m_totalTaskCnt++;


}

// タスクの削除（子供タスクも全て死ぬ）
void TaskManager::DeleteTask(Task* task)
{
	assert(task != nullptr);

	// 子供タスクを再帰で削除していく
	auto childList = task->GetTaskConnectInfo().GetChildList();
	while (childList->empty() != true)
	{
		DeleteTask(childList->back());
		childList->pop_back();
	}

	// リストから削除する
	auto parentChildList = task->GetParent()->GetTaskConnectInfo().GetChildList();
	auto it = std::find(parentChildList->begin(), parentChildList->end(), task);
	if (it != parentChildList->end())
	{
		parentChildList->erase(it);
	}
}

// 子供タスクの更新関数を再起呼び出しで実行する関数
void TaskManager::ChildTaskUpdate(Task* task, float deltaTime)
{
	// 有効でなければ更新しない
	if (!task->IsEnabled()) { return; }

	auto childList = task->GetTaskConnectInfo().GetChildList();
	auto it = childList->begin();
	while (it != childList->end())
	{
		// 実行中のタスクを設定
		m_currentTask = (*it);

		// Update関数の戻り値がfalseの場合はタスクを削除する
		if (!(*it)->UpdateTask(deltaTime))
		{
			// タスクの削除
			DeleteTask(*it);
			// 子供タスクリストから削除
			it = childList->erase(it);
			continue;
		}
		// 描画順序管理テーブルに登録
		m_ot.insert(*it);
		// 子供のタスクの更新処理を実行する
		ChildTaskUpdate(*it, deltaTime);
		it++;
	}
}

