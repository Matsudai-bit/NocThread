/*****************************************************************//**
 * @file    TaskManager.cpp
 * @brief   タスクを管理するクラス
 * 
 * @author  松下大暉
 * @date    2025/11/18
 *********************************************************************/
#include "pch.h"
#include "TaskManager.h"

using namespace MyLib;

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

// 更新関数
void TaskManager::Update(float elapsedTime)
{
	// ルートタスクから子供タスクに向かって再起的に更新処理を実行する
	ChildTaskUpdate(m_rootTask, elapsedTime);

	// 実行中のタスクをルートタスクに設定
	m_currentTask = m_rootTask;
}

// 描画関数
void TaskManager::Render()
{
	// 描画順序管理テーブルに従ってタスクの描画関数を呼び出す
	for (auto it = m_ot.begin(); it != m_ot.end(); it++)
	{
		// 実行中のタスクを設定
		m_currentTask = (*it);

		(*it)->Render();
	}

	// 描画順序管理テーブルクリア
	m_ot.clear();

	// 実行中のタスクをルートタスクに設定
	m_currentTask = m_rootTask;
}

void MyLib::TaskManager::AddTask(Task* task)
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
			std::ostringstream ostr;
			ostr << "Task_" << m_totalTaskCnt;
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

	// メモリから解放する
	auto parentChildList = task->GetParent()->GetTaskConnectInfo().GetChildList();
	auto it = std::find(parentChildList->begin(), parentChildList->end(), task);
	if (it != parentChildList->end())
	{
		parentChildList->erase(it);
	}
}

// 子供タスクの更新関数を再起呼び出しで実行する関数
void TaskManager::ChildTaskUpdate(Task* task, float elapsedTime)
{
	auto childList = task->GetTaskConnectInfo().GetChildList();
	auto it = childList->begin();
	while (it != childList->end())
	{
		// 実行中のタスクを設定
		m_currentTask = (*it);

		// Update関数の戻り値がfalseの場合はタスクを削除する
		if (!(*it)->Update(elapsedTime))
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
		ChildTaskUpdate(*it, elapsedTime);
		it++;
	}
}

