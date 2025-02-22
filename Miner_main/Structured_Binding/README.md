---
title: 礦坑系列 ── 結構化綁定 Structured Binding Declaration
date: 2021-04-01
tag: C++ Miner
category: C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 <img src = "https://i.imgur.com/thmVmX6.png?" height = 50><br>結構化綁定 Structured Binding Declaration</center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

#

# <span class = "orange">前言</span> 

Structured binding declaration 是 C++17 加入的一個新特性，它讓我們能夠更簡單地去處理多個回傳值或多變數的情況，通常會在要接 tuple_like 的容器或 Struct 回傳值時搭配 auto 來使用

我盡量將原裡理解並寫在了「介紹及原理」的部分，有些部分是翻譯了官方的文件，原先只是想弄個翻譯，結果翻著翻著發現官方有些地方講的不是讓人很明白，於是就又加上了自己的文字，漸漸地就變一篇文件了，如果只想知道如何使用的朋友可以直接跳到下方的應用部分

# <span class = "orange">語法</span> 

主要有三種初始化的方式：
> **1. attr(opt) cv(opt) auto ref-qualifier(opt) \[idendentifier-list] = expression;** [color=#30DCD8]  
> **2. attr(opt) cv(opt) auto ref-qualifier(opt) \[idendentifier-list]{ expression };**	  
> **3. attr(opt) cv(opt) auto ref-qualifier(opt) \[idendentifier-list]( expression );**  

> opt 代表的是 optional，可加可不加的意思

+ **attr** :whale:
    指的是 [attrubutes](https://en.cppreference.com/w/cpp/language/attributes)，可加可不加

+ **cv** :whale:
    可能是[cv-qualifier](https://en.cppreference.com/w/cpp/language/cv)，後方需加上 auto，需要的話也可以加上 static、thread_local 之類的[儲存類說明符](https://en.cppreference.com/w/cpp/language/storage_duration)，但不推薦使用到 volatile

+ **ref-qualifier** :whale:
    `&` 或 `&&`，可加可不加，取決於你的需求

+ **identifier-list** :whale:
    這裡放妳要使用的變數名稱，他實際上不是變數而是標示符，它們之間需要以逗點 `,` 隔開，後方會有例子

+ **expression** :whale:
    表達式，通常會放 array、tuple-like 容器或是個沒有 union 成員的 Class，語法上會是 assignment-expression，它們不能是 `throw` 表達式，並且在 top-level 不能有逗號運算符，這裡應該是指 expression 能夠有 sub-expression，而它要的是最上層的那個 (感謝marty大佬)。 另外，expression 內的變數名不能和 identifier-list 內的變數名相同，簡單來說就是不能重複宣告同樣名字的變數

# <span class = "orange">介紹及原理</span> 

Structed binding 會在你現在的 [scope](https://en.cppreference.com/w/cpp/language/scope) 內採用你 identifier-list 裡給的標示符，並且將其連結到你 expression 裡寫的元素或子物件。採用時它會先創造出一個特殊的變數來存取你的初始化敘述(initializer)，型態取決於你的 expression，這個變數的名稱這裡我們先取作 `__e` ，由於 `__e` 可能是個容器或參考，所以我們給他取叫 initializer，沒看過這個詞的朋友不用太擔心，而 `__e` 在存取時有一些規則：

+ **如果 expression 是個 A型態的 array，而且你沒有使用 ref-qualifier，那麼 `__e` 會是原先 expression 計算結果的複本，型態會是 `cv A`，cv指的就是[cv-qualifier](https://en.cppreference.com/w/cpp/language/cv)。而 `__e` 內的元素會依據你使用的初始化方式(最上方寫的三種方式)來初始化。**

    如果你使用的是第一種(`=`號)方式，那麼 `__e` 內的元素會使用[複製初始化](https://en.cppreference.com/w/cpp/language/copy_initialization)來初始化為你 expression 內相對應的元素；

    若你使用的是第二或第三種方法，那麼 `__e` 內的元素會使用[直接初始化](https://en.cppreference.com/w/cpp/language/direct_initialization)來初始化為你 expression 內相對應的元素

+ **如果不是上面的那種情況，那麼編譯器先將 Structured Binding 改寫，直接使用 `__e` 這個名稱作為原先 expression 的複本，像這樣：**
    > **1. attr(opt) cv(opt) auto ref-qualifier(opt) __e = expression;** [color=#30DCD8]  
    > **2. attr(opt) cv(opt) auto ref-qualifier(opt) __e{ expression };**  
    > **3. attr(opt) cv(opt) auto ref-qualifier(opt) __e( expression );**  

    `__e` 會是個匿名的 tuple-like 的容器(沒ref-qualifier時) 或是 tuple-like 的容器的參考(有ref-qualifier時)，簡短來說妳有寫 `&` 這個 `__e` 就會是個參考，如果沒寫就是個容器。 接著編譯器會去看它是否符合 [Tuple-Like" Binding Protoco ( tuple-like 連結協定)](https://timsong-cpp.github.io/cppwp/n4861/dcl.struct.bind#4)，簡單來說會長這樣：

    +  `std::tuple_size<__E>::value` 必須是個格式正確的整數常量表達式 (integer constant expression)
    +  identifier-list 內元素的數量必須與 `std::tuple_size<__E>::value` 相同
    +  如果上面兩項有其中一項不符合，便去檢查這個 Class 的成員變數是否都為 public，如果不是(有 private 的成員變數)，則編譯錯誤

    接著 identifier-list 內的元素便會「連結」到 `__e` 內相對應的元素，這也是妳有寫 `&` 時，對 identifier-list 內的元素做改動就能改動到原容器的原因，因為 `__e` 是個參考，舉個例子：

    ```cpp
    std::tuple<int,int> a{1,2}
    auto [x,y] = a; // __e 是個容器， x 與 std::get<0>(__e) 連結， y 與 std::get<1>(__e) 連結
    auto &[x,y] = a; // __e 是 a 的左值參考，  x 與 std::get<0>(__e) 連結， y 與 std::get<1>(__e) 連結
    auto &&[x, y] = std::make_tuple( 1, 2 ); // __e 是右邊那個 tuple 的右值參考，x 與 std::get<0>(__e) 連結， y 與 std::get<1>(__e) 連結
    ```

    Code 有點長，大家可以複製下來看，在網頁上可能不太好閱讀。 可以看見內部是使用 `std::get<>()` 來存取元素的，因此妳的 expression 必須是個回傳 tuple-like 容器的敘述，否則妳的 `__e` 不會是個 tuple-like 的容器(或容器的參考)，那個也就無法使用 `std::get<>()` 了
	
    :::info  
    :bulb: 這邊只舉了 tuple-like 容器的例子，因為原生陣列沒有複製建構子，也就是說他不能被改寫成上面那三種樣式，也就不能用那三種方法初始化，可以看下面這個例子，它會噴錯：  

    ```cpp
    int a[2]{ 1, 2 };
    int b[2] = a;
    ```

    所以會需要另外規定方式來初始化。  
    :::

    「連結」這個動作無法以 C\+\+ 語言描述，妳可以把他想像成參考，又或是宏定義，但要記得他不是，他是 C++ 語言的本身，沒辦法用 C++ 寫出來，已經類似語言特性的概念了，就好像我們無法自己實作 function-body 的大括號一樣 (感謝Cy解釋)

    :::danger  
    :bulb: 官方文件是這麼寫的： Structured Binding 像是個參考，它是某個已經存在的物件的別名，但 Structured Binding 不是參考，它不需要是個引用類型。  
    :::

    ~~挺玄學的~~，我自己是用「類似宏定義」來理解的，底下也會如此解釋，但各位要記得它不是宏定義，也許是為了確保將標示符丟進 `std::remove_reference_t<decltype((標示符))>()` 時型態要與連結到的元素丟進 `std::remove_reference_t<decltype((連結到的元素))>()` 一樣才如此設計的

接下來我會詳細的講解一下內部的原理，這裡用 `__E` 來表示 `__e` 的型態，也就是說 `__E` 為初始化敘述(initializer) 的型態，另外我們也可以說 `__E` 與 `std::remove_reference_t<decltype((__e))>` 等價

上述的初始化結束後，它會根據 `__E` 的狀況來進行連結，會有三種情況：

+ **如果 `__E` 是個 array 型態 ，那麼 identifier-list 內的元素會與初始化敘述(initializer) 內相對應的元素連結**

    這種情況下，每個 identifier-list 內的標示符會是一個左值(lvalue)，與初始化敘述(initializer) 內相應的元素連結，也因此，identifier-list 內的標示符數量需要與 array 內的變數數量一樣多，看一下下面這個例子：
    
    ```cpp
    int a[2] = {1,2};
    auto [x,y] = a;
    ```
    
    `auto [x,y] = a;` 會創建一個名字叫 `__e` 的 array `__e[2]`，利用[複製初始化](https://en.cppreference.com/w/cpp/language/copy_initialization)來初始化 `__e[2]`，之後 x 與 y 分別會與 `__e[0]` 與 `__e[1]` 連結，你可以把他們想像成參考，或是宏定義，但要記住它們實際上不是

    如果有寫 ref-qualifier 且 expression 回傳的是 lvalue，則 identifier-list 內的元素會間接與 a 內的元素連結，對 identifier-list 內的元素的操作將會反應到 a 的元素上：
    
    ```cpp
    int a[2] = {1,2};
    auto& [x,y] = a;
    ```
    
    `auto& [x,y] = a;` 會創建一個名字叫 `__e` 的參考引用 expression 的計算結果，而 identifier-list 內的元素則會透過 `__e` 間接變為 a 內元素的參考，可以把他想像成這樣：
    
    ```cpp
    int a[2] = {1,2};
    auto & __e = a;    // 等價於 int(&e)[2] = a;
    #define x __e[0]
    #define y __e[1]
    ```
    
    而如果 expression 回傳的是 rvalue，則 identifier-list 內的元素會與 `__e` 內的元素連結：
    
    ```cpp
    using T = int[3];
    auto &&[x, y, z] = T{ 1, 2, 3 };
    ```
    
    可以把他想像成這樣：
    
    ```cpp
    auto &&__e = T{ 1, 2, 3 };
    #define x __e[0]
    #define y __e[1]
    #define z __e[2]
    ```

    當然上面這兩個例子都是偽代碼，內部當然不是這樣的，連結無法以 C++ 語言來描述，x 與 y 僅僅是標示符，所以不會是上面這個樣子，這只是個示意

+ **如果 `__E` 是個沒有 union 成員的 Class 型態，而且 [std::tuple_size\<__E>](https://en.cppreference.com/w/cpp/utility/tuple/tuple_size) 是個有成員的完全型(不用管這個成員的型態或可訪問性如何)，簡單來說就是 `__e` 能夠做成 tuple-like 的容器，符合 tuple-like 連結協定，那麼就會使用 tuple-like 連結協定來進行連結**

    與前面提到的一樣，首先 `std::tuple_size<__E>::value` 必須是個格式正確的整數常量表達式 (integer constant expression)，並且 identifier-list 內元素的數量必須與 `std::tuple_size<__E>::value` 相同

    再來對於每個標示符，都會連結一個元素(也就是 `__e` 內的元素)，元素的型態會類似是 「`std::tuple_element<i,__E>::type` 的 "引用"」，注意它是「引用」，`i` 指的是 `__e` 內第 i 個元素，如果這個型態對應的初始化敘述(initializer) 是左值，那這個變數就會是左值引用，如果是右值那就是右值引用

    連結到的第 i 個元素詳細如下：

    +  如果通過 [Class成員訪問](https://en.cppreference.com/w/cpp/language/operator_member_access)的方式在 `__E` 的範圍內查找到至少一個函式模板，且這個函式模板的第一個模板參數是個 [non-type參數](https://www.learncpp.com/cpp-tutorial/template-non-type-parameters/)，那麼第 i 個元素的初始化敘述(initializer) 會是 `e.get<i>()`

    + 如果沒有找到符合情況的函式模板，那麼會使用 [argument-dependent lookup](https://en.cppreference.com/w/cpp/language/adl) 的方式來呼叫 `get<i>(__e)`，因此第 i 個元素的初始化敘述(initializer) 會是 `e.get<i>(__e)`

    在這些初始化敘述中，如果 `__e` 是一個左值參考 (這只會發生在你的 `ref-qualifier` 是 `&` ，或是你的初始化敘述是個左值而且 `ref-qualifier` 是 `&&` ，簡單來說就是收合為 `&` 時)，那麼你將 expression 內相對應的元素會是一個左值 (這聽起來很廢話，但重點在下一句)

    否則 expression 內相對應的元素會是一個消亡值(xvalue)，因為內部實際上執行了一次完美轉發(perfect-forwarding)， 而 `i` 會是個型態為 `std::size_t` 的純右值(prvalue)，因此 `<i>` 會被轉換(解釋)為[模板參數列表](https://en.cppreference.com/w/cpp/language/template_parameters)

    :::info  
    :bulb: 有三點提醒大家一下
    + identifier-list 內的標示符、`__e` 內的元素與 expression 內相對應的元素，這三個會有一樣的生命週期
    + 我們通常會直接稱 identifier-list 內的標示符為「變數」，儘管它不是，但它使用上與變數基本上一樣，概念也類似
    +  identifier-list 內第i個元素型態會是 `std::tuple_element<i,E>::type`
    :::

    看一下這個例子：

    ```cpp
    float x{};
	  char y{};
	  int z{};

	  std::tuple<float &, char &&, int> tpl( x, std::move( y ), z );
	  const auto &[a, b, c] = tpl;
    ```

    a 的名字叫做「Structured Binding」，連結到 tpl內第一個元素， `decltype(a)` 為 `float&`  
    b 的名字叫做「Structured Binding」，連結到 tpl內第二個元素， `decltype(b)` 為 `char&&`  
    c 的名字叫做「Structured Binding」，連結到 tpl內第三個元素， `decltype(c)` 為 `const int`  

+ **如果不是以上兩種情況，則 expression 內的每個 non-static 成員變數都需要是個直接成員或是 expression 的相同基類，而且 Structured Binding 格式需要正確，讓我們能夠間接使用 `__e.name` 來呼叫變數。你的 expression 內不能有匿名或是 union 的成員，identifier-list 內的標示符數量需要與 non-static 的成員變數數量相同。**

    每個 identifier-list 內的標示符都會連結到相對應的成員變數，實際上是連結到 `__e.m_i` ，`m_i` 表示第 i 個成員變數，另外 Structured Binding 支援 bit field 用法，看這個例子：

    ```cpp
    #include <iostream>

    struct S {
        mutable int x1 : 2;
        volatile double y1;
        int z1;
    };

    int main() {
        using f = S;    
        const auto [x, y, z] = f();

        return 0;
    }
    ```

    x 會是個整數左值標示符，連結到一個 2-bit 的整數元素 x1，y 會連結到 const volatile double 的元素 y1

# <span class = "orange">使用 Structured Binding</span> 

> 記得要切換成 C++17 才能夠使用

現在我們舉個簡單的例子([來源](https://www.youtube.com/watch?v=eUsTO5BO3WI&ab_channel=TheCherno))，現在我要定義一個「人」的函式，人會有年齡、名字等等，因此它的回傳型態會是一個 `std::tuple<std::string, int>`：

```cpp
std::tuple<std::string, int> CreatePerson() {
    return { "Cherno", 24 };
}
```

而在 `main` 內我們需要用到資料時，過去需要像這樣：

```cpp
std::tuple<std::string, int> person = CreatePerson();    // 當然你可以用auto
std::string &name = std::get<0>( person );
int &age = std::get<1>( person );
```

而對 tuple 熟悉的朋友可能會使用 `std::tie`：

```cpp
std::string name;
int age;
std::tie( name, age ) = CreatePerson();
```

好多了，我們不需要為了賦值多個變數而額外創個 `person`，但它仍然需要 3 行，又或許我們可以使用 Struct，但在 C++17 後多了一個新特性 Structured Binding，現在我們只需要這樣：

```cpp
auto [name, age] = CreatePerson();
```

而它也不只限定 tuple-like 的容器，也可以與 Struct 和原生陣列連結，看一下這個例子([來源](https://ithelp.ithome.com.tw/articles/10217358))：

```cpp
struct TeaShopOwner {
    int id;
    std::string name;
};

auto GetTeaShopOwner() -> TeaShopOwner {
    TeaShopOwner owner { 1, "test"}; 
    return owner;
}

auto [id, name] = GetTeaShopOwner();   //  id = 1 ， name = "test"
```

如此一來 id 便會等於 1，name 則會是 "test" 了。Structured Binding 的另一個好處是可以搭配 Ranged-based for Loop 使用：

```cpp
struct TeaShopOwner {
    int         id;
    std::string name;
};

std::vector<TeaShopOwner> owners {{1, "COCO"}, {2, "1Shit"}};

// C++17 前的用法
for (const auto& owner : owners) {
    printf("Owner id = %d\n", owner.id);
}

// Using Structured Binding
for (const auto& [id, _] : owners) {
    printf("Owner id = %d\n", id);
}
```

留意上面的 Ranged-based for-loop 接住 `owners` 時，將第二個變數名稱取為 `_`，通常會利用這個手法來表示 `name` 在迴圈裡不受「重視」

以往 C++ 函數的回傳值多是單一型別，如 bool, int。有了 Structured Binding 再搭配其他技巧，在處理回傳值時更有彈性

# <span class = "orange">補充</span> 

+ 對成員的 `get` 進行查找時會忽略可訪問性與非類型模板參數的確切類型。像是 `template<char*> void get();` ，成員將導致使用成員解釋，即使格式是錯的

+ 有些 `[` 前方的聲明僅適用於隱藏變數 `__e` ，而不適用 identifier-list 內的元素：
    ```cpp
    #include <cassert>
    #include <tuple>

    int main() {

      int a = 1, b = 2;
      const auto &[x, y] = std::tie( a, b );    // x and y are of type int&
      auto [z, w] = std::tie( a, b );    // z and w are still of type int&
      assert( &z == &a );    // passes

      return 0;
    }
    ```

+ tuple-like 的意思是使用 `std::tuple_size<>` 會是個完全型，即使它可能導致格式錯誤：
    ```cpp
    #include <iostream>
    #include <tuple>

    struct A {
        int x;
    };

    namespace std {
    template <>
    struct tuple_size<::A> {};
    }    // namespace std

    int main() {

        auto [x] = A{};    // error; the "data member" interpretation is not considered.

        return 0;
    }
    ```

# <span class = "orange">參考資料</span> 

**<a href = "https://en.cppreference.com/w/cpp/language/structured_binding" class = "redlink">1. Structured binding declaration (since C++17)</a>** (文章部分來源，例子來源)

**<a href = "https://codertw.com/%E7%A8%8B%E5%BC%8F%E8%AA%9E%E8%A8%80/449584/" class = "redlink">2. C 17嚐鮮：結構化繫結宣告（Structured Binding Declaration）
</a>**

**<a href = "https://zh-blog.logan.tw/2019/10/29/cxx-17-structured-binding/" class = "redlink">3. C++ 17 結構化綁定</a>** 

**<a href = "https://blog.csdn.net/LYR1994/article/details/105340773?ops_request_misc=&request_id=&biz_id=102&utm_term=Structured%20binding%20declaration&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-4-105340773.first_rank_v2_pc_rank_v29" class = "redlink">4. C++ 学习指南基础（三）</a>**

**<a href = "https://blog.csdn.net/zwvista/article/details/78111346?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522161728731916780264014555%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=161728731916780264014555&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-2-78111346.first_rank_v2_pc_rank_v29&utm_term=Structured+binding+declaration&spm=1018.2226.3001.4187" class = "redlink">5. C++17尝鲜：结构化绑定声明（Structured Binding Declaration）</a>**

**<a href = "https://blog.csdn.net/yangsenuestc/article/details/79840670?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522161728731916780264014555%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=161728731916780264014555&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-1-79840670.first_rank_v2_pc_rank_v29&utm_term=Structured+binding+declaration" class = "redlink">6. C++17新特性(1) -- 结构化绑定初始化(Structured binding declaration)</a>**

**<a href = "https://stackoverflow.com/questions/44963201/when-does-an-incomplete-type-error-occur-in-c" class = "redlink">7. When does an Incomplete Type error occur in C++</a>**

**<a href = "https://en.cppreference.com/w/cpp/types/remove_reference" class = "redlink">8. std::remove_reference</a>**

**<a href = "https://www.mdeditor.tw/pl/pjYZ/zh-tw" class = "redlink">9. remove_reference 引用移除工作原理</a>**

**<a href = "https://en.cppreference.com/w/cpp/types/is_same" class = "redlink">10. std::is_same</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/storage_duration" class = "redlink">11. Storage class specifiers</a>**

**<a href = "https://www.youtube.com/watch?v=eUsTO5BO3WI&ab_channel=TheCherno" class = "redlink">12. STRUCTURED BINDINGS in C++ </a>** (例子來源)

**<a href = "https://stackoverflow.com/questions/53787312/if-with-initializer-in-structured-binding-declaration-example-ill-formed" class = "redlink">13. if-with-initializer in structured binding declaration example ill formed?</a>**

**<a href = "https://blog.csdn.net/zqxf123456789/article/details/71582384" class = "redlink">14. [C++] - 中的复制初始化(copy initialization)</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/attributes" class = "redlink">15. Attribute specifier sequence(since C++11)</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/cv" class = "redlink">16. cv (const and volatile) type qualifiers</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/copy_initialization" class = "redlink">17. Copy initialization</a>**

**<a href = "https://stackoverflow.com/questions/1051379/is-there-a-difference-between-copy-initialization-and-direct-initialization" class = "redlink">18. Is there a difference between copy initialization and direct initialization?</a>**

**<a href = "https://docs.microsoft.com/zh-tw/cpp/cpp/initializers?view=msvc-160" class = "redlink">19. Initializers</a>**

**<a href = "https://stackoverflow.com/questions/22577679/what-is-a-sub-expression-in-c/22577708" class = "redlink">20. What is a sub-expression in C?</a>**

**<a href = "https://en.cppreference.com/w/cpp/utility/tuple/tuple_size" class = "redlink">21. std::tuple_size\<std::tuple></a>**

**<a href = "http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0144r0.pdf" class = "redlink">22. P0144R0</a>**

**<a href = "https://stackoverflow.com/questions/39763517/structured-bindings-implementation-underground-and-stdtuple" class = "redlink">23. Structured bindings implementation underground and std::tuple</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/operator_member_access" class = "redlink"></a>**

**<a href = "" class = "redlink">24. Member access operators</a>**

**<a href = "https://www.learncpp.com/cpp-tutorial/template-non-type-parameters/" class = "redlink">25. Template non-type parameters</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/adl" class = "redlink">26. Argument-dependent lookup</a>**

**<a href = "https://en.cppreference.com/w/cpp/language/template_parameters" class = "redlink"></a>**

**<a href = "" class = "redlink">27. Template parameters and template arguments
</a>**

**<a href = "https://en.cppreference.com/w/cpp/utility/tuple/get" class = "redlink">28. std::get(std::tuple)</a>**

**<a href = "http://eel.is/c++draft/dcl.struct.bind#4" class = "redlink">29. Structured binding declarations [dcl.struct.bind]</a>**

**<a href = "https://stackoverflow.com/questions/49795131/understand-structured-binding-in-c17-by-analogy" class = "redlink">30. Understand structured binding in C++17 by analogy</a>**

**<a href = "https://stackoverflow.com/questions/55329651/structured-binding-on-const" class = "redlink">31. Structured binding on const</a>**

**<a href = "https://stackoverflow.com/questions/45821678/is-always-the-address-of-a-reference-equal-to-the-address-of-origin" class = "redlink">32. Is always the address of a reference equal to the address of origin?</a>**

**<a href = "https://stackoverflow.com/questions/17259531/c-primary-expressions-is-it-primary-expression-or-not" class = "redlink">33. C++ primary expressions - Is it primary expression or not?</a>**

**<a href = "https://ithelp.ithome.com.tw/articles/10217358" class = "redlink">34. DAY 16：Structured Bindings</a>** (例子來源)