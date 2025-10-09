//--------------------------------------------------------------------------------------
// File: SceneManager.h
//
// シーンを管理するクラス
//
// Date: 2025.7.28
//--------------------------------------------------------------------------------------
#pragma once

// ESCキーで終了したい場合有効にしてください
//#define ESC_QUIT_ENABLE

#ifdef ESC_QUIT_ENABLE
#include "Keyboard.h"
#endif

#include <mutex>
#include <thread>

#include "Library/DebugHelper.h"
#include <string>
#include <sstream>
#include <typeinfo>


namespace MyLib
{
	template <typename T>
	class SceneManager;

	// シーンの基底クラス
	template <typename T>
	class Scene
	{
	private:

		// シーンマネージャーへのポインタ
		SceneManager<T>* m_sceneManager;



	public:

		// コンストラクタ
		Scene() : m_sceneManager(nullptr) {}

		// デストラクタ
		virtual ~Scene() = default;

		// 初期化
		virtual void Initialize() = 0;

		// 更新
		virtual void Update(float elapsedTime) = 0;

		// 描画
		virtual void Render() = 0;

		// 終了処理
		virtual void Finalize() = 0;

		// デバイスに依存するリソースを作成する関数
		virtual void CreateDeviceDependentResources() {}

		// ウインドウサイズに依存するリソースを作成する関数
		virtual void CreateWindowSizeDependentResources() {}

		// デバイスロストした時に呼び出される関数
		virtual void OnDeviceLost() {}

	public:

		// シーンマネージャー設定関数
		void SetSceneManager(SceneManager<T>* sceneManager) { m_sceneManager = sceneManager; }

		// シーンの切り替え関数
		template <typename U>
		void ChangeScene();

		template <typename U, typename V>
		void ChangeScene();

		// ユーザーが設定したリソース取得関数
		T* GetCommonResources();

	};



	// ロード画面の基底クラス
	template <typename T>
	using LoadingScreen = Scene<T>;



	// シーンマネージャークラス
	template <typename T>
	class SceneManager
	{
	private:

		// 共通でアクセスしたいオブジェクトへのポインタ
		T* m_commonResources;

		// 実行中のシーンへのポインタ
		std::unique_ptr<Scene<T>> m_scene;

		// 次のシーンへのポインタ
		std::unique_ptr<Scene<T>> m_nextScene;

		// ロード画面 
		std::unique_ptr<LoadingScreen<T>> m_loadingScreen;


		std::thread m_loadingThread;	///< ロード用スレッド
		std::mutex	m_loadingMutex;		///<
		bool		m_isLoading;		///< ロード中かどうか

		// シーン削除関数
		void DeleteScene();

	public:

		// コンストラクタ
		SceneManager(T* commonResources = nullptr)
			: m_commonResources(commonResources)
			, m_scene()
			, m_nextScene()
			, m_loadingScreen()
			, m_isLoading{ false }
		{
		};

		// デストラクタ
		virtual ~SceneManager()
		{
			Finalize();

		};

		// 更新
		void Update(float elapsedTime);

		// 描画
		void Render();

		// デバイスに依存するリソースを作成する関数
		void CreateDeviceDependentResources();
		
		// ウインドウサイズに依存するリソースを作成する関数
		void CreateWindowSizeDependentResources();

		// デバイスロストした時に呼び出される関数
		virtual void OnDeviceLost();

		// シーンの設定関数
		template <typename U>
		void SetScene();

		// 次のシーンのリクエスト関数
		template <typename U>
		bool RequestSceneChange();

		template <typename U, typename V>
		bool RequestSceneChange();

		// ユーザーリソース設定関数
		void SetCommonResources(T* commonResources) { m_commonResources = commonResources; }

		// ユーザーリソース取得関数
		T* GetCommonResources() { return m_commonResources; }

		void PrepareNextScene(std::function<std::unique_ptr<Scene<T>>()> sceneFactory);

		bool IsLoading();

		// 追加
		void Finalize();

	};

	// シーンの切り替え関数
	template <typename T>
	template <typename U>
	void Scene<T>::ChangeScene()
	{
		m_sceneManager->RequestSceneChange<U>();
	}

	template <typename T>
	template <typename U, typename V>
	void Scene<T>::ChangeScene()
	{
		m_sceneManager->RequestSceneChange<U, V>();
	}


	// ユーザーが設定したリソース取得関数
	template <typename T>
	T* Scene<T>::GetCommonResources()
	{
		assert(m_sceneManager);

		return m_sceneManager->GetCommonResources();
	}

	// シーンの設定関数
	template <typename T>
	template <typename U>
	void SceneManager<T>::SetScene()
	{
		assert(m_scene == nullptr);

		RequestSceneChange<U>();
	}

	// シーンの設定関数
	template <typename T>
	template <typename U>
	bool SceneManager<T>::RequestSceneChange()
	{
		std::string newStateName = "call RequestSceneChange";
		OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());
 		if (!m_nextScene)
		{
			// シーンを生成
			PrepareNextScene([]() {return std::make_unique<U>(); });
			return true;
		}
		
		return false;
	}

	template <typename T>
	template <typename U, typename V>
	bool SceneManager<T>::RequestSceneChange()
	{
		if (RequestSceneChange<U>())
		{
			// ロード画面を作成
			m_loadingScreen = std::make_unique<V>();
			m_loadingScreen->SetSceneManager(this);
			m_loadingScreen->Initialize();
		}

		return false;
	}

	// 更新関数
	template <typename T>
	void SceneManager<T>::Update(float elapsedTime)
	{
#ifdef ESC_QUIT_ENABLE
		// ESCキーで終了
		auto kb = DirectX::Keyboard::Get().GetState();
		if (kb.Escape) PostQuitMessage(0);
#endif

		// シーンの切り替え処理
		if (!IsLoading() && m_nextScene)
		{
			DeleteScene();

			assert(m_scene == nullptr);

			// シーンを切り替え
			m_scene = std::move(m_nextScene);

			if (m_loadingScreen)
			{
				m_loadingScreen->Finalize();
				m_loadingScreen.reset();
			}
		}

		if (m_loadingScreen)
		{
			m_loadingScreen->Update(elapsedTime);
			return;
		}

		// シーンの更新
		if (m_scene) m_scene->Update(elapsedTime);

	}

	// 描画関数
	template <typename T>
	void SceneManager<T>::Render()
	{
		if (m_loadingScreen)
		{
			m_loadingScreen->Render();
			return;
		}
		// シーンの描画
		if (!IsLoading() && m_scene) m_scene->Render();

	}

	// デバイスに依存するリソースを作成する関数
	template <typename T>
	void SceneManager<T>::CreateDeviceDependentResources()
	{
		if (m_scene) m_scene->CreateDeviceDependentResources();
	}

	// ウインドウサイズに依存するリソースを作成する関数
	template <typename T>
	void SceneManager<T>::CreateWindowSizeDependentResources()
	{
		if (m_scene) m_scene->CreateWindowSizeDependentResources();
	}

	// デバイスロストした時に呼び出される関数
	template <typename T>
	void SceneManager<T>::OnDeviceLost()
	{
		if (m_scene) m_scene->OnDeviceLost();
	}

	// シーンの削除関数
	template <typename T>
	void SceneManager<T>::DeleteScene()
	{
		if (m_scene)
		{
			m_scene->Finalize();

			m_scene.reset();
		}
	}


	template<typename T>
	inline void SceneManager<T>::PrepareNextScene(std::function<std::unique_ptr<Scene<T>>()> sceneFactory)
	{
		std::stringstream ss{};

		std::string newStateName = "call PrepareNextScene";
		OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());

		if(m_loadingThread.joinable())
		{
			ss << m_loadingThread.get_id();
			newStateName = "in join " + ss.str();
			OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());

			m_loadingThread.join();

			std::stringstream sd{};
			sd << m_loadingThread.get_id();
			newStateName = "end join" + sd.str();
			OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());
		}

		m_isLoading = true;
		std::stringstream se{};
		se << m_loadingThread.get_id();
		newStateName = "exit joinable" + se.str();
		OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());
		

		m_loadingThread = std::thread{
			[=]()
			{



				DeleteScene();
				m_nextScene = sceneFactory();

				std::string newStateName = typeid(*m_nextScene).name();
				OutputDebugString(L"Next Scene -> %ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());

				m_nextScene->SetSceneManager(this);
				m_nextScene->Initialize();
				newStateName = "complete";
				OutputDebugString(L"%ls\n", std::wstring(newStateName.begin(), newStateName.end()).c_str());
				m_loadingMutex.lock();
				m_isLoading = false;
				m_loadingMutex.unlock();
			}
		};
	}

	template<typename T>
	inline bool SceneManager<T>::IsLoading()
	{
		// 確認のタイミングを合わせるため一旦ロックする
		m_loadingMutex.lock();
		bool isLoading = m_isLoading;
		m_loadingMutex.unlock();

		return isLoading;
	}

	template<typename T>
	inline void SceneManager<T>::Finalize()
	{
		DeleteScene();
		// スレッドが動いているかどうかの確認
		if (m_loadingThread.joinable())
		{
			m_loadingThread.join();
		}
	}

}

