/**
 * @file   CollisionTypes.h
 * @brief  当たり判定システムで使用する共通の構造体・クラス定義
 * 
 * メインスレッド用の実体データ(CollisionData)と、
 * サブスレッド判定用のコピーデータ(ThreadCollisionObjectProxy)を分離して定義します。
 * * @author 松下大暉
 * @date   2025/03/22
 */

 // 多重インクルードの防止 =====================================================
#pragma once

#include <vector>
#include <memory>
#include <intrin.h> // _BitScanForward用

// ヘッダファイルの読み込み ===================================================
#include "Game/GameObjects/Common/GameObject.h"
#include "Game/Common/Utillities/Collision/ICollision.h"

// 前方宣言
class GameObject;

/**
 * @brief 衝突通知用インフォメーション
 * 
 *  GameObjectのOnCollisionコールバックへ渡される、衝突相手の詳細情報です。
 */
class CollisionInfo
{
public:
    ICollider* pOtherCollider;  ///< 相手側のコライダー
    GameObject* pOtherObject;   ///< 相手側のゲームオブジェクト
    ICollider* pMyCollider;     ///< 自分側の衝突したコライダー

    CollisionInfo(ICollider* pOtherCollider, GameObject* pOtherObject, ICollider* pMyCollider)
        : pOtherCollider{ pOtherCollider }
        , pOtherObject{ pOtherObject }
        , pMyCollider{ pMyCollider }
    {
    }
};

/**
 * @brief 広域判定（ブロードフェーズ）用の一時データ
 * 
 *  詳細は後ほど実装予定の空間分割等の最適化に使用します。
 */
struct BroadCollision
{
    ICollider* pCollider;       ///< 判定対象のコライダー
    bool       isHit;           ///< 衝突フラグ

    BroadCollision(ICollider* pCollider)
        : pCollider{ pCollider }
        , isHit{ false }
    {
    }
};

/**
 * @brief 衝突判定管理データ（メインスレッド用実体）
 * 
 * CollisionManagerが保持する「持ち主」としてのデータです。
 * ゲームオブジェクトとの参照を保持し、通知の際に使用されます。
 */
struct CollisionData
{
    GameObject* pGameObject;    ///< 衝突通知を送る対象のオブジェクト
    ICollider* pCollider;       ///< 形状データ（本体）

    int         id;             ///< オブジェクト固有のユニークID（プロキシとの紐付けに使用）
    uint32_t    tagBitIndex;    ///< 衝突マトリックス参照用のタグビット位置（高速化用キャッシュ）

    std::vector<int> children;  ///< 子オブジェクト（部位など）のCollisionData IDリスト

    bool isStatic;              ///< 静的オブジェクトフラグ（trueなら動かないものとして判定を最適化）

    CollisionData()
        : pGameObject{ nullptr }
        , pCollider{ nullptr }
        , tagBitIndex{ 0 }
        , children{}
        , id{ -1 }
        , isStatic{ false }
    {
    }

    /**
     * @brief 初期化用コンストラクタ
     * @param pGameObject 紐付けるオブジェクト
     * @param pCollider   使用するコライダー
     * @param isStatic    移動しないオブジェクトかどうか
     */
    CollisionData(GameObject* pGameObject, ICollider* pCollider, bool isStatic)
        : CollisionData{}
    {
        this->isStatic = isStatic;
        this->pGameObject = pGameObject;
        this->pCollider = pCollider;
        if (pGameObject)
        {
            unsigned long index;
            // GameObjectTag(enum)からビットが立っている位置を取得しキャッシュ
            if (_BitScanForward(&index, static_cast<uint32_t>(pGameObject->GetTag())))
            {
                this->tagBitIndex = index;
            }
        }
    }
};

/**
 * @brief 衝突検知結果データ
 * 
 *  サブスレッドでの判定後、メインスレッドに「どのID同士が当たったか」を伝えるための軽量構造体です。
 */
struct DetectedCollisonData
{
    int collisionDataIdA;       ///< 衝突したオブジェクトAのID
    int collisionDataIdB;       ///< 衝突したオブジェクトBのID
};

/**
 * @brief 衝突判定用プロキシ（サブスレッド用コピー）
 * 
 * スレッドセーフに判定を行うため、判定に必要な数値データのみを「スナップショット」として複製したものです。
 * サブスレッドはこのデータのみを参照し、GameObjectには直接アクセスしません。
 */
struct ThreadCollisionObjectProxy
{
    bool isActive;              ///< 有効フラグ（非アクティブなら判定をスキップ）
    GameObjectTag tag;          ///< オブジェクトの種類（タグ）
    uint32_t    tagBitIndex;    ///< 衝突マトリックス参照用のタグビット位置

    int id;                                             ///< 元のCollisionDataと紐付けるためのID
    std::unique_ptr<ICollider> collider;                ///< 形状データのディープコピー
    std::vector<ThreadCollisionObjectProxy> children;   ///< 子要素のプロキシリスト

    ThreadCollisionObjectProxy()
        : id{ -1 }
        , tag{ GameObjectTag::DEFAULT }
        , tagBitIndex{ 0 }
        , isActive{ false }
    {
    }

    /**
     * @brief コピーコンストラクタ（ディープコピー）
     * 
     *  IColliderのGetClone()を使用して、形状データを安全に複製します。
     */
    ThreadCollisionObjectProxy(const ThreadCollisionObjectProxy& origin)
        : id{ origin.id }
        , collider{ }
        , children{ origin.children }
        , tag{ origin.tag }
        , tagBitIndex{ origin.tagBitIndex }
        , isActive{ origin.isActive }
    {
        if (origin.collider)
        {
            collider = origin.collider->GetClone();
        }
    }

    /**
     * @brief 代入演算子（ディープコピー）
     */
    ThreadCollisionObjectProxy& operator=(const ThreadCollisionObjectProxy& origin)
    {
        if (this != &origin) {
            id = origin.id;
            tag = origin.tag;
            tagBitIndex = origin.tagBitIndex;
            isActive = origin.isActive;
            children = origin.children;
            if (origin.collider) {
                collider = origin.collider->GetClone();
            }
            else {
                collider.reset();
            }
        }
        return *this;
    }

    // ムーブセマンティクスを有効にするためのデフォルト定義
    ThreadCollisionObjectProxy(ThreadCollisionObjectProxy&&) noexcept = default;
    ThreadCollisionObjectProxy& operator=(ThreadCollisionObjectProxy&&) noexcept = default;
};