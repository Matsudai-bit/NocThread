/*****************************************************************//**
 * @file    TaskManager.h
 * @brief   タスクを管理するクラス
 * 
 * @author  松下大暉
 * @date    2025/11/18
 *********************************************************************/
#pragma once

#include <list>
#include <set>
#include <sstream>
#include <functional>

class Camera;

class TaskManager;
class Task;

/**
	* @brief タスク連結情報管理クラス
	*/
class TaskConnectInfo
{
private:
	// タスク管理オブジェクトへのポインタ
	TaskManager* m_taskManager;

	// 親タスクへのポインタ
	Task* m_parent;

	// 子供タスクのリスト
	std::list<Task*> m_children;

public:
	TaskConnectInfo() : m_taskManager{}, m_parent{} {}
	TaskManager* GetTaskManager() const { return m_taskManager; }
	Task* GetParent() const { return m_parent; }
	std::list<Task*>* GetChildList() { return &m_children; }
	void SetTaskManager(TaskManager* taskManager) { m_taskManager = taskManager; }
	void SetParent(Task* task) { m_parent = task; }
	void AddChild(Task* task) { m_children.push_back(task); }
	bool DeleteChildList(Task* task);
};

/**
	* @brief タスクの基底クラス	
	*/
class Task
{
private:
	// 描画順序管理用(小さいほど手前に描画される）
	int m_ot;

	// タスク連結情報
	TaskConnectInfo m_connect;

	// タスクの名前
	std::string m_name;

	// タスクが有効かどうか
	bool m_isEnabled;

public:


	TaskConnectInfo& GetTaskConnectInfo() { return m_connect; }

public:

	// コンストラクタ
	Task() : m_ot(0), m_isEnabled{ true } {}
	Task(Task* parent) : Task() { ChangeParent(parent); }

	// デストラクタ
	virtual ~Task() {}

// オーバーライドして使う関数
public:

	// 更新関数（falseを返すとタスクが消える）
	virtual bool UpdateTask(float deltaTime)
	{
		UNREFERENCED_PARAMETER(deltaTime);
		return true;
	}

	// 描画関数
	virtual void DrawTask(const Camera& camera) { UNREFERENCED_PARAMETER(camera); }

public:

	// タスクマネージャーの取得関数
	TaskManager* GetTaskManager() const { return m_connect.GetTaskManager(); }

	// 親タスクの取得関数
	Task* GetParent() const { return m_connect.GetParent(); }

	// 描画順設定関数（otの値が0が一番手前）
	void SetOt(int ot) { this->m_ot = ot; }

	// 描画順の取得関数
	int GetOt() { return m_ot; }

	// 指定したタスクの子供にする変更する関数
	void ChangeParent(Task* parent);

	// タスクに名前を付ける関数
	void SetName(const std::string& name) { m_name = name; }

	// タスクの名前を取得する関数
	const std::string& GetName() { return m_name; }

	// タスク更新関数（子供のタスクまでタスク更新関数が呼び出される）
	void UpdateTasks(std::function<void(Task*)> func)
	{
		func(this);
		std::list<Task*>* children = GetTaskConnectInfo().GetChildList();
		for (const auto it : *children)
		{
			it->UpdateTasks(func);
		}
	}

	// タスクを有効化する
	void EnableTask();

	// タスクを無効化する
	void DisableTask();

	// 有効かどうか
	bool IsEnabled() const;

};

/**
	* @brief タスク管理クラス
	*/
class TaskManager
{

private:
#pragma region
	// 描画順序管理テーブルへの登録用比較関数（otの値が大きい物が先頭へ）
	struct greater_ot : std::binary_function<Task*, Task*, bool>
	{
		bool operator()(Task* a, Task* b) const
		{
			return a->GetOt() > b->GetOt();
		}
	};

	// 描画順序管理テーブル
	std::multiset<Task*, greater_ot> m_ot;

	// ルートタスク
	std::unique_ptr<Task> m_rootTask;

	// 実行中のタスク
	Task* m_currentTask;

	

	// 子供タスクの更新関数を実行する関数
	void ChildTaskUpdate(Task* task, float deltaTime);

	// タスク生成数
	uint64_t m_totalTaskCnt;

#pragma endregion

public:

	// コンストラクタ
	TaskManager() : m_rootTask{}, m_currentTask{}, m_totalTaskCnt{}
	{
		// ルートタスクの作成
		m_rootTask = std::make_unique<Task>();
		// 現在のタスクの設定
		m_currentTask = m_rootTask.get();
		// ルートタスクの名前設定
		m_rootTask->SetName("RootTask");
	}

	// デストラクタ
	virtual ~TaskManager()
	{
		//DeleteTask(m_rootTask);
	}

	// 更新関数
	virtual void Update(float deltaTime);

	// 描画関数
	virtual void Render(const Camera& camera);

	void AddTask(Task* task);

	// タスクの削除関数
	void DeleteTask(Task* task);

	// ルートタスクを取得する関数
	Task* GetRootTask() { return m_rootTask.get(); }

};

#pragma region
	

	
#pragma endregion



