---
title: 礦坑系列 ── std::function
date: 2021-06-12
tag: C++ Miner-main
category: C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 <img src = "https://i.imgur.com/thmVmX6.png?" height = 50><br>std::function</center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>
## 前言 

`std::function` 是 C\+\+11 時加入的東西，它基本上是一個類型模板(Class Template)，目的是對可呼叫物件進行包裝，用起來會像是函式指標那樣，但用途更為廣泛，只要是可使用複製建構的可呼叫物件都可以使用，像是函式、lambda、`std::bind`、Functor(function object) 等等

包裝起來的可呼叫物件我們稱它為 `target`，如果一個 `std::function` 還沒包裝任何物件，也就是沒有 target，我們稱它為空(empty)，此時我們如果使用了 target，它會丟出一個 <a href = "https://en.cppreference.com/w/cpp/utility/functional/bad_function_call" class = "pinklink">std::bad_function_call</a> 的例外處理

`std::function` 在 C\+\+17 時有做了一些修正，遺棄了一些東西，這篇文章會以新版的規則來記錄

### 為什麼要有 std::function? 

也許有人想問為什麼需要有 `std::function` ? 直接用 function pointer 之類的不行嗎? 其實 `std::function` 的設計初衷就是保證泛用性，看一下這個例子：

```cpp
#include <iostream>

void call_func( void ( *func_ptr )( int ), int a ) {
  func_ptr( a );
}

void normal_func( int a ) {
  int b{ 10 };
  std::cout << "a = " << a << "\nb = " << b;
}

int main() {
  int a{ 5 };

  call_func( normal_func, a );
  return 0;
}

輸出：
a = 5
b = 10
```

上面這個例子中我們有一個 `normal_func`，作用是將傳入的數字和 b = 10 印出來。然後有一個 `call_func`，作用是吃一個 function pointer，並且把傳入的數字傳進這個 function pointer

上面這個例子會正常的輸出，但如果我們今天加入了 lambda：
```cpp
#include <iostream>

void call_func( void ( *func_ptr )( int a ), int b ) {
  func_ptr( b );
}

auto ret_lambda( int a ) {
  int b{ 10 };

  return [a]( int b ) {
    std::cout << "a = " << a << "\nb = " << b;
  };
}

void normal_func( int a ) {
  int b{ 10 };
  std::cout << "a = " << a << "\nb = " << b;
}

int main() {
  int a{ 5 }, b{ 10 };

  auto lambda_func = [a]( int b ) {
    std::cout << "a = " << a << "\nb = " << b;
  };

  auto lambda2 = ret_lambda( a );

  call_func( normal_func, a );
  call_func( lambda_func, b );
  call_func( lambda2, b );
  return 0;
}
```

下面這兩個 `lambda_func` 和 `lambda2` 就無法傳入 call_func，因為 capture list 有東西的 lambda 是無法轉換成 function pointer 的

此時我們就需要 `std::function` 了，前面有提到，這個東西的產生就是為了保證泛用性，讓 lambda 或其他 callable 物件都可以被傳入：
```cpp
#include <functional>
#include <iostream>

void call_func( std::function<void( int )> func_ptr, int b ) {
  func_ptr( b );
}

std::function<void( int )> ret_lambda( int a ) {
  int b{ 10 };

  return [a]( int b ) {
    std::cout << "a = " << a << "\nb = " << b << '\n';
  };
}

void normal_func( int a ) {
  int b{ 10 };
  std::cout << "a = " << a << "\nb = " << b << '\n';
}

int main() {
  int a{ 5 }, b{ 10 };

  std::function<void( int )> lambda_func = [a]( int b ) {
    std::cout << "a = " << a << "\nb = " << b << '\n';
  };

  std::function<void( int )> lambda2 = ret_lambda( a );

  call_func( normal_func, a );
  call_func( lambda_func, b );
  call_func( lambda2, b );
  return 0;
}
```

這樣一來就可以成功的運作了，可呼叫的 Class 也可以用：
```cpp
#include <functional>
#include <iostream>

void call_func( std::function<void( int )> func_ptr, int b ) {
  func_ptr( b );
}

std::function<void( int )> ret_lambda( int a ) {
  int b{ 10 };

  return [a]( int b ) {
      std::cout << "a = " << a << "\nb = " << b << '\n';
  };
}

class Collable_class {
public:
  void operator()( int a ) const {
      std::cout << "a = " << a << "\nb = " << b << '\n';
  }

private:
  int b{ 10 };
};

int main() {
  int a{ 5 }, b{ 10 };

  Collable_class C_instance;

  call_func( C_instance, a );
  return 0;
}
```

記得要引入標頭檔 `<functional>`

## 語法與相關函式 

這次試著改了文章的順序，先教大家怎麼用，再去講內部實現的概念，主要是因為有些人比較在意該怎麼用與何時使用，並不是太在意裡面怎麼實作的。再來雖然核心概念差不多，但不同編譯器實作的方法還是會稍微不太一樣

### 語法與重要定義 

語法其實挺簡單的，基本上就是

> std::function<回傳型態(參數列)> 實例名 = 初始化器

初始化器就是拿來初始化的東西，前面也提到了只要是可呼叫的物件都可以拿來用，看看 <a href = "https://en.cppreference.com/w/cpp/utility/functional/function" class = "pinklink">cppreference</a> 上的例子：

```cpp
#include <functional>
#include <iostream>

struct Foo {
  Foo( int num ) : num_( num ) {}
  void print_add( int i ) const { std::cout << num_ + i << '\n'; }
  int num_;
};

//一般函式
void print_num( int i ) {
  std::cout << i << '\n';
}

//可呼叫的Struct，有重載operator()
struct PrintNum {
  void operator()( int i ) const {
    std::cout << i << '\n';
  }
};

int main() {
  // std::function 儲存一個回傳 void 的 function，參數吃一個int
  std::function<void( int )> f_display = print_num;
  f_display( -9 );

  // std::function 儲存一個回傳 void 的 lambda，參數沒吃東西
  std::function<void()> f_display_42 = []() { print_num( 42 ); };
  f_display_42();

  // std::function 儲存 std::bind 回傳的 function，function 回傳 void，參數列不吃東西
  std::function<void()> f_display_31337 = std::bind( print_num, 31337 );
  f_display_31337();

  // std::function 儲存一個 member function，這個 member function 回傳 void，參數吃一個 Foo和一個 int
  std::function<void( const Foo &, int )> f_add_display = &Foo::print_add;
  const Foo foo( 314159 );
  f_add_display( foo, 1 );
  f_add_display( 314159, 1 );

  // std::function 儲存一個 accessor
  std::function<int( const Foo & )> f_num = &Foo::num_;
  std::cout << "num_: " << f_num( foo ) << '\n';

  // std::function 儲存一個 std::bind 回傳的 function，std::bind 吃一個 member function 和實例
  std::function<void( int )> f_add_display2 = std::bind( &Foo::print_add, foo,std::placeholders::_1 );
  f_add_display2( 2 );

  // std::function 儲存一個 std::bind 回傳的 function，std::bind 吃一個 member function 和實例的位址
  std::function<void( int )> f_add_display3 = std::bind( &Foo::print_add, &foo,std::placeholders::_1 );
  f_add_display3( 3 );

  // std::function 儲存一個可呼叫的物件
  std::function<void( int )> f_display_obj = PrintNum();
  f_display_obj( 18 );

  auto factorial = []( int n ) {
    // store a lambda object to emulate "recursive lambda"; aware of extra overhead
    std::function<int( int )> fac = [&]( int n ) { return ( n < 2 ) ? 1 : n * fac( n - 1 ); };
    // note that "auto fac = [&](int n){...};" does not work in recursive calls
    return fac( n );
  };
  for ( int i{ 5 }; i != 8; ++i ) {
    std::cout << i << "! = " << factorial( i ) << ";  ";
  }
}
```

從上例可見可儲存的東西挺多的，只要是可呼叫物件基本上都可以儲存。不過不知道大家有沒有覺得有其中一個很奇怪，那就是 `std::function<int( const Foo & )> f_num = &Foo::num_;`，右邊的這個 `&Foo::num_` 是個資料成員指標，那為什麼這個可以拿來初始化 `std::function` 呢?

讓我們回顧一下最一開始的<a href = "https://timsong-cpp.github.io/cppwp/n4868/func.wrap.func#general-1" class = "pinklink">定義</a>：

> std::funtion 這個類別模板提供了多型的 wrapper，通常拿來包函式指標。 這個 Wrapper 可以儲存，複製，和透過 `()` 呼叫任何的可呼叫物件，並允許 function 為一級函式

接下來就有個問題了，到底什麼是可呼叫物件呢? 定義如下：

> 一個可呼叫物件指的是一個物件擁有可呼叫的型態 ( <a href = "" class = "pinklink">20.14.3-4</a> )

所以接下來的問題就變成「什麼是可呼叫的型態」了，定義如下：

> 可呼叫型態指的是 function object type 或是一個資料成員的指標 ( <a href = "https://timsong-cpp.github.io/cppwp/n4868/func.def#3" class = "pinklink">20.14.3-3</a> )

這樣答案就很明顯了，`&Foo::num_` 是個資料成員指標，代表他是可呼叫的物件，因此可以拿來初始化 `std::function`

至於 function object type 指的則是一種在 function call 內可以是後序表達式(<a href = "https://timsong-cpp.github.io/cppwp/n4868/expr.post.general#nt:postfix-expression" class = "pinklink">postfix-expression</a>) 的物件型態，至於什麼是後序表達式這邊就先不介紹了，再寫下去會有點偏題

那麼既然知道了什麼是 function object type，那麼 function object 很直覺的就是那些型態是 function object type 的物件了

簡單來說，可呼叫物件(collable object) 的意思就是某個有明確定義動作的物件，對 function object 來說就是透過 `()` 來呼叫的相對應的物件，對函式指標來說就是呼叫相對應的函式，對成員指標來說就是拜訪相對應的成員

複雜的定義大概就這樣，那麼 `std::function` 的使用時機在前言的地方也提到了，`std::function` 主要就是為了泛用性，消除型態上面的差異而出現的東西。 代價也不大，`std::function` 並不肥，能使用的話就盡量使用吧

### 相關函式 

在 C\+\+17、C\+\+20 時有部分的相關函式被刪除了，那些被刪除的函式本篇就不介紹了，主要介紹那些仍可使用的，有興趣的可以到 cppreference 上看

這邊的內容除了怎麼使用以外可能也會扯到內部的結構，

接下來的 `fn` 代表某個 `std::function` 的 instance，簡單來說就是 `std::function<void()> fn = 某個 callable object` 這樣，只是一個代稱方便大家理解
+ 建構子

    基本上內部寫好的建構子有五個，加上最新的 <a href = "https://cplusplus.github.io/LWG/issue2774" class = "pinklink">Defect Report(C\+\+23)</a> 內提出的方法有六個 

    1、2：
    ```cpp
    function() noexcept;    // 1
    function( std::nullptr_t ) noexcept;    // 2
    ```
    上面這兩個(1、2) 會建構出空的 `std::function`

    3、4：
    ```cpp
    function( const function& other );    // 3
    function( function&& other ) noexcept;    // 4
    ```
    上面這兩個(3、4) 會將 other 的 target 複製(3) 或移動(4) 到 `*this`。 簡單來說就是把對象包裝起來的東西移動或複製到自己身上。如果 other 是空的，`*this` 也會是空的。 

    另外，移動(4) 後的 other 會處於有效但未指定的狀態

    5：
    ```cpp
    template< class F >
    function( F f );    // 5
    ```

    前面的 4 個都是用 `std::function` 來初始化，這個可以說是最常用的了，這個建構子也是 `std::function` 可以做到高泛用性的原因

    基本上就是用我們給的東西來初始化內部的 target，如果想看內部的實作可以直接往下拉到內部概念那邊看，因為這扯到整個結構的問題

    不過要注意的是如果 `std::function` 的回傳型態是個繫結到某個沒有 trailing-return-type 的 lambda 所回傳的 reference type，由於 auto deduction 的關係，這種 lambda 回傳的型態一定會是個 prvalue，因此 `std::function` 的回傳型態的 reference 會繫結到一個暫時物件，但這個暫時物件的生命週期會在 `std::function::operator()` 回傳時結束，因此會變一個懸掛的 reference(dangling reference)

    例子：
    ```cpp
    std::function<const int&()> F([]{ return 42; });
    int x = F(); // Undefined behavior: the result of F() is a dangling reference
    ```

    6：
    ```cpp
    template< class F >
    function( F&& f );    // 6
    ```

    最後這個是最近更新的，還不在標準裡面，但有在 <a href = "https://eel.is/c++draft/func.wrap.func#lib:function,constructor____" class = "pinklink">C\+\+23 的草案</a>裡面，如果想看舊版的規範可以到<a href = "https://timsong-cpp.github.io/cppwp/n4868/func.wrap.func.con#lib:function,constructor____" class = "pinklink">這裡</a>看，另外裡面還有對編譯器實作的建議、Postconditions 和 Preconditions 等等，有興趣的也可以進來看

    具體上的原因可以到 <a href = "https://cplusplus.github.io/LWG/issue2774" class = "pinklink">Defect Report(C\+\+23)</a> 看，簡單來說就是 `operator=` 吃的是 `F&& f`，而如果 constructor 用複製的會有一些 Value Categories 上的影響

    在新版的實作內，我們會利用 `std::forward<F>(f)` 來初始化 target，target 的型態會是 `std::decay<F>::type`

    如果 `f` 是個 null 的 function pointer、member pointer 或一個由 `std::function` 特化的空值，那麼 `*this` 會是一個空的 `std::function`

    這個 constructor 不會參加多載解析(<a href = "https://en.cppreference.com/w/cpp/language/overload_resolution" class = "pinklink">Overload resolution</a>)，除非 target 的 type 不是 `std::function`，而且對於那些參數的型態 Args... 來說，它的 lvalue 是可呼叫的而且回傳的型態 R 也是可呼叫的型態

    如果 target 的 type 不能複製，或者初始化的格式不對，那麼這是個 ill-formed

+ 解構子

    destructor 就相對沒那麼複雜了，但要注意一下解構的時候的時候會連 target 一起解構

+ operator=

    基本上內部寫好的有五個

    1：
    ```cpp
    function& operator= ( const function& other );
    ```

    上面這個會 copy other 的 target 到 `*this` 的 target

    2：
    ```cpp
    function& operator=( function&& other );
    ```

    上面這個會移動 other 的 target 到 `*this` 的 target，此時 other 會處於未定義的有效狀態

    3:
    ```cpp
    function& operator=( std::nullptr_t ) noexcept;
    ```

    上面這個會把目前的 target 解構，讓 `*this` 回復為空

    4：
    ```cpp
    template< class F >
    function& operator=( F&& f );
    ```

    上面這個將會把 `*this` 的 target 設為可呼叫的 f

    除非 f 對於參數的類型 Args... 和回傳的型態 R 是可呼叫的，不然這個運算符不會執行 Overload Resolution

    5：
    ```cpp
    template< class F >
    function& operator=( std::reference_wrapper<F> f ) noexcept;
    ```

    上面這個會把 f 的 target 複製給 `*this` 的 target

+ operator bool

    `std::function` 的 operator bool 會檢查 `*this` 的 target 是否為空，如果非空就回傳 true，如果是空的就回傳 false

    例子：
    ```cpp
    #include <functional>
    #include <iostream>

    void sampleFunction() {
    }

    void checkFunc( std::function<void()> &func ) {
      // 利用 operator bool 來確認 std::function 是否為空
      if ( func ) {
          std::cout << "Function is not empty!\n";
      }
      else {
          std::cout << "Function is empty. Nothing to do.\n";
      }
    }

    int main() {
      std::function<void()> f1;
      std::function<void()> f2( sampleFunction );

      std::cout << "f1: ";
      checkFunc( f1 );    // std::function 為空，進入 else

      std::cout << "f2: ";
      checkFunc( f2 );    // std::function 非空，進入 if
    }
    ```

+ operator()

    > R operator()( Args... args ) const;

    `std::function` 的 operator() 會傳入參數，呼叫儲存的 callable object

    例子：
    ```cpp
    #include <functional>
    #include <iostream>

    void call( std::function<int()> f )    // std::function 可以 passed by value
    {
      std::cout << f() << '\n';
    }

    int normal_function() {
      return 42;
    }

    int main() {
      int n = 1;
      std::function<int()> f = [&n]() { return n; };
      call( f );

      n = 2;
      call( f );

      f = normal_function;
      call( f );
    }
    ```

+ swap
    語法： `fn.swap(目標)`
    作用： 交換所存的 callable object
    例子：

    ```cpp
    #include <functional>
    #include <iostream>

    void fn1() {
      std::cout << "fn 1\n";
    }

    void fn2() {
      std::cout << "fn 2\n";
    }

    int main() {
      std::function<void()> s1{ fn1 };
      std::function<void()> s2{ fn2 };

      s1(), s2(); // fn1 \n fn2

      puts( "" ), s1.swap( s2 );

      s1(), s2(); // fn2 \n fn1
      return 0;
    }
    ```

+ target_type

    > const std::type_info& target_type() const noexcept;

    回傳儲存的 target 的 `typeid`

    例子：
    ```cpp
    #include <functional>
    #include <iostream>

    int f( int a ) { return -a; }

    void g( double ) {}

    int main() {
      // fn1 and fn2 have the same type, but their targets do not
      std::function<int( int )> fn1( f ),
                                fn2( []( int a ) { return -a; } );

      std::cout << fn1.target_type().name() << '\n'    // PFiiE
                << fn2.target_type().name() << '\n';    // Z4mainEUliE_

      // since C++17 deduction guides (CTAD) can avail
      std::cout << std::function{ g }.target_type().name() << '\n';    // PFvdE
    }
    ```

+ target

    ```cpp
    template< class T >
    T* target() noexcept;    // (1)
    //----------------------------------//
    template< class T >
    const T* target() const noexcept    // (2)
    ```

    如果 `target_type() == typeid(T)`，回傳一個指向儲存的 target 的 pointer，否則回傳一個 null pointer

    例子：
    ```cpp
    #include <functional>
    #include <iostream>

    int f( int, int ) {
      puts( "calling f" );
      return 1;
    }
    int g( int, int ) {
      puts( "calling g" );
      return 2;
    }

    void sampleFunction() {
        puts( "calling ssample Function" );
    }

    void test( const std::function<int( int, int )> &arg ) {
      std::cout << "test function: ";
      if ( arg.target<std::plus<int>>() )
          std::cout << "it is plus\n";
      if ( arg.target<std::minus<int>>() )
          std::cout << "it is minus\n";

      int ( *const *ptr )( int, int ) = arg.target<int ( * )( int, int )>();

      if ( ptr && *ptr == f ) {
          std::cout << "it is the function f\n";
          ( *ptr )( 1, 1 );
      }
      if ( ptr && *ptr == g ) {
          std::cout << "it is the function g\n";
          ( *ptr )( 1, 1 );
      }
    }

    int main() {
      test( std::function<int( int, int )>( std::plus<int>() ) );
      test( std::function<int( int, int )>( std::minus<int>() ) );
      test( std::function<int( int, int )>( f ) );
      test( std::function<int( int, int )>( g ) );

        std::function<void()> fn = sampleFunction;

        void ( **fn_ptr )() = fn.target<void ( * )()>();
        ( *fn_ptr )();
    }
    ```

### Deduction Guides 模板推導指引 

在 C\+\+17 時有個叫 CTAD 的東西出現了，不知道的朋友可以先到 <a href = "https://en.cppreference.com/w/cpp/language/class_template_argument_deduction" class = "pinklink">cppreference</a> 上看，或看一下 <a href = "https://tjsw.medium.com/%E6%BD%AE-c-17-class-template-argument-deduction-%E5%92%8C-deduction-guide-%E9%A1%9E%E5%88%A5%E6%A8%A3%E7%89%88%E5%8F%83%E6%95%B8%E6%8E%A8%E5%B0%8E-70cc36307a42" class = "pinklink">TSJW的文章</a>

簡單來說就是讓模板自己推定型態的技術，舉個例子：`std::vector<int> vec = {5};` 可以寫成 `std::vector vec = {5};` 這樣，int 可以由編譯器推導出來。雖然看起來好像很簡單，但後面牽扯到的東西挺多的，像是自訂的型態等等，這邊不討論太多，因為不是今天的主題

那麼 `std::function` 自然也會有這個技術了，畢竟也是一個通過模板來實現的東西

內部有兩個定義：
```cpp
template<class R, class... ArgTypes>
function(R(*)(ArgTypes...)) -> function<R(ArgTypes...)>;    // (1)
```

上面這個讓 `std::function` 可以自動推斷出一般函式的型態

```cpp
template<class F>
function(F) -> function</*see below*/>;    // (2)
```

而第二個這個，只有當 `&F::operator()` 不是拿來當計算的操作數，語法合法，且 `decltype(&F::operator())` 的形式是 `R(G::*)(A...)` (可有 cv限定詞、noexcept、lvalue reference)，用於某些 class type G 時，會進入 Overload Resolution

看起來很複雜，但簡單來說就是 `decltype(&F::operator())` 是有效的，此時的型態會被推導為 `std::function<R(A...)>`

例子：
```cpp
#include <functional>
int func( double ) { return 0; }

int main() {
    std::function f{ func };    // 利用第一個規則推導出 std::function<int(double)>
    int i = 5;
    std::function g = [&]( double ) { return i; };    // 利用第二個規則推導出 std::function<int(double)>
}
```

不過未來這個 Deduction Guides 可能還會改變，尤其是在 `std::function` 支援 `noexcept` 之後
## 內部概念

內部概念我大概會講一下架構，然後帶大家看一下 GCC 上的優化與 code

接下來的內容主要參考了 Raymond 的兩篇文章、 Stackoverflow 上的講解與 jerrt_fuyi 的文章：<a href = "https://devblogs.microsoft.com/oldnewthing/20200513-00/?p=103745" class = "pinklink">連結 1</a>、<a href = "https://devblogs.microsoft.com/oldnewthing/20200514-00/?p=103749" class = "pinklink">連結 2</a>、<a href = "https://stackoverflow.com/questions/18453145/how-is-stdfunction-implemented" class = "pinklink">連結 3</a>、<a href = "https://www.cnblogs.com/jerry-fuyi/p/std_function_interface_implementation.html" class = "pinklink">連結 4</a>，有興趣的可以進去看看原文

### 架構

我們的目的是消除型態，雖然有很多不同的方法能達到這個目的，但概念上的架構上主要會有一個可呼叫物件的基類，之後利用模板，由這個基類衍生出不同的可呼叫物件的實例，之後 `std::function` 再去存取這個實例的指標，大概會像這樣：

```cpp
#include <memory>

struct callable_base {
    callable_base() = default;
    virtual ~callable_base() {}
    virtual bool invoke( int, char * ) = 0;
    virtual std::unique_ptr<callable_base> clone() = 0;
};

template <typename T>
struct callable : callable_base {
    T m_t;

    callable( T const &t ) : m_t( t ) {}
    callable( T &&t ) : m_t( move( t ) ) {}

    bool invoke( int a, char *b ) override {
        return m_t( a, b );
    }

    std::unique_ptr<callable_base> clone() override {
        return make_unique<callable>( m_t );
    }
};

struct function {
    std::unique_ptr<callable_base> m_callable;

    template <typename T>
    function( T &&t ) : m_callable( new callable<decay_t<T>>( forward<T>( t ) ) ) {
    }

    function( const function &other ) : m_callable( other.m_callable ? other.m_callable->clone() : nullptr ) {
    }

    function( function &&other ) = default;

    bool operator()( int a, char *b ) {
        return m_callable->invoke( a, b );
    }
};
```

這只是一個簡單的偽代碼，可能無法編譯，`callable_base` 提供了解構、呼叫、複製等動作的介面，再由各種可呼叫物件自行去實作

在上面使用了 `unique_ptr`，這會讓複製建構子有些難做，因為 `unique_ptr` 無法複製，因此在實作上通常會使用原生的指標，並自行管理資源的分配，比較複雜，但也比較方便我們優化

可以看看另一個人幫忙做的簡化版本，這是 <a href = "https://stackoverflow.com/questions/18453145/how-is-stdfunction-implemented" class = "pinklink">StackOverflow</a> 上有人做的，他簡化的是 Ubuntu 14.04 gcc 4.8 上的 `std::function`，這邊他選擇將 `callable_base` 那些繼承的東西給簡化掉了，比較注重在內部消除型態的實現上，我幫忙整理了一下並加上了一些註解：

```cpp
#include <iostream>
#include <memory>

template <typename T>
class function;

template <typename R, typename... Args>
class function<R( Args... )> {
    // 這些 function pointer type 是拿來消除型態的，下面這些 void* 都是從某種 function type 轉過來的
    using invoke_fn_t = R ( * )( void *, Args &&... );    //拿來呼叫的函式指標
    using construct_fn_t = void ( * )( void *, void * );    //建構子的函式指標
    using destroy_fn_t = void ( * )( void * );    //解構子的函式指標

    // 因為有些 callable 物件無法直接被轉換成 function pointer，像是有捕捉東西的 lambda，所以我們需要一個泛型的函式幫助我們將其轉換成指標並呼叫
    template <typename Functor>
    static R invoke_fn( Functor *fn, Args &&...args ) {
        return ( *fn )( std::forward<Args>( args )... );
    }

    template <typename Functor>
    static void construct_fn( Functor *construct_dst, Functor *construct_src ) {
        // Functor type 需要是用複製建構的，這個是 Placement new，沒看過的可以去看一下用法
        new ( construct_dst ) Functor( *construct_src );
    }

    template <typename Functor>
    static void destroy_fn( Functor *f ) {
        f->~Functor();
    }

    // 這些 pointer 拿來儲存行為
    invoke_fn_t invoke_f;    // 儲存呼叫函式的指標
    construct_fn_t construct_f;    // 儲存建構子的函式指標
    destroy_fn_t destroy_f;    // 儲存解構子的函式指標

    // 這邊要把 Functor 的型態消除，並將它存到 void*。 因此我們也要能夠拿到儲存的空間
    std::unique_ptr<std::byte[]> data_ptr;    // 指向一段 memory pool 的 unique_ptr，用來放 function pointer
    std::size_t data_size;    // 能夠儲存的空間

  public:
    // 空的 function
    function()
        : invoke_f( nullptr ), construct_f( nullptr ), destroy_f( nullptr ), data_ptr( nullptr ), data_size( 0 ) {}

    // 從任何的 Functor 來建構的 function
    template <typename Functor>
    function( Functor f )
        // 特化 function 並利用轉型消除它的型態
        : invoke_f( reinterpret_cast<invoke_fn_t>( invoke_fn<Functor> ) ),    // 初始化呼叫指標
          construct_f( reinterpret_cast<construct_fn_t>( construct_fn<Functor> ) ),    // 初始化建構指標
          destroy_f( reinterpret_cast<destroy_fn_t>( destroy_fn<Functor> ) ),    // 初始化解構指標
          data_ptr( new std::byte[sizeof( Functor )] ),    // 初始化資料陣列的指標
          data_size( sizeof( Functor ) ) {    // 初始化儲存空間的大小
        // 複製 functor 到內部拿來儲存行為的指標
        this->construct_f( this->data_ptr.get(), reinterpret_cast<void *>( &f ) );
    }

    // 複製建構子
    function( function const &rhs )
        : invoke_f( rhs.invoke_f ), construct_f( rhs.construct_f ), destroy_f( rhs.destroy_f ), data_size( rhs.data_size ) {
        if ( this->invoke_f ) {
            //如果 target 非空，那就複製 target
            this->data_ptr.reset( new std::byte[this->data_size] );
            this->construct_f( this->data_ptr.get(), rhs.data_ptr.get() );
        }
    }

    ~function() {
        if ( data_ptr != nullptr ) {
            this->destroy_f( this->data_ptr.get() );
        }
    }

    // 拿來呼叫的運算子
    R operator()( Args &&...args ) {
        return this->invoke_f( this->data_ptr.get(), std::forward<Args>( args )... );
    }
};

// examples
int main() {
    int i = 0;
    auto fn = [i]( const std::string &s ) mutable {
        std::cout << ++i << ". " << s << std::endl;
    };
    fn( "first" );    // 1. first
    fn( "second" );    // 2. second

    // 利用 lambda 建構
    ::function<void( const std::string & )> f( fn );
    f( "third" );    // 3. third

    // 利用別的 function 來建構
    ::function<void( const std::string & )> g( f );
    f( "forth - f" );    // 4. forth - f
    g( "forth - g" );    // 4. forth - g

    // 利用有捕捉一個 std::string 的 lambda 建構
    std::string x( "xxxx" );
    ::function<void()> h( [x]() { std::cout << x << std::endl; } );
    h();

    ::function<void()> k( h );
    k();

    ::function<int()> ifn = []() { return 5; };
    int xx = ifn();
    std::cout << xx;    // 5
    return 0;
}
```

雖然因為簡化的關係和 gcc 裡面寫的已經有點不太一樣了，但主要的核心概念仍一樣，就是透過模板來消除型態

### 內部實作與優化

#### 前言

接下來的內容有很多地方都來自 <a href = "https://www.cnblogs.com/jerry-fuyi/p/std_function_interface_implementation.html" class = "pinklink">jerry_fuyi</a> 的文章，基本上是引用過來，改寫一些部分，自己讀了很久，希望多加一些解釋，抓一些重點出來後，大家能更容易理解，如果看不習慣我的也很建議回去讀讀原文，因為真的寫得蠻好蠻詳細的

而如果你已經看得很累了，那麼可以去休息一下，因為接下來的內容會比較生硬 XD

看完了前面約略的概念後各位可能覺得這個東西就這樣了，其實沒那麼簡單，還有很多隱藏的問題(雖然前面可能先講了答案XD，但大家可能沒想過會有這些問題)，例如，函式不是一個固定大小的對象，但 class 是，我們用 class 來包裝不同的函式，卻要有相同的大小，不會有問題嗎? 還有其他很多的小問題，導致這個東西一直到 C\+\+23 都還拿出來被討論，前面的建構子就是其中一項，有些東西可能會牽扯到 ABI Break，蠻複雜的

#### 實作前的想法

`std::function` 能儲存不同的可呼叫物件，表示有多態性，這我們前面使用了 virtual function 與繼承解決了，這是個簡單、直覺且有效的方法，但這個方法會用到動態記憶體，很多情況下這是種浪費，像是一個沒捕捉東西的 lambda，這種情況下他只有一個 char 的大小，但我們卻為了它去呼叫了分配動態記憶體的函式

於是我們就需要想辦法避免這種浪費，剛剛說了 `std::function` 的大小是固定的，但是這個大小是可以讓我們自己定的，所以我們可以在 `std::function` 裡面放一個空白且大小適中的 field 來存這種小對象，來避免呼叫到動態記憶體

而對於更大的可呼叫物件，雖然放不進這個 field，但可以放這個可呼叫物件的指標，這種 TDM 的結構可以用 union 來做

這種小物件直接儲存，大物件才放在 heap 上並儲存指標的方法叫做 <span class = "yellow">small object optimization</span>

在利用了繼承的實作中，可呼叫物件被包裝在一個子類裡面，`std::function` 會持有一個其父類的指標，但為了效率，我們需要把剛剛那個空白的 field 與這個指針用 union 包裝起來

既然用了 union，在不確定這個 union 存的是什麼之前，當然不能去呼叫裡面的東西，因此這樣的設計我們就不能使用繼承來去消除型態了

於是我們需要想其他的方法來達成多態性，而這邊大家可以想一下 virtual function 是怎麼實現的，有 virtual function 的 class 裡面會有個 vptr，指向一個 vtable，這個 vtable 有很多個 slot，裡面有指向 type_info 對象的指標和函式指標

發現了嗎? 就是函式指標，函式指標也是一個能幫助我們實現多態性的東西，如果你是想在 C 裡面實現多態，比較方便的方法也是在 struct 內直接放 function pointer，因此我們主要會使用 function pointer 來實作

#### 開始實作

基本上 `std::function` 是個模板類，模板參數就是一個 type，注意是一個 type，不是好幾種 type，以這個例子來說：

```cpp
std::function<int(double)> f;
```

`f` 是一個可以呼叫的對象，參數吃一個 `double`，回傳一個 `int`。 你可能會覺得奇怪，這邊這樣不就有兩個 type 了嗎? 怎麼還說是一個 type，其實 `int(double)` 自己就是一個 type，它是一種 function type，但要注意不是 function pointer

接下來我會帶大家看一下 gcc 的實作 (待補，被高微殘害中QQ)

## 參考資料 

**<a href = "https://en.cppreference.com/w/cpp/utility/functional/function" class = "redlink">1. std::function</a> (cppreference) (文章部分來源)**

**<a href = "https://timsong-cpp.github.io/cppwp/n4868/expr.post.general#nt:postfix-expression" class = "redlink">2. Expressions</a>**

**<a href = "https://timsong-cpp.github.io/cppwp/n4868/function.objects#def:function_object,type" class = "redlink">3. General utilities library</a>**

**<a href = "https://timsong-cpp.github.io/cppwp/n4868/func.def" class = "redlink">4. Function Definitions</a>**

**<a href = "https://stackoverflow.com/questions/18453145/how-is-stdfunction-implemented" class = "redlink">5. How is std::function implemented?
</a>**

**<a href = "https://devblogs.microsoft.com/oldnewthing/20200513-00/?p=103745" class = "redlink">6. Inside std::function, part 1: The basic idea</a>**

**<a href = "https://stackoverflow.com/questions/1174169/function-passed-as-template-argument" class = "redlink">7. Function passed as template argument</a>**

**<a href = "https://dev.to/lesleylai/what-is-std-function-in-c-and-why-we-need-them-48d0" class = "redlink">8. What is std::function in C++, and why we need them?</a>**

**<a href = "https://stackoverflow.com/questions/25848690/should-i-use-stdfunction-or-a-function-pointer-in-c" class = "redlink">9. Should I use std::function or a function pointer in C++?</a>**

**<a href = "https://www.796t.com/article.php?id=14448" class = "redlink">10. C++ 11 std::function和std::bind使用詳解</a>**

**<a href = "https://stackoverflow.com/questions/35303332/what-are-preconditions-and-postconditions" class = "redlink">11. What are preconditions and postconditions?</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/overload_resolution" class = "redlink">12. Overload resolution</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/class_template_argument_deduction" class = "redlink">13. Class template argument deduction (CTAD)</a>**

**<a href = "https://tjsw.medium.com/%E6%BD%AE-c-17-class-template-argument-deduction-%E5%92%8C-deduction-guide-%E9%A1%9E%E5%88%A5%E6%A8%A3%E7%89%88%E5%8F%83%E6%95%B8%E6%8E%A8%E5%B0%8E-70cc36307a42" class = "redlink">14. 潮．C++17 | Class Template Argument Deduction 和 Deduction Guide 類別樣版參數推導</a>**

**<a href = "https://stackoverflow.com/questions/22180312/difference-between-undefined-behavior-and-ill-formed-no-diagnostic-message-requ" class = "redlink">15. Difference between Undefined Behavior and Ill-formed, no diagnostic message required</a>**

**<a href = "https://iter01.com/553394.html" class = "redlink">16. C++ typeid關鍵字詳解</a>**

**<a href = "https://stackoverflow.com/questions/17183761/c11-type-deduction-with-stdfunction" class = "redlink">17. C++11 Type Deduction With std::function</a>**

**<a href = "https://stackoverflow.com/questions/12405102/template-argument-type-deduction-from-stdfunction-return-type-with-lambda" class = "redlink">18. template argument type deduction from std::function return type with lambda</a>**

**<a href = "https://stackoverflow.com/questions/5712826/argument-type-auto-deduction-and-anonymous-lambda-functions" class = "redlink">19. Argument type auto deduction and anonymous lambda functions</a>**

**<a href = "https://stackoverflow.com/questions/42773202/type-deduction-for-stdfunction/42773516" class = "redlink">20. Type deduction for std::function</a>**

**<a href = "https://www.cnblogs.com/jerry-fuyi/p/std_function_interface_implementation.html" class = "redlink">21. 剖析STD::FUNCTION接口与实现</a>** (文章部分來源)

**<a href = "https://stackoverflow.com/questions/4736044/checking-invariants-in-c" class = "redlink">22. checking invariants in C++</a>**

**<a href = "https://advancedcppwithexamples.blogspot.com/2009/07/example-of-c-class-invariant.html" class = "redlink">23. Example of C++ class invariant</a>**

**<a href = "https://stackoverflow.com/questions/14302834/when-to-make-a-type-non-movable-in-c11" class = "redlink">24. When to make a type non-movable in C++11?</a>**

**<a href = "http://llllkkkk.github.io/2014/04/26/-stdfunction-/" class = "redlink">25. 简析 std::function 实现原理</a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**

**<a href = "" class = "redlink"></a>**
