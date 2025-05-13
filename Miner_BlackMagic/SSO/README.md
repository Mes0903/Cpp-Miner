---
title: 礦坑系列 ── Small String Optimization (SSO)
date: 2022-05-22
tag: C++ Miner-BlackMagic
category: C++ Miner
---

# 礦坑系列 ── Small String Optimization (SSO)

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

## Small String Optimization (SSO)

最近由於讀書會又把這個主題拿出來講了，想想再把這篇補得更詳細一點好了，因此就以 Raymond Chen 的文章為主體翻譯一下，再多補充一些資訊

### std::string

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

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_BlackMagic/SSO/image/string-begin-layout.png">  

（layout 示意圖，[img src](https://devblogs.microsoft.com/oldnewthing/20230803-00/?p=108532)）

</div>

從這邊你會發現一個問題，也是通常我們不太喜歡 string 的原因 ── 其有 heap allocation 的操作，因此可能會造成效能上的影響

### SSO

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

<div class = "center-column">

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_BlackMagic/SSO/image/basic-string-layout.png">

（layout 示意圖，[img src](https://devblogs.microsoft.com/oldnewthing/20230803-00/?p=108532)）

</div>

至於 buffer 的大小具體要是多少就要看你的編譯器實作了，太大會導致記憶體的浪費，太小則會增加程式碼的複雜度。 在 VS2019 的 msvc 裡是 15 個字，而在我的環境上(mingw-gcc 11.2.0) 也是 15

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

而會選擇去使用 const char pointer 來減少負擔，然而以這個例子來講，因為這個字串長度很短，所以其實根本不會使用到 heap 段，負擔也很小

### 使用 union 包裝變數

#### gcc

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

#### msvc

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

#### clang

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

#### 小節

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
struct string
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

### 測試 & 實例

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

上面我們 overload 了 `new` operator，因此當它要 allocate heap 空間時就會多輸出一段文字，而我們可以看見 `str` 因為只有 15 個字，所以不會分配 heap 段的記憶體，但 `str2` 就有了，這裡分配的是 17 bytes，clang 14.0.0 也是分配 17 bytes，然而 VS2019 的 msvc 則是 32 bytes

如果你的環境上沒有跑出一樣的結果大概是因為你那邊它有自己的 extension 我猜，如 tcc、icc file 之類的，內部實作可能就有些許差異，又或者你是在 Debug 模式下輸出的結果也有可能不同

### 入坑挖礦

#### MSVC ([src code](https://github.com/microsoft/STL/blob/main/stl/inc/xstring))

底下 msvc 版本：

```
Microsoft (R) C/C++ Optimizing Compiler Version 19.41.34120 for x64
Copyright (C) Microsoft Corporation.  著作權所有，並保留一切權利
```

之前有寫過一個舊版的，但我今天發現新版的 code 乾淨很多，所以就更新一下，想看舊的話去 github 翻一下記錄吧，這邊就不留了XD

如果我們進到 `std::string` 的內部去看會發現 `std::string` 是一種元素為 `char` 的 `basic_string`：

```cpp
_EXPORT_STD template <class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
class basic_string { // null-terminated transparent array of elements
  // lot of contents...
}; // end of basic_string

// many lines...

_EXPORT_STD using string  = basic_string<char, char_traits<char>, allocator<char>>;
```

我們先來看裡面其中比較重要的一個 class `_String_val`，裡面放了一些重要的變數與成員函式：
```cpp
template <class _Val_types>
class _String_val : public _Container_base {
// ...
    static constexpr size_type _BUF_SIZE = 16 / sizeof(value_type) < 1 ? 1 : 16 / sizeof(value_type);
// ...
    static constexpr size_type _Small_string_capacity = _BUF_SIZE - 1;

    _NODISCARD _CONSTEXPR20 value_type* _Myptr() noexcept {
        value_type* _Result = _Bx._Buf;
        if (_Large_mode_engaged()) {
            _Result = _Unfancy(_Bx._Ptr);
        }

        return _Result;
    }
// ...
    _NODISCARD _CONSTEXPR20 bool _Large_mode_engaged() const noexcept {
        return _Myres > _Small_string_capacity;
    }
// ...
  union _Bxty { // storage for small buffer or pointer to larger one
        // This constructor previously initialized _Ptr. Don't rely on the new behavior without
        // renaming `_String_val` (and fixing the visualizer).
        _CONSTEXPR20 _Bxty() noexcept : _Buf() {} // user-provided, for fancy pointers
        _CONSTEXPR20 ~_Bxty() noexcept {} // user-provided, for fancy pointers

        value_type _Buf[_BUF_SIZE];
        pointer _Ptr;
        char _Alias[_BUF_SIZE]; // TRANSITION, ABI: _Alias is preserved for binary compatibility (especially /clr)

        _CONSTEXPR20 void _Switch_to_buf() noexcept {
            _STD _Destroy_in_place(_Ptr);

#if _HAS_CXX20
            // start the lifetime of the array elements
            if (_STD is_constant_evaluated()) {
                for (size_type _Idx = 0; _Idx < _BUF_SIZE; ++_Idx) {
                    _Buf[_Idx] = value_type();
                }
            }
#endif // _HAS_CXX20
        }
    };
    _Bxty _Bx;

    // invariant: _Myres >= _Mysize, and _Myres >= _Small_string_capacity (after string's construction)
    // neither _Mysize nor _Myres takes account of the extra null terminator
    size_type _Mysize = 0; // current length of string (size)
    size_type _Myres  = 0; // current storage reserved for string (capacity)
};
```

可以看見裡面定義了 `size`、`capacity` 與最重要的 union 實例 `_Bx`，union 內包含了 pointer 與 buffer。 然後我們也可以看到在 `_Myptr` 內他會根據大小來去回傳 buffer 的位址，或是 pointer 存的值

接著我們回來看 `basic_string` 的建構子，我們在意的是以字串為參數來建構的版本：

```cpp
_CONSTEXPR20 basic_string(_In_z_ const _Elem* const _Ptr) : _Mypair(_Zero_then_variadic_args_t{}) {
    _Construct<_Construct_strategy::_From_ptr>(_Ptr, _Convert_size<size_type>(_Traits::length(_Ptr)));
}
```

雖然還有超級多其他的建構子，但這兩個是我們比較關心的版本。 這裡可以看見他把指標本身與字串的長度當作參數呼叫了 `_Construct`，所以接著看 `_Construct`：

```cpp
template <_Construct_strategy _Strat, class _Char_or_ptr>
_CONSTEXPR20 void _Construct(const _Char_or_ptr _Arg, _CRT_GUARDOVERFLOW const size_type _Count) {
// ...
    if (_Count <= _Small_string_capacity) {
        _My_data._Mysize = _Count;
        _My_data._Myres  = _Small_string_capacity;

        if constexpr (_Strat == _Construct_strategy::_From_char) {
            _Traits::assign(_My_data._Bx._Buf, _Count, _Arg);
            _Traits::assign(_My_data._Bx._Buf[_Count], _Elem());
        } else if constexpr (_Strat == _Construct_strategy::_From_ptr) {
            _Traits::copy(_My_data._Bx._Buf, _Arg, _Count);
            _Traits::assign(_My_data._Bx._Buf[_Count], _Elem());
        } else { // _Strat == _Construct_strategy::_From_string
#ifdef _INSERT_STRING_ANNOTATION
            _Traits::copy(_My_data._Bx._Buf, _Arg, _Count + 1);
#else // ^^^ _INSERT_STRING_ANNOTATION / !_INSERT_STRING_ANNOTATION vvv
            _Traits::copy(_My_data._Bx._Buf, _Arg, _BUF_SIZE);
#endif // ^^^ !_INSERT_STRING_ANNOTATION ^^^
        }

        _Proxy._Release();
        return;
    }

    size_type _New_capacity = _Calculate_growth(_Count, _Small_string_capacity, max_size());
    const pointer _New_ptr  = _Allocate_for_capacity(_Al, _New_capacity); // throws
    _Construct_in_place(_My_data._Bx._Ptr, _New_ptr);

    _My_data._Mysize = _Count;
    _My_data._Myres  = _New_capacity;
    if constexpr (_Strat == _Construct_strategy::_From_char) {
        _Traits::assign(_Unfancy(_New_ptr), _Count, _Arg);
        _Traits::assign(_Unfancy(_New_ptr)[_Count], _Elem());
    } else if constexpr (_Strat == _Construct_strategy::_From_ptr) {
        _Traits::copy(_Unfancy(_New_ptr), _Arg, _Count);
        _Traits::assign(_Unfancy(_New_ptr)[_Count], _Elem());
    } else { // _Strat == _Construct_strategy::_From_string
        _Traits::copy(_Unfancy(_New_ptr), _Arg, _Count + 1);
    }

    _ASAN_STRING_CREATE(*this);
    _Proxy._Release();
}
```

你會看見裡面有一個分支檢查 `_Count` 是否小於 `_Small_string_capacity`，如果是，那就會對 `_Buf` 賦值，然後 return，因此完全<span class = "yellow">沒有多餘的 allocation</span>

而如果 `_Count` 大於於 `_Small_string_capacity`，則會利用 `_Allocate_for_capacity` 與 `_Construct_in_place` 分配記憶體空間，然後再賦值給 `_Ptr`(也就是 `_New_ptr`)

`_Allocate_for_capacity` 內會利用 allocator 分配記憶體空間：

```cpp
enum class _Allocation_policy { _At_least, _Exactly };

template <_Allocation_policy _Policy = _Allocation_policy::_At_least>
_NODISCARD static _CONSTEXPR20 pointer _Allocate_for_capacity(_Alty& _Al, size_type& _Capacity) {
    _STL_INTERNAL_CHECK(_Capacity > _Small_string_capacity);
    ++_Capacity; // Take null terminator into consideration

    pointer _Fancy_ptr = nullptr;
    if constexpr (_Policy == _Allocation_policy::_At_least) {
        _Fancy_ptr = _Allocate_at_least_helper(_Al, _Capacity);
    } else {
        _STL_INTERNAL_STATIC_ASSERT(_Policy == _Allocation_policy::_Exactly);
        _Fancy_ptr = _Al.allocate(_Capacity);
    }

#if _HAS_CXX20
    // Start element lifetimes to avoid UB. This is a more general mechanism than _String_val::_Activate_SSO_buffer,
    // but likely more impactful to throughput.
    if (_STD is_constant_evaluated()) {
        _Elem* const _Ptr = _Unfancy(_Fancy_ptr);
        for (size_type _Idx = 0; _Idx < _Capacity; ++_Idx) {
            _STD construct_at(_Ptr + _Idx);
        }
    }
#endif // _HAS_CXX20
    --_Capacity;
    return _Fancy_ptr;
}
```

而 `_Construct_in_place` 的內容其實就是一個 placement new，利用剛剛分配的記憶體空間構建物件：

```cpp
// in file "xutility"
template <class _Ty, class... _Types>
_CONSTEXPR20 void _Construct_in_place(_Ty& _Obj, _Types&&... _Args) noexcept(
    is_nothrow_constructible_v<_Ty, _Types...>) {
#if _HAS_CXX20
    if (_STD is_constant_evaluated()) {
        _STD construct_at(_STD addressof(_Obj), _STD forward<_Types>(_Args)...);
    } else
#endif // _HAS_CXX20
    {
        ::new (static_cast<void*>(_STD addressof(_Obj))) _Ty(_STD forward<_Types>(_Args)...);
    }
}
```

對於空字串的，建構子內會呼叫 `_Construct_empty`：

```cpp
_CONSTEXPR20
basic_string() noexcept(is_nothrow_default_constructible_v<_Alty>) : _Mypair(_Zero_then_variadic_args_t{}) {
    _Construct_empty();
}

// ...

_CONSTEXPR20 void _Construct_empty() {
    auto& _My_data = _Mypair._Myval2;
    _My_data._Alloc_proxy(_GET_PROXY_ALLOCATOR(_Alty, _Getal()));

    // initialize basic_string data members
    _My_data._Mysize = 0;
    _My_data._Myres  = _Small_string_capacity;
    _My_data._Activate_SSO_buffer();

    // the _Traits::assign is last so the codegen doesn't think the char write can alias this
    _Traits::assign(_My_data._Bx._Buf[0], _Elem());
}
```

基本上就做了一些簡單的初始化，讀完前面的應該不難懂

### gcc & clang

讀書會後原本還想多補 [gcc](https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/include/bits/basic_string.h) & [clang](https://github.com/llvm-mirror/libcxx/blob/master/include/string) 的版本，但太忙了，之後再補XD

## Reference

- [Small String Optimization in C++](https://www.youtube.com/watch?v=S7oVXMzTo4w)
- [Inside STL: The string STL](https://devblogs.microsoft.com/oldnewthing/20230803-00/?p=108532)
- [An informal comparison of the three major implementations of std::string](https://devblogs.microsoft.com/oldnewthing/20240510-00/?p=109742)
- [CppCon 2016: Nicholas Ormrod “The strange details of std::string at Facebook"](https://www.youtube.com/watch?v=kPR8h4-qZdk)