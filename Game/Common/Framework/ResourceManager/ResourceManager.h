/*****************************************************************//**
 * @file    ResourceManager.h
 * @brief   リソース管理のヘッダーファイル
 * 
 * @author  松下大暉
 * @date    2025/4/29
 *********************************************************************/
#pragma once

#include <unordered_map>
#include <string>
#include <mutex>
/**
 * @brief リソース管理
 */
class ResourceManager final
{
// エイリアス宣言　-----------------------------
private:

	using TextureCache = std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>;	// テクスチャのキャッシュ
	using ModelCache = std::unordered_map<std::wstring, std::unique_ptr<DirectX::Model>>;						// モデルのキャッシュ
	using SoundCache = std::unordered_map<std::wstring, std::unique_ptr<DirectX::SoundEffect>>;					// サウンドのキャッシュ

// クラス定数の宣言　-----------------------------
	const static constexpr char* TEXTURE_FOLDER_PATH = "Resources/Textures/";
	const static constexpr char* MODEL_FOLDER_PATH = "Resources/Models/";
	const static constexpr char* SOUND_FOLDER_PATH = "Resources/Sounds/";

private:


private:

	//** ファイルの種類
	enum class Kind
	{
		UNKNOWN,
		DDS,
		CMO,
		WAV,
	};

	ID3D11Device* m_device;					///< デバイス

	DirectX::AudioEngine* m_audioEngine;	///< オーディオエンジンへのポインタ


	TextureCache m_textureCache;			///< テクスチャキャッシュ

	ModelCache m_modelCache;				///< モデルキャッシュ

	SoundCache m_soundCache;				///< サウンドキャッシュ

	std::mutex m_mutex;

//	コンストラクタ・デストラクタ
public:
	// コンストラクタ
	ResourceManager(ID3D11Device* device, DirectX::AudioEngine* audioEngine);

	// デストラクタ
	~ResourceManager() = default; 

public:

	// ファイル名から種類を判定する
	Kind GetFileKind(const std::wstring& name);

	// テクスチャ作成関数
	ID3D11ShaderResourceView* CreateTexture(const std::string& fileName);

	// テクスチャの解放関数
	void ReleaseTexture();

	// モデル作成関数
	DirectX::Model CreateModel(const std::string& fileName, const std::string& addFactoryPath = std::string());

	// モデルの解放関数
	void ReleaseModel();

	// サウンドエフェクト作成関数
	DirectX::SoundEffect* CreateSoundEffect(const std::string& fileName);

	// サウンドエフェクトの解放関数
	void ReleaseSoundEffect();

	// 全リソースの開放
	void Clear();
};

