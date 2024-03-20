---
title: 礦坑系列 ── Small String Optimization (SSO)
date: 2022-05-22
tags: C++ Miner-BlackMagic
categories:
- C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 ── Small String Optimization (SSO) <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# Small String Optimization (SSO)

source：[Small String Optimization in C++](https://www.youtube.com/watch?v=S7oVXMzTo4w)

首先要注意 SSO 是 <span class = "yellow">optional</span> 的，也就是不被保證，但在大多數的編譯器都有實作。

通常我們不太喜歡 string 的原因是其常常會試圖去 allocate 記憶體空間，而 heap allocation 又可能會造成效能上的影響，因此 C++ 內的 `std::string` 有一項針對長度較短的字串去做優化，就是標題所寫的 SSO。

核心概念就是我們可以先分配一小塊 stack 段的記憶體，如果字串夠短就可以直接塞到這個 stack 內，如果字串太長，塞不下，就拿這個空間放 pointer，指向 heap 段存取的字串。

而字串長度具體是多少以下才可以去做優化這就要看你用的 lib 了，在 VS2019 的 msvc 裡是 15 個字，而在我的環境上(mingw-gcc 11.2.0) 也是 15。

很多人可能會覺得 `std::string` 會有 heap allocation 因此不去用他：

```cpp
#include <iostream>
#include <string>

int main()
{
  // some people think there this is kind of implication of a heap allocation
  std::string name = "Name";

  // so they tend to use char pointer
  const char *name_ptr = "Name";    // Don't forget the `const`
}
````

而會選擇去使用 const char pointer 來減少負擔，然而以這個例子來講，因為這個字串長度很短，所以其實根本不會使用到 heap 段，負擔也很小。

## 測試 & 實例

避免有人不想看內部 code，就先放例子，這裡我是用 Compiler Explorer gcc 12.1 來測的 ([網址](https://godbolt.org/z/qajh5PeKc))：

```cpp
#include <iostream>
#include <string>

void *operator new(size_t size)
{
  std::cout << "Allocating " << size << " bytes\n";
  return malloc(size);
}

int main()
{
  puts("str:");
  std::string str = "123456789012345";    // 15 words

  puts("str2:");
  std::string str2 = "1234567890123456";    // 16 words
}
```

輸出：

```cpp
str:
str2:
Allocating 17 bytes
```

上面我們 overload 了 `new` operator，因此當它要 allocate heap 空間時就會多輸出一段文字，而我們可以看見 `str` 因為只有 15 個字，所以不會分配 heap 段的記憶體，但 `str2` 就有了，這裡分配的是 17 bytes，clang 14.0.0 也是分配 17 bytes，然而 VS2019 的 msvc 則是 32 bytes。

如果你的環境上沒有跑出一樣的結果大概是因為你那邊它有自己的 extension 我猜，如 tcc、icc file 之類的，內部實作可能就有些許差異，又或者你是在 Debug 模式下輸出的結果也有可能不同。

## 挖礦，入坑看魔法

在一開始先說一下，<span class = "yellow">下面的 code 只是其中一種實作方式</span>，不代表每個 lib 都是這樣做的，後面就以我的環境為例帶大家看一下 code。

如果我們進到 `std::string` 的內部去看會發現 `std::string` 是一種元素為 `char` 的 `basic_string`：

```cpp
template <class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
class basic_string { // null-terminated transparent array of elements
  // lot of contents...
};

// many lines...

using string  = basic_string<char, char_traits<char>, allocator<char>>;
```

而當我們去找這裡對應的建構子，會看見這個：

```cpp
_CONSTEXPR20 basic_string(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count)
    : _Mypair(_Zero_then_variadic_args_t{}) {
    auto&& _Alproxy = _GET_PROXY_ALLOCATOR(_Alty, _Getal());
    _Container_proxy_ptr<_Alty> _Proxy(_Alproxy, _Mypair._Myval2);
    _Tidy_init();
    assign(_Ptr, _Count);
    _Proxy._Release();
}
```

以 `std::stirng` 來說，就是吃一個 const char pointer，這裡的重點是 `assign(_Ptr, _Count);` 函式，`_Ptr` 指的是實際的 pointer，只像我們的 `"Name"`，而 `_Count` 則是有幾個字，以這裡來說會是 4。

我們再繼續往這個 `assign` 函式看，會看見它長這樣：

```cpp
_CONSTEXPR20 basic_string& assign(
    _In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        // assign [_Ptr, _Ptr + _Count)
    if (_Count <= _Mypair._Myval2._Myres) {
        _Elem* const _Old_ptr   = _Mypair._Myval2._Myptr();
        _Mypair._Myval2._Mysize = _Count;
        _Traits::move(_Old_ptr, _Ptr, _Count);
        _Traits::assign(_Old_ptr[_Count], _Elem());
        return *this;
    }

    return _Reallocate_for(
        _Count,
        [](_Elem* const _New_ptr, const size_type _Count, const _Elem* const _Ptr) {
            _Traits::copy(_New_ptr, _Ptr, _Count);
            _Traits::assign(_New_ptr[_Count], _Elem());
        },
        _Ptr);
}
```

你會看見裡面有一個 `if` 判斷式，如果 `_Count` 小於某個值，那就會拿到 stack 段 buffer 的指標 `_Old_ptr`，並直接把我們的字串移動到 buffer 內，然後 return，因此完全<span class = "yellow">沒有多餘的 allocation</span>。

但如果它沒有進到上面那個 if-statement，也就是 `_Count` 比某個值還大，那就會去用到下面那個 `_Reallocate_for` 函式，這個 function 內有一行是這個：

```cpp
const pointer _New_ptr = _Al.allocate(_New_capacity + 1); // throws
```

也就是說去動到了 allocation。

而這個關鍵的值 `_Myres` 在一個叫做 `_String_val` 的 class 裡面，這個 class 的最下面有著這兩行：

```cpp
size_type _Mysize = 0; // current length of string
size_type _Myres  = 0; // current storage reserved for string
```

而關於 `_Myres` 的賦值在這裡：

```cpp
void _Become_small() {
    // release any held storage and return to small string mode
    // pre: *this is in large string mode
    // pre: this is small enough to return to small string mode
    // (not _CONSTEXPR20; SSO should be disabled in a constexpr context)

    _Mypair._Myval2._Orphan_all();
    const pointer _Ptr = _Mypair._Myval2._Bx._Ptr;
    auto& _Al          = _Getal();
    _Destroy_in_place(_Mypair._Myval2._Bx._Ptr);
    _Traits::copy(_Mypair._Myval2._Bx._Buf, _Unfancy(_Ptr), _Mypair._Myval2._Mysize + 1);
    _Al.deallocate(_Ptr, _Mypair._Myval2._Myres + 1);
    _Mypair._Myval2._Myres = _BUF_SIZE - 1;          // 這行
}
```

在最下面你可以看見它被設定為 `_BUF_SIZE-1` 的大小，而 `_BUF_SIZE` 的大小則定義在 `basic_stirng` 裡面：

```cpp
static constexpr auto _BUF_SIZE   = _Scary_val::_BUF_SIZE;
static constexpr auto _ALLOC_MASK = _Scary_val::_ALLOC_MASK;
```

而 `_Scary_val::_BUF_SIZE` 則定義在 `_String_val` 裡面：

```cpp
// length of internal buffer, [1, 16]:
static constexpr size_type _BUF_SIZE = 16 / sizeof(value_type) < 1 ? 1 : 16 / sizeof(value_type);
```

以我們的例子來說會是 16，而 `_Myres` 的值還需要再 -1，也就是 15，因此當超過 15 個字的時候就會呼叫 heap allocation。
