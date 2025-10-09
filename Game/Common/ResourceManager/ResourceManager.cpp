/*****************************************************************//**
 * @file    ResourceManager.cpp
 * @brief   リソース管理クラスのソースファイル
 * 
 * @author  松下大暉
 * @date    2025/4/29
 *********************************************************************/

#include "pch.h"
#include "ResourceManager.h"

using namespace DirectX;

/**
 * @brief コンストラクタ
 * 
 * @param[in] device        デバイス
 * @param[in] audioEngine   オーディオエンジン
 */
ResourceManager::ResourceManager(ID3D11Device* device, DirectX::AudioEngine* audioEngine)
    : m_device(device), m_audioEngine(audioEngine)
{

}

/**
 * @brief テクスチャ作成関数
 * 
 * @param[in] fileNname  ファイル名
 * 
 * @return テクスチャ
 */
ID3D11ShaderResourceView* ResourceManager::CreateTexture(const std::string& fileName)
{
    // ファイルパスの作成
    std::string factoryPath = TEXTURE_FOLDER_PATH;

    std::string filePath = factoryPath + fileName;

    std::wstring fileString = std::wstring(filePath.begin(), filePath.end());
    const wchar_t* name = fileString.c_str();

    auto it = m_textureCache.find(name);

    // キャッシュに存在しなかった場合
    if (it == m_textureCache.end())
    {
        m_mutex.lock();

        // テクスチャの作成
        ID3D11ShaderResourceView* srv;

        // 例外処理
        DX::ThrowIfFailed(
            CreateDDSTextureFromFile(m_device, name, nullptr, &srv)
        );

        TextureCache::value_type v(name, srv);
        m_textureCache.insert(v);
        m_mutex.unlock();
    }

    return m_textureCache.at(name).Get();
}

/**
 * @brief テクスチャの開放
 */
void ResourceManager::ReleaseTexture()
{
   
    for (auto& element : m_textureCache)
    {
        element.second->Release();
    }

    m_textureCache.clear();

}

// モデル作成関数
DirectX::Model ResourceManager::CreateModel(const std::string& fileName, const std::string& addFactoryPath)
{
    // ファイルパスの作成
    std::string filePath = MODEL_FOLDER_PATH + addFactoryPath + fileName;

    std::wstring fileString = std::wstring(filePath.begin(), filePath.end());
    const wchar_t* name = fileString.c_str();

    auto it = m_modelCache.find(name);

    // キャッシュに存在しなかった
    if (it == m_modelCache.end())
    {
        m_mutex.lock();

        wchar_t path[MAX_PATH] = {};
        _wsplitpath_s(name, nullptr, 0, path, MAX_PATH, nullptr, 0, nullptr, 0);
        EffectFactory fx(m_device);
        fx.SetDirectory(path);
        // モデルの作成
        DirectX::ModelLoaderFlags flags = DirectX::ModelLoader_Clockwise | DirectX::ModelLoader_IncludeBones;

        std::unique_ptr<DirectX::Model> model = Model::CreateFromSDKMESH(m_device, name, fx, flags);
        ModelCache::value_type v(name, std::move(model));
        m_modelCache.insert(std::move(v));

        m_mutex.unlock();

    }

    return *(m_modelCache.at(name).get());
}

// モデルの解放関数
void ResourceManager::ReleaseModel()
{
    m_modelCache.clear();   
}

// サウンドエフェクト作成関数
DirectX::SoundEffect* ResourceManager::CreateSoundEffect(const std::string& fileName)
{

    // ファイルパスの作成
    std::string filePath = SOUND_FOLDER_PATH +  fileName;

    std::wstring fileString = std::wstring(filePath.begin(), filePath.end());
    const wchar_t* name = fileString.c_str();

    auto it = m_soundCache.find(name);

    // キャッシュに存在しなかった
    if (it == m_soundCache.end())
    {
        m_mutex.lock();

        wchar_t path[MAX_PATH] = {};
        _wsplitpath_s(name, nullptr, 0, path, MAX_PATH, nullptr, 0, nullptr, 0);
        EffectFactory fx(m_device);
        fx.SetDirectory(path);
        // サウンドエフェクトの作成
        std::unique_ptr<DirectX::SoundEffect> sound = std::make_unique<SoundEffect>(m_audioEngine, name);
        SoundCache::value_type v(name, std::move(sound));
        m_soundCache.insert(std::move(v));

        m_mutex.unlock();

    }

    return m_soundCache.at(name).get();
}

// サウンドエフェクトの解放関数
void ResourceManager::ReleaseSoundEffect(const wchar_t* name)
{
    if (!m_soundCache.count(name)) return;

    m_soundCache.erase(name);
}

/**
 * @brief 全リソースの開放
 * 
 */
void ResourceManager::Clear()
{
    m_mutex.lock();

    ReleaseTexture();
    ReleaseModel();

    m_mutex.unlock();

}

#if 0
// ファイルの種類を取得する関数
ResourceManager::Kind ResourceManager::GetFileKind(const std::wstring& name)
{
    size_t dotPos = name.rfind(L'.');
    if (dotPos == std::wstring::npos)
    {
        // '.'が存在しない場合
        return Kind::UNKNOWN;
    }

    // 拡張子の抜き出し
    std::wstring extension = name.substr(dotPos + 1);

    // 拡張子を小文字に変える
    std::transform(extension.begin(), extension.end(), extension.begin(), std::towlower);

    // 拡張子を判定する
    if (extension == L"dds") return Kind::DDS;
    if (extension == L"cmo") return Kind::CMO;

    return Kind::UNKNOWN;
}
#endif
