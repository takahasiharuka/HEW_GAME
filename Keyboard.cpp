
//このエラーの原因は、「Keyboard.」コードのどこかにあります。
//Error	C2027	use of undefined type 'DirectX::Keyboard::Impl'
//Error	C2338	static_assert failed: 'can't delete an incomplete type'

#include "Keyboard.h"

#include <memory>

// とりあえずエラーが発生する部分はコメントアウトしておきました。

DirectX::Keyboard::Keyboard() noexcept(false)
{
    // コンストラクタ: Keyboard オブジェクトを初期化
}

DirectX::Keyboard::Keyboard(Keyboard&& moveFrom) noexcept
{
    // Moveコンストラクタ: 他のKeyboardオブジェクトからリソースを移動
}

//Keyboard& DirectX::Keyboard::operator=(Keyboard&& moveFrom) noexcept
//{
//    // Move代入演算子: 他のKeyboardオブジェクトからリソースを移動 (未実装)
//}

DirectX::Keyboard::~Keyboard()
{
    // デストラクタ: Keyboard オブジェクトのクリーンアップ
}

//State __cdecl DirectX::Keyboard::GetState() const
//{
//    // 現在のキーボード状態を取得 (未実装)
//    return State();
//}

void __cdecl DirectX::Keyboard::Reset() noexcept
{
    // キーボード状態をリセット
}

bool __cdecl DirectX::Keyboard::IsConnected() const
{
    // キーボードが接続されているかどうかを確認
    return false; // デフォルトで未接続を返す
}

//void __cdecl DirectX::Keyboard::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//    // キーボード関連のWindowsメッセージを処理 (未実装)
//}
//
//void __cdecl DirectX::Keyboard::SetWindow(ABI::Windows::UI::Core::ICoreWindow* window)
//{
//    // キーボード入力に関連付けるウィンドウを設定 (未実装)
//}

void __cdecl DirectX::Keyboard::KeyboardStateTracker::Update(const State& state) noexcept
{
    // 現在のキーボード状態でトラッカーを更新
}

void __cdecl DirectX::Keyboard::KeyboardStateTracker::Reset() noexcept
{
    // トラッカーをリセット
}