---
title: 礦坑系列 ── Small String Optimization (SSO)
date: 2022-05-22
description: 核心概念是編譯器通常會先分配一小塊 stack 段的記憶體給 std::string，如果字串夠短就可以直接塞到這個 stack 內，如果字串太長，塞不下，就拿這個空間放 pointer，指向 heap 段存取的字串
tags: C++ Miner-BlackMagic
categories:
- C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 ── Small String Optimization (SSO) <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# Small String Optimization (SSO)

最近由於讀書會又把這個主題拿出來講了，想想再把這篇補得更詳細一點好了，因此就以 Raymond Chen 的文章為主體翻譯一下，再多補充一些資訊

## std::string

`std::string` 的起點是這樣的：

```cpp
template <typename T>
struct basic_string {
    T* ptr;
    size_t size;
    size_t capacity;
}
```

- `ptr`：指向字串起始位址的指標
- `size`：字串的字元數，<span class = "yellow">不包含 `'\0'`</span>
  - 這個「不包含 `'\0'`」是一個比較巧妙的設計，編譯器會利用這點來做一些優化，後面會提
- `capacity`：字串的容量，一樣<span class = "yellow">不包含 `'\0'`</span>

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/tree/standard-markdown/Miner_BlackMagic/SSO/string-begin-layout.png?raw=true">  

（layout 示意圖，[img src](https://devblogs.microsoft.com/oldnewthing/20230803-00/?p=108532)）

</center>

從這邊你會發現一個問題，也是通常我們不太喜歡 string 的原因 ── 其有 heap allocation 的操作，因此可能會造成效能上的影響

## SSO

對於這個問題，編譯器便希望針對長度較短的字串去做優化，這個優化就被稱為 SSO。 要注意這是編譯器的實作，因此並不是標準，換句話說 SSO 是 <span class = "yellow">optional</span> 的，不一定會有這個行為，但大多數的編譯器都有實作

核心概念就是我們可以先分配一小塊 stack 段的記憶體，如果字串夠短就可以直接將其塞到這塊記憶體內，如果字串太長，塞不下，就拿這個空間放指向 heap 段字串的 pointer

因此我們就可以更新一下上方的實作：

```cpp
template<typename T>
struct basic_string
{
    static constexpr size_t BUFSIZE = 8;
    T* ptr;
    size_t size;
    size_t capacity;
    T buf[BUFSIZE]; // special storage for short strings
};
```

如果字串的容量小於等於我們設定的值，以這邊來說是 `8`，那就可以將 `ptr` 指向內部的 `buf`，從而省去 heap allocation，這樣有一個很大的好處是我們不需要任何的 heap allocation 就可以建立空字串了

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/tree/standard-markdown/Miner_BlackMagic/SSO/basic-string-layout.png?raw=true">

（layout 示意圖，[img src](https://devblogs.microsoft.com/oldnewthing/20230803-00/?p=108532)）

</center>

至於 buffer 的大小具體要是多少就要看你的編譯器實作了，太大會導致記憶體的浪費，太小則會增加程式碼的複雜度。 在 VS2019 的 msvc 裡是 15 個字，而在我的環境上(mingw-gcc 11.2.0) 也是 15。

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
```

而會選擇去使用 const char pointer 來減少負擔，然而以這個例子來講，因為這個字串長度很短，所以其實根本不會使用到 heap 段，負擔也很小。

## 使用 enumeration 包裝變數

### gcc

而在思考 buffer 大小時，gcc 意識到如果 `std::string` 內部是使用 stack 上的 buffer，那就不需要再紀錄 `capacity` 了，因為我們已經知道它是 `BUFSIZE - 1` 了

因此 gcc 的實作中使用 enumeration 來包裝 `capacity` 與 `buf`，畢竟每次都只會需要其中一個：

```cpp
template<typename T>
struct basic_string
{
    static constexpr size_t BUFSIZE = 16 / sizeof(T);
    T* ptr;
    size_t size;
    union {
        size_t capacity;
        T buf[BUFSIZE]; // special storage for short strings
    } storage;
};
```

這裡的 `BUFSIZE` 設為 `16 / sizeof(T)`，也是目前比較常見的數字，對於 `char`，它可以容納 15 個字元（少了一個是因為要拿來放 `'\0'`）

### msvc

而對於 msvc，他們的想法則是透過 `capacity` 來確認容量是否小於 `BUFSIZE - 1`，如果是，那就代表目前使用的是 stack 上的 buffer，從而拋棄了 `ptr`，反之則拋棄 `buf` 留下 `ptr`：

```cpp
template<typename T>
struct basic_string
{
    static constexpr size_t BUFSIZE = 16 / sizeof(T);
    union {
        T* ptr;
        T buf[BUFSIZE];
    } storage;
    size_t size;
    size_t capacity;
};
```

### clang

clang 的實作就比較複雜了，他寫了兩個結構體：

```cpp
template<typename T>
struct basic_string_large
{
    size_t capacity; // always multiple of 2
    size_t size;
    T* ptr;
};
```

這裡的實作要求 `capacity` 需要 round up 為偶數，因此最低的一個 bit 總是為 `0`（才會是偶數）

```cpp
template<typename T>
struct basic_string_small
{
    unsigned char is_small:1; // always set for small strings
    unsigned char size:7;
    T buf[BUFSIZE];
};
```

這裡使用到的語法叫做 [bit-field](https://en.cppreference.com/w/cpp/language/bit_field)，主要就是要把最低的一個 bit 設為 `1`，然後將以 7 個 bit 拿來存 `size`。 bit-field 不是我們的重點，所以就不展開敘述了，

因此對於 little-endian 的系統，它以第一個 byte 的最低一個 bit（lowest-order bit of the first byte），如果第一個 bit 已經被清除了，那會使用 `large` 的版本，進行 heap allocation，反之則代表正在使用 SSO 的版本

在 `BUFSIZE` 的選擇上會注意不讓 `basic_string_small` 的大小超過 `basic_string_large`。 一般來說 `size_t` 與 pointer 在 32-bit 的系統上是 4 byte，在 64-bit 的系統上是 8 byte，因此 `basic_string_large` 的大小在 32-bit 的系統上是 12 byte，在 64-bit 的系統上是 24 byte

而 `T` 通常為 `CharT`，所以對於 64 位元的系統來說，如果 `sizeof(CharT)` 為 1，則 `BUFSIZE` 為 23，如果 `sizeof(CharT)` 為 2，則 `BUFSIZE` 為 11

### 小節

因此如果把三個編譯器的實作簡化再簡化，就會如下：

```cpp
// gcc
struct string
{
    char* ptr;
    size_t size;
    union {
        size_t capacity;
        char buf[16];
    };

    bool is_large() { return ptr != buf; }
    auto data() { return ptr; }
    auto size() { return size; }
    auto capacity() { return is_large() ? capacity : 15; }
};

// msvc
struct string
{
    union {
        char* ptr;
        char buf[16];
    };
    size_t size;
    size_t capacity;

    bool is_large() { return capacity > 15; }
    auto data() { return is_large() ? ptr : buf; }
    auto size() { return size; }
    auto capacity() { return capacity; }
};

// clang
union string
{
    struct large {
        unsigned char is_large : 1;
        size_t capacity : 63;
        char* data;
    };

    struct small {
        unsigned char is_large : 1;
        unsigned char size : 7;
        char data[sizeof(large) - 1];
    };

    union {
        small s;
        large l;
    };

    bool is_large() const { return l.is_large; }
    const char* data() const { return is_large() ? l.data : s.data; }
    size_t size() const { return is_large() ? l.capacity : s.size; }
    size_t capacity() const { return is_large() ? l.capacity : sizeof(s.data); }
};
```

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

# Reference

- [Small String Optimization in C++](https://www.youtube.com/watch?v=S7oVXMzTo4w)
- [Inside STL: The string STL](https://devblogs.microsoft.com/oldnewthing/20230803-00/?p=108532)