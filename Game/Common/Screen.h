/**
 * @file    Screen.h
 *
 * @brief   ゲーム画面に関するヘッダファイル
 *
 * @author 松下大暉
 *
 * @date    2025/07/11 (修正日)
 */

 // 多重インクルードの防止 =====================================================
#pragma once

#include <memory> // std::unique_ptr のために必要

// クラスの定義 ===============================================================
/**
 * @brief 画面
 */
class Screen final
{
private:
    // 画面の幅
    float m_width;
    // 画面の高さ
    float m_height;

    // 画面サイズに対する相対的なスケール
    float m_screenScale;

private:
    // コンストラクタ
    Screen();

    // コピーコンストラクタと代入演算子
    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    // シングルトンインスタンスへのポインタ
    static std::unique_ptr<Screen> s_screen;

public:
    // シングルトンインスタンスを取得
    static Screen* Get();

    // 画面サイズを設定
    void SetScreenSize(float width, float height);

    // 画面の幅を取得 (int型)
    int GetWidth() const;

    // 画面の高さを取得 (int型)
    int GetHeight() const;

    // 画面の上端を取得 (int型)
    int GetTop() const;

    // 画面の下端を取得 (int型)
    int GetBottom() const;

    // 画面の左端を取得 (int型)
    int GetLeft() const;

    // 画面の右端を取得 (int型)
    int GetRight() const;

    // 画面の中央(X座標)を取得 (int型)
    int GetCenterX() const;

    // 画面の中央(Y座標)を取得 (int型)
    int GetCenterY() const;

    // 画面の幅を取得 (float型)
    float GetWidthF() const;

    // 画面の高さを取得 (float型)
    float GetHeightF() const;

    // 画面の上端を取得 (float型)
    float GetTopF() const;

    // 画面の下端を取得 (float型)
    float GetBottomF() const;

    // 画面の左端を取得 (float型)
    float GetLeftF() const;

    // 画面の右端を取得 (float型)
    float GetRightF() const;

    // 画面の中央(X座標)を取得 (float型)
    float GetCenterXF() const;

    // 画面の中央(Y座標)を取得 (float型)
    float GetCenterYF() const;

    // 画面の相対的なスケールを取得
    float GetScreenScale() const;
};