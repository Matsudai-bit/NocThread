/**
 * @file    Screen.cpp
 *
 * @brief   ゲーム画面に関する実装ファイル
 *
 * @author 松下大暉
 *
 * @date    2025/07/11 (修正日)
 */

#include "pch.h" 
#include "Screen.h" // Screen.h をインクルード

 // 静的メンバーの定義
std::unique_ptr<Screen> Screen::s_screen = nullptr;

/**
 * @brief Screenクラスのプライベートコンストラクタ。
 * シングルトンパターンを強制するため、外部からの直接呼び出しはできません。
 * 画面の幅と高さを0.0fで初期化します。
 */
Screen::Screen()
    : m_width{ 0.0f }, m_height{ 0.0f }
    , m_screenScale{}
{
}

/**
 * @brief Screenクラスのシングルトンインスタンスを取得します。
 * インスタンスがまだ生成されていない場合は新しく生成します。
 * @return Screenクラスのシングルトンインスタンスへのポインタ。
 */
Screen* Screen::Get()
{
    if (!s_screen)
    {
        s_screen.reset(new Screen());
    }
    return s_screen.get();
}

/**
 * @brief 画面の幅と高さを設定します。
 * アプリケーションの初期化時や、ウィンドウのリサイズ、
 * フルスクリーンモードへの切り替え時などに呼び出します。
 * @param width 設定する画面の幅 (ピクセル単位、float型)。
 * @param height 設定する画面の高さ (ピクセル単位、float型)。
 */
void Screen::SetScreenSize(float width, float height)
{
    m_width = width;
    m_height = height;

    // 画面サイズに対する相対的なスケールに調整
    m_screenScale =  std::sqrtf(m_width * m_width + m_height * m_height) / std::sqrtf(1280 * 1280 + 720 * 720);
}

/**
 * @brief 現在設定されている画面の幅を整数値で取得します。
 * @return 画面の幅 (ピクセル単位、int型)。
 */
int Screen::GetWidth() const { return static_cast<int>(m_width); }

/**
 * @brief 現在設定されている画面の高さを整数値で取得します。
 * @return 画面の高さ (ピクセル単位、int型)。
 */
int Screen::GetHeight() const { return static_cast<int>(m_height); }

/**
 * @brief 画面の上端の座標を整数値で取得します。
 * 常に0を返します。
 * @return 画面の上端の座標 (ピクセル単位、int型)。
 */
int Screen::GetTop() const { return 0; }

/**
 * @brief 現在設定されている画面の下端の座標を整数値で取得します。
 * GetHeight() と同じ値を返します。
 * @return 画面の下端の座標 (ピクセル単位、int型)。
 */
int Screen::GetBottom() const { return GetHeight(); }

/**
 * @brief 画面の左端の座標を整数値で取得します。
 * 常に0を返します。
 * @return 画面の左端の座標 (ピクセル単位、int型)。
 */
int Screen::GetLeft() const { return 0; }

/**
 * @brief 現在設定されている画面の右端の座標を整数値で取得します。
 * GetWidth() と同じ値を返します。
 * @return 画面の右端の座標 (ピクセル単位、int型)。
 */
int Screen::GetRight() const { return GetWidth(); }

/**
 * @brief 現在設定されている画面の中央X座標を整数値で取得します。
 * @return 画面の中央X座標 (ピクセル単位、int型)。
 */
int Screen::GetCenterX() const { return GetWidth() / 2; }

/**
 * @brief 現在設定されている画面の中央Y座標を整数値で取得します。
 * @return 画面の中央Y座標 (ピクセル単位、int型)。
 */
int Screen::GetCenterY() const { return GetHeight() / 2; }

/**
 * @brief 現在設定されている画面の幅を浮動小数点数 (float) で取得します。
 * @return 画面の幅 (ピクセル単位、float型)。
 */
float Screen::GetWidthF() const { return m_width; }

/**
 * @brief 現在設定されている画面の高さを浮動小数点数 (float) で取得します。
 * @return 画面の高さ (ピクセル単位、float型)。
 */
float Screen::GetHeightF() const { return m_height; }

/**
 * @brief 画面の上端の座標を浮動小数点数 (float) で取得します。
 * 常に0.0fを返します。
 * @return 画面の上端の座標 (ピクセル単位、float型)。
 */
float Screen::GetTopF() const { return 0.0f; }

/**
 * @brief 現在設定されている画面の下端の座標を浮動小数点数 (float) で取得します。
 * GetHeightF() と同じ値を返します。
 * @return 画面の下端の座標 (ピクセル単位、float型)。
 */
float Screen::GetBottomF() const { return m_height; }

/**
 * @brief 画面の左端の座標を浮動小数点数 (float) で取得します。
 * 常に0.0fを返します。
 * @return 画面の左端の座標 (ピクセル単位、float型)。
 */
float Screen::GetLeftF() const { return 0.0f; }

/**
 * @brief 現在設定されている画面の右端の座標を浮動小数点数 (float) で取得します。
 * GetWidthF() と同じ値を返します。
 * @return 画面の右端の座標 (ピクセル単位、float型)。
 */
float Screen::GetRightF() const { return m_width; }

/**
 * @brief 現在設定されている画面の中央X座標を浮動小数点数 (float) で取得します。
 * @return 画面の中央X座標 (ピクセル単位、float型)。
 */
float Screen::GetCenterXF() const { return m_width / 2.0f; }

/**
 * @brief 現在設定されている画面の中央Y座標を浮動小数点数 (float) で取得します。
 * @return 画面の中央Y座標 (ピクセル単位、float型)。
 */
float Screen::GetCenterYF() const { return m_height / 2.0f; }

float Screen::GetScreenScale() const
{
    return m_screenScale;
}
