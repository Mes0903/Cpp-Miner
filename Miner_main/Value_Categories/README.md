---
title: 礦坑系列 ── 值類別 Value Categoraries
date: 2021-04-27
tag: C++ Miner-main
category: C++ Miner
---

<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> 礦坑系列 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50><br>值類別 Value Categoraries</center></h1>
礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

## 前言

原本想說要一個禮拜寫一篇礦坑，結果發現這種文都要花很多的時間，好像一篇一個月就差不多了XD 這篇文章特別感謝 Kris van Rens，我寫信問了他許多問題，他也很耐心的回答我XD 文章內容有許多部份都參考了他的講稿，他也提供了授權給我，真ㄉ是太感謝了XD

移動語意是 C\+\+11 新增的一個特性，主要是針對物件導向來做的優化，以淺複製(shallow copy) 代替了深複製(deep copy)。另外，由於移動語意的出現，使原先 C\+\+ 的文法分的更加詳細了，讓編譯器替我們做了更多事，提升了許多效率

值類別是 C\+\+11 新產生出來的新文法，其實早在 C\+\+03，甚至是 C\+\+98 時就已經有類似的定義了，不過這些定義在 C\+\+11時進行了大改動，並且在 C\+\+17 時，由於 Copy Elision 與 TMC 的緣故，定義又有一次小改動，本文會以 C++17 的定義為主。那由於這是給編譯器看的，是語言的文法部分，所以有時用組語會看不出個所以然，也因此這個東西十分的抽象，據我所知有修過編譯器相關課程的人會比較容易理解，大家需要自己多多消化一下 XD

這篇原本預計是要講移動語意的，但要講移動之前需要先知道值類別是什麼，也許有人會說知道 rvalue、lvalue 就很夠了，但畢竟都要寫了，我就詳細的把全部都寫下來吧，看完整篇文章相信大家也會大概的抓到移動的感覺，對之後理解移動語意肯定有幫助的

希望這篇文章能幫助大家少翻一點草案，幫大家整理一下重點，同時也希望能讓想深入閱讀的人能夠有中文的資源可以看，~~畢竟不是每個人都像我一樣閒可以在那邊用破英文慢慢看~~，大家可以依自己的興趣來跳著看

##  Variable、Expression 與 Statement
移動語意是一個很大很廣的概念，在看移動語意前我們需要知道「值類別(Value Category)」是什麼。而若要知道什麼是值類別，我們需要先分清楚「變數(Variable)」、「運算式(Expression)」、「陳述式(Statement)」的差別。 這些東西比較偏向程式的文法，那我們就開始吧!

### 變數 Variable

首先大家一開始在學 C++ 都有學過變數，但大家可能沒什麼注意過他的定義，他的定義是這樣的：

> 變數是一種物件(object)，或者是 non-static data member 的 reference [(來源)](https://eel.is/c++draft/basic.pre#6)

一般在沒有編譯器優化的情況下，宣告一個變數的時候我們會在記憶體裡面挖一塊空間，並在這塊記憶體空間上加上識別字(identifier)，如此一來，我們便能通過這個 identifier 對這個記憶體的位址操作，像是賦值之類的，看看這個例子：

```cpp
#include <iostream>

int main() {
    int a = 4;
    int b[5];

    std::cout << "&a = " << &a << '\n';    //&a = 0x7f485ffb3c
    std::cout << "&b[3] = " << &b[3];    //&b[3] = 0x7f485ffb2c

    return 0;
}
```

可以看見我們能過通過 `a`、`b` 來對它標記到的記憶體進行操作

一般來說，「變數」是個內容可更改的物件(除非我們加了 cv 限定詞等綴詞)，但「常量值」不能更改，有時它甚至沒有記憶體位址，變數可以拿來接運算式回傳的值

另外變數並不一定要有記憶體位址，因為 C++11 後變數的定義多了 Reference，而 Reference 可能沒有記憶體位址。 簡單來說，變數只是一個在妳程式中，能夠存取一個值的識別字(identifier)，至於實際上有沒有 memory allocate 則不是這麼重要，因為這很看編譯器優化與作業系統，但他肯定會有 logic address

> 這邊的物件照著 C++ 標準的規定：<br><br>
>
> 在 C++ 中，物件(object) 一定要有
> + 大小 (可以用 `sizeof` 來檢查)
> + 對齊需求 (可以用 `alignof` 來檢查)
> + 儲存期限 (自動、靜態、動態、執行緒局部儲存)
> + 生命週期 (暫時或根據儲存期限來決定)
> + 型態(type)
> + 值 (可能是不確定的，像是有預設初始化的 non-class types)<br><br>
>
> 而名字則不一定要有

### 運算式 Expression

一個運算式由一個以上的運算元(operand) 和運算子組成，用來表示一個計算式，被計算後它可能會「回傳一個結果」，又或可能會有額外的作用(side-effects)，像是 `std::printf("%d", 4)` 會去呼叫 system call，印出一個 4

讓我們看看下面這些例子：

```cpp
42    //一個常數的運算式，回傳42
17 + 42    //一個加法的運算式，回傳59

int a;
a = 23    //一個賦值的運算式，回傳23
a + 17    //一個加法的運算式，回傳40

static_cast<float>(a)    //一個轉型的運算式，回傳「運算式」 a 的float
sizeof a    //一個計算byte size的運算式，回傳「運算式」 a 的 byte size
[]{return 3;}    //一個回傳 3 的 lambda 運算式
printf("Hi!")    //一個有額外作用的運算式，回傳值被捨棄了(discarded-value)，其實它的回傳值是輸出的字數，酷ㄅ
```

注意我都沒寫分號，因為運算式是一個「組件」，一但加上分號，他就不是一個「組件」，而是一個陳述句了。也因此上面這個程式肯定是無法正常運作的，因為他們都不是陳述句，程式運作的是陳述句

運算式主要有分「主運算式(Primary Expression)」、「非計算運算式(Unevaluated expressions)」與「棄值運算式(Discarded-value Expression)」，但這不是今天我們的主軸，有興趣的可以到 <strong>[cppreference](https://en.cppreference.com/w/cpp/language/expressions)</strong> 看

### 陳述句 Statement

最後是陳述句，一個陳述句是程式執行的基本元素，通常最後面會有 `;` 來結尾。 陳述句會照順序被執行，它可能會單獨一行出現，但我們也可以利用 `{}` 將許多陳述句包成一個新的陳述句，這個我們稱它為「複合陳述句(Compound Statement)」。 而運算式通常會是陳述句的一部分，另外，還有一種陳述句叫「運算陳述句(Expression Statement)」，這種陳述句裡面只有運算式

陳述句有分下面這幾種：

1. 空陳述句 (Null Statement)
2. 運算陳述句 (Expression Statement)
3. 複合陳述句 (Compound Statement)
4. 選擇陳述句 (Selection Statement)
5. 迭代陳述句 (Iteration Statement)
6. 跳躍陳述句 (Jump Statement)
7. 宣告陳述句 (Declaration Statement)
8. 錯誤嘗試區塊 (Try Blocks)
9. 多執行緒區塊 (Atomic and Synchronized Blocks)

要注意的是「宣告」本身就算一個 Statement 而不是 Expression，因此我們無法像這樣做：
```cpp
int main() {
    int a = ( int b = 5 );
}
```

錯誤訊息：

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Value_Categories/image/image1.png?raw=true">

</center><br>

原因是因為 `int b = 5` 本身就是一個 Statement 了，所以 Compiler 並不希望妳把 `int b = 5` 放在 `int a = ` 的右邊，它原先預期右邊要是一個 Expression，所以就噴了這個錯

然後我們將運算陳述句拿出來講一下，因為要分清楚運算式跟陳述句的差別，可以直接用這個例子來看，其實就只差了一個分號，因此一個可執行，一個不行。 其他的大家有興趣可以到 <strong>[cppreference](https://en.cppreference.com/w/cpp/language/statements)</strong> 上看

運算陳述句的結構長這樣：

> expression(optional);

attr 是<strong>[屬性識別字](https://en.cppreference.com/w/cpp/language/attributes)</strong>，用來告訴編譯器一些事情，妳可以想像成給編譯器看的註解，可加可不加

而 expression 就是前面提到的運算式，「可加可不加」，如果不加就會變一個空陳述句，也就是只有一個分號的陳式句，空陳述句通常用來提供 for、while 迴圈一個空的代碼塊(body)，或是一個標籤(label)的代碼塊

程式裡大部分的陳述句都是運算陳述句，像是賦值或是函式的呼叫等都是

## 值類別 Value Categories

那麼我們現在知道什麼是淺複製與深複製，也知道變數、運算式和陳述式差在哪了，那我們就可以來討論什麼是 Value Categories 了，首先有一點很重要，那就是值類別是用來判斷<strong>運算式的種類</strong>的，而不是給物件實體用的，因為超重要，所以講三次，是<strong>運算式的種類!! 運算式的種類!! </strong><strong>運算式的種類!!</strong>，如果不知道這個，我們接下來講的妳大概也都聽不懂，或者是一知半解了

運算式有兩種獨立的特性，分別是「[Type](https://en.cppreference.com/w/cpp/language/type)」和「[Value Catories](https://en.cppreference.com/w/cpp/language/value_category?fbclid=IwAR0MGKszwMzsKrkc-OHb5cHIUrF5hTWelt1xzqBrCoooXCGKhrsSkqEycWo)」，Type 不是我們今天討論的主題，我們直接來看 Value Categories

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Value_Categories/image/image2.png?raw=true">

([圖源](https://krisvanrens.github.io/slides/value-categories-talk-cpp-it/talk.html#/expressions-in-c))

</center><br>

Value Categories 主要分三種：<strong>Lvalue</strong>、<strong>Xvalue</strong> 和 <strong>Prvalue</strong>。如果你曾讀過值類別，可能還會聽過 glvalue 與 rvalue，那這兩個又是什麼呢? 這其實與運算式發展的歷史有關，是一個方便記憶而創造出的名詞，glvalue = Lvalue + Xvalue，rvalue = Prvalue + Xvalue。讀到這裡妳可能有點頭花，我們看這兩張圖來方便自己記憶：

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Value_Categories/image/image3.png?raw=true">

[圖源](https://docs.microsoft.com/zh-tw/windows/uwp/cpp-and-winrt-apis/cpp-value-categories?fbclid=IwAR0pCGputntm0KZ1AgDiysUB4m8HNjRQqgNUeCao8mhYLec9i3nFDZMst94)<br>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Value_Categories/image/image4.png?raw=true">

[圖源](https://openhome.cc/Gossip/CppGossip/RvalueReference.html)

</center><br>

是不是清楚多了，那麼這邊我先將每個的翻譯名稱都給大家，以免之後在閱讀的時候搞混：

+ glvalue：泛左值
+ rvalue：右值
+ lvalue：左值
+ xvalue：將亡值
+ prvalue：純右值

#### 判斷依據

那接下來就來看看這些分類是怎麼分的吧! 主要有兩個判斷依據：

1. 這個運算式回傳的是有身分標識符(identity)的物件嗎? 還是僅僅是個常量值?
2. 如果是有身分標識符的物件，那這個物件的值可以安全的「移動」給別人嗎?

##### 1. 這個運算式回傳的是有身分標識符(identity)的物件嗎? 還是僅僅是個常量值

首先我們先來看一下，什麼叫做 「有身分標識符(identity)」 呢?，有身分標識符表示我們能夠有辦法去<strong>證明這個 Expreesion 回傳的物件與另一個 Expression 回傳的物件相</strong>，方法隨便，最常見的就是取址，我們看一下這個例子幫大家理解一下什麼叫做「有身分標識符」：

```cpp
#include <iostream>

int main() {
    int a;
    std::cout << &a << '\n';    // 0x67bd3ffaec
    std::cout << &a << '\n';    // 0x67bd3ffaec
}
```

這是個非常直覺的例子，因為 a 是個有記憶體位址的變數，所以上下兩行 `a` 這個 expression 回傳的物件都是「變數 `a`」，我們透過取址來證明了這件事

要注意的是我們會說「有」身分標識符，但並不會特別去提什麼是「身分標識符(identity)」，它是一個概念，並不是像 identifier 這樣有一個東西叫做 identifier

那麼接下來我們大略的看一下一些例子，讓大家感受一下那個感覺：
```cpp
int a;

a    //擁有 identity
a + 2    //沒有identity
a || true    //沒有identity
a++    //沒有identity
++a    //擁有identity
42    //沒有 identity
nullptr    //沒有 identity
false    //沒有 identity
[]{return 42;}    //沒有 identity
"Hello world"    //擁有 identity!!
std::cout    //擁有 identity，std::cout 是 std::ostream 的 instance
static_cast<int>(a)    //沒有 identity
std::move(a)    //擁有 identity
```

##### 2. 如果是有身分標識符的物件，那這個物件的值可以安全的「移動」給別人嗎

再來看第二點，那麼什麼時候運算式回傳的物件可以安全的「移動」給別人呢? 如果這個物件是個匿名的「暫時」物件，或者這個物件的生命週期快要結束了，我們就稱它可以安全的「移動」給別人，是不是已經有一點移動語意的感覺了ㄋ? 沒錯，這94移動語意的核心，讓我們來看看接下來的例子：

```cpp
#include <iostream>
#include <vector>

std::string func() {
    return "Steal me!";
}

int main() {
    std::vector<std::string> vec;
    vec.push_back( func() );

    std::string x{ "Steal me!" };
    vec.push_back( std::move( x ) );
    return 0;
}
```

`func()` 會回傳一個字串，而我們想要將這個回傳的字串推入 `vector`，現在問題來了，這個字串能不能安全的「移動」給別人呢? 如果可以，那麼我們將可以少一次的複製，因為我們可以直接將這個字串「移動」進 `vector`，，而不是複製一個一樣的字串後推入 `vector`。 那麼這邊答案是可以! 因為 `func()` 回傳的字串在 `push_back()` 後就要被解構了，因此我們可以將他的資源接收過來，而不是複製後再將 `func()` 的字串解構

再來下面有另一個例子，我們有個 `std::string x`，並且我們想將他「移動」進 `vector`，原本不應該可以移動的，因為 `std::string x` 等等並不會被解構，但我們可以利用 `std::move(x)` 來讓他強制「移動」

原則上來講，如果一個物件僅僅是為了「傳遞」資訊，或「暫時存取」某個東西，而不是用來「儲存」某些資訊的，那麼他就不可被「移動」

那麼這樣我們就可以開始看什麼是 Lvalue、Prvalue 與 Xvalue 了!

## Lvalue Expression

Lvalue，中文翻作左值，一個 Lvalue Expression 回傳的物件擁有「身分標識符(identity)」，並且在正常的情況下它不該被「移動」，只能複製。 這代表一個 Lvalue Expression 回傳的物件會有一個記憶體位址，用途常會類似是「儲存」一個東西，像變數一樣。 我們能夠透過這個名稱，在它「生成出來的那行外」來使用這塊記憶體內的東西

我們看看 Stackoverflow 上的一個[例子](https://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues?fbclid=IwAR1BIns58XNgIy3KmXgFuc33KHkUC1mS10nUYvQec6BqxXK5BlUXM_f9UfE)：

```cpp
#include <iostream>

int i = 7;

const int& f() {
    return i;
}

int main() {
    std::cout<<&"www"<<std::endl;    //0x7ff7c0ca5050
    std::cout<<&"www"<<std::endl;    //0x7ff7c0ca5050

    i;
    i;

    int* p_i = new int(7);
    *p_i;
    *p_i;

    const int& r_I = 7;
    r_I;
    r_I;

    f();
    i;

    return 0;
}
```

第 10 行的 `"www"` 是 Lvalue expression，因為它回傳的物件與第 11 行的 `"www"` 是同一個，它回傳的物件的用處是「儲存」一段文字，並且可以「在外部呼叫」

第 13 行的 `i` 是 Lvalue expression，因為它回傳的物件與第 14 行的 `i` 是同一個，它回傳的物件的用處是「儲存」一個整數，並且可以「在外部呼叫」

第 17 行的 `*p_i` 是 Lvalue expression，因為它回傳的物件與第 18 行的 `*p_i` 是同一個，它回傳的物件的用處是「儲存」一個我們 `new` 出來的整數，並且可以「在外部呼叫」

第 21 行的 `r_I` 是 Lvalue expression，因為它回傳的物件與第 22 行的 `r_I` 是同一個，它回傳的物件的用處是「儲存」一個整數，並且可以「在外部使用」

第 24 行的 `f()` 是 Lvalue expression，因為它回傳的物件與第 25 行的 `i` 是同一個，它回傳的物件的用處是「儲存」一個整數，並且可以「在外部使用」

不知道大家有沒有感受到那個精神，小心不要把他們想成 variable，因為這邊講的是文法，而不是在說記憶體內部的分配，所以他們是 expression，不是variable

#### 種類

那 Lvalue Expression 有以下這些 [(來源)](https://en.cppreference.com/w/cpp/language/value_category#lvalue)：

+ 變數的名稱、函式本體、[模板參數物件(C++20)](https://en.cppreference.com/w/cpp/language/template_parameters#Non-type_template_parameter)、或一個資料成員，像是 `std::cin` 或 `std::endl`

    函式本體的位址我們可以這樣印出來看：
    ```cpp
    #include <iostream>

    int f() { return 0; };

    int main() {
        std::cout << f << '\n';    //1
        std::cout << reinterpret_cast<void *>( f );    //0x7ff6292217e0
    }
    ```
    不能直接印出來看的原因是 `ostream` 的 `<<` 並沒有重載函式指標，但在標準中規範了函式指標可以隱式轉換成布林值，並且布林值會是 `true` [(來源1)](http://eel.is/c++draft/conv.bool)、[(來源2)](https://en.cppreference.com/w/cpp/language/implicit_conversion#Boolean_conversions)，因此我們需要將其轉換為 `void*` 或其他指標型態才能把它印出來看

    而 [模板參數物件(C++20)](https://en.cppreference.com/w/cpp/language/template_parameters#Non-type_template_parameter) 則是有靜態的儲存位址，所以是 Lvalue Expression

+ 回傳 Lvalue Reference 的運算子重載或函式呼叫(function call)，像是 `std::getline(std::cin, str)` ， `std::cout << 1` ， `str1 = str2` 或 `++it` 等等

+ 利用了[賦值運算子或複合賦值運算子](https://en.cppreference.com/w/cpp/language/operator_assignment)的運算式，像是`a = b`、`a += b`、`a %= b`

+ 利用了[間接取值運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_indirection_operator)的運算式，像是 `*p`

+ 利用了 [注標運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_subscript_operator) 的運算式，像是 `a[n]`、`p[n]` ，但注意 `a` 要是一個陣列左值，不能是 [陣列右值](https://en.cppreference.com/w/cpp/language/array#Array_rvalues) 之類的東西

+ 利用了 [成員物件訪問運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_member_access_operators) 的運算式，像是 `a.m`，但 `m` 不能是 `enum` 的成員、某些非靜態的成員函式，或一個 class prvalue 內的非靜態成員(以上例來說就是 `a` 是 class prvalue 且 `m` 是非靜態成員)

    class prvalue 和 array rvalue 可以看這個[例子](https://en.cppreference.com/w/cpp/language/array#Array_rvalues)，我有稍微改一下：
    ```cpp
    #include <iostream>
	#include <type_traits>
	#include <utility>

	// x 可以是 xvalue 或 prvalue 的參考 (因為他是rvalue reference)
	void f( int( &&rr )[2][3] ) {
	    std::cout << " " << sizeof( rr ) << '\n';
	}

	struct X {
	    X() { std::cout << " X()" << '\n'; }
	    int i[2][3];
	    ~X() { std::cout << " ~X()" << '\n'; }
	};

	template <typename T>
	using identity = T;
	int main() {
	    std::cout << "sizeof( X().i ) = " << sizeof( X().i ) << '\n';    // size of the array

	    std::cout << "\n now call f( X().i )\n";
	    f( X().i );    // okay: rr binds to xvalue

	    // f(x.i);    // error: cannot bind to lvalue

	    int a[2][3];
	    std::cout << "\n now call f( std::move( a ) )\n";
	    f( std::move( a ) );    // okay: rr binds to xvalue

	    using arr_t = int[2][3];
	    std::cout << "\n now call f( arr_t{} )\n";
	    f( arr_t{} );    // okay: rr binds to prvalue

	    std::cout << "\n now callf( identity<int[][3]>{ { 1, 2, 3 }, { 4, 5, 6 } } )\n";
	    f( identity<int[][3]>{ { 1, 2, 3 }, { 4, 5, 6 } } );    // okay: rr binds to prvalue
	}
    ```
    看不懂的話可以等後面的TMC、Copy Elision、RVO讀完後再回來看一次

+ 部分利用了 [成員指標訪問運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_member_access_operators) 的運算式，像是 `p->m`，但 `m` 不能是 `enum` 的成員或某些非靜態的成員函式

+ 部分利用了指向「資料成員」(物件) 的 [成員指標運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_pointer-to-member_access_operators) 的運算式，像是 `a.*mp`，`a` 是一個 lvalue 且 `mp` 是一個「資料成員」的指標

+ 部分利用了指向成員「指標」的 [成員指標運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_pointer-to-member_access_operators) 的運算式，像是 `p->*mp`，`mp` 是一個「資料成員」的指標

+ 利用了 [逗號運算子](https://en.cppreference.com/w/cpp/language/operator_other#Built-in_comma_operator) 的運算式，像是 `a, b` ，`b` 要是一個 lvalue

+ (待補) 部分利用了 [三元運算子](https://en.cppreference.com/w/cpp/language/operator_other#Conditional_operator) 的運算式

+ 字串常量 (string literal)，像是 `"Hello World"`

    因為字串常量其實像是一個靜態成員，在整個程式結束前都會被存在某個唯讀的記憶體區段

+ 轉型(cast)成 lvalue reference 型態的運算式，像是 `static_cast<int&>(x)`

+ 轉型(cast)成「函式型態」的 rvalue reference 的運算式，像是 `static_cast<void (&&)(int)>(x)`

+ 回傳「函式」的 rvalue reference 的函式呼叫(function call)

### 特性

那麼 Lvalue Expression 有一些特性，有些是只有 Lvalue Expression 有的，有些是 Xvalue Expression 也有的 (換句話說就是 glvalue 有的)

<strong>僅限 Lvalue Expression 擁有的特性:</strong>

+ 可以被取址

    取址運算符 `&` 的後方必須要接上 Lvalue Expression 或一個 [Qualified-id](https://en.cppreference.com/w/cpp/language/identifiers#Qualified_identifiers)  ([來源](https://timsong-cpp.github.io/cppwp/n4659/expr.unary.op#3))

+ 一個可更改的 Lvalue Expression 可以擺在賦值運算子和複合賦值運算子的左邊

+ Lvalue Expression 可以拿來初始化 lvalue reference，這個動作會給被連結到的物件一個新的識別字(identifier)

<strong>Xvalue Expression 也有的特性</strong>

+ 可被隱式轉換成 prvalue，像是「Lvalue 與 Prvalue的隱式轉換」、「陣列與指標的隱式轉換」、「函式與指標的隱式轉換」

+ 可能是[多形](https://en.cppreference.com/w/cpp/language/object#Polymorphic_objects)的，它標識的對象的[動態類型](https://en.cppreference.com/w/cpp/language/type#Dynamic_type) 不一定是表達式的靜態類型

    如果某個 Lvalue Expression 或 Xvalue Expression 指向了[多形](https://en.cppreference.com/w/cpp/language/object#Polymorphic_objects) 的物件，那麼它衍生的型態(子類)我們稱它為[動態類型](https://en.cppreference.com/w/cpp/language/type#Dynamic_type) :
    ```cpp
	struct B { virtual ~B() {} }; // polymorphic type
	struct D: B {}; // polymorphic type
	D d; // most-derived object
	B* ptr = &d;
	// the static type of (*ptr) is B
	// the dynamic type of (*ptr) is D
    ```
    想知道詳細狀況可以去讀[執行期型態訊息 (RTTI)](https://en.cppreference.com/w/cpp/types#Runtime_type_identification) 的操作

+ 在表達式合法的情況下可以是[不完全型態](https://en.cppreference.com/w/cpp/language/type#Incomplete_type)
## Xvalue Expression

Xvalue，中文翻作將亡值，一個 Xvalue Expression 回傳的物件擁有「身分標識符(identity)」，但是它通常用來「暫時儲存」某個東西。 這代表一個 Xvalue Expression 回傳的物件會有一個記憶體位址，但我們不能夠在它「生成出來的那行外」來使用這塊記憶體內的東西

我們看一下這個例子:
```cpp
#include <iostream>

struct X {
    X() {
        std::cout << " X()" << '\n';
        std::cout << "sizeof(*this) = " << sizeof( *this ) << '\n';
        std::cout << "this = " << this << '\n';
    }
    int i;
    ~X() { std::cout << " ~X()" << '\n'; }
};

int main() {
    X().i;    //this is a temporary object
    X().i;    //this is a temporary object too
}
```
輸出：
```cpp
 X()
sizeof(*this) = 4
this = 0x878d9ff9f8
 ~X()
 X()
sizeof(*this) = 4
this = 0x878d9ff9fc
 ~X()
```

在 `main` 內有兩行同樣的 Expression `X().i`，這個 Expression 會生成一個暫時的匿名物件，所以我們可以看到輸出裡面第一個 `X().i` 執行了建構之後馬上又解構了，如果我們沒有用記憶體標識符去抓住它，它就消失了，因為它是一個暫時物件，所以這個 Expression 為 Xvalue Expression。 這邊我呼叫了兩次，大家可以看見上下兩行的暫時物件的位址不一樣，他們不是同一個物件，我們無法在「外部呼叫」它

#### 種類

那 Xvalue Expression 有以下這些 [(來源)](https://en.cppreference.com/w/cpp/language/value_category#xvalue)：

+ 回傳「物件」的 rvalue reference 的函式呼叫 (function call) 或運算子重載表達式，像是 `std::move(x)`

+ 利用了[注標運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_subscript_operator)的運算式，像是 `a[n]`，`a` 要是一個[陣列右值](https://en.cppreference.com/w/cpp/language/array#Array_rvalues) 

+ 部分利用了 [成員物件訪問運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_member_access_operators) 的運算式，像是 `a.m`，但 `a` 要是 rvalue 而且 `m` 是一個非引用類型的非靜態的資料成員

+ 部分利用了指向「資料成員」(物件) 的 [成員指標運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_pointer-to-member_access_operators) 的運算式，像是 `a.*mp`，`a` 是一個 rvalue 且 `mp` 是一個「資料成員」的指標

+ (待補) 部分利用了 [三元運算子](https://en.cppreference.com/w/cpp/language/operator_other#Conditional_operator) 的運算式

+ 轉型(cast)成「物件」的 rvalue reference 的運算式，像是 `static_cast<char&&>(x)`

+ 經過 TMC 後產生出暫時物件的運算式

    TMC 是一個挺重要的東西，後面會提到

#### 特性

Xvalue Expression 的特性會與 Lvalue Expression 或 Prvalue Expression 其中一個共用，因為 Xvalue Expression 算是這兩個的中間過度區，官方因此才分出了 glvalue 與 rvalue，簡單來說，與 Lvalue Expression 共用的特性我們稱它為 glvalue 的特性，與 Prvalue Expression 共用的特性我們稱它為 rvalue 的特性，但我覺得再分這兩個出來可能會變得更亂，所以我這邊就不分了，直接列出來

<strong>與 Lvalue Expression 共用的特性</strong>

+ 可被隱式轉換成 prvalue，像是「Lvalue 與 Prvalue的隱式轉換」、「陣列與指標的隱式轉換」、「函式與指標的隱式轉換」

+ 可能是 [多形](https://en.cppreference.com/w/cpp/language/object#Polymorphic_objects) 的，它標識的對象的[動態類型](https://en.cppreference.com/w/cpp/language/type#Dynamic_type)不一定是表達式的靜態類型

    如果某個 Lvalue Expression 或 Xvalue Expression 指向了[多形](https://en.cppreference.com/w/cpp/language/object#Polymorphic_objects)的物件，那麼它衍生的型態(子類)我們稱它為 [動態類型](https://en.cppreference.com/w/cpp/language/type#Dynamic_type) :
    ```cpp
	struct B { virtual ~B() {} }; // polymorphic type
	struct D: B {}; // polymorphic type
	D d; // most-derived object
	B* ptr = &d;
	// the static type of (*ptr) is B
	// the dynamic type of (*ptr) is D
    ```
    想知道詳細狀況可以去讀 [執行期型態訊息 (RTTI)](https://en.cppreference.com/w/cpp/types#Runtime_type_identification) 的操作

+ 在表達式合法的情況下可以是 [不完全型態](https://en.cppreference.com/w/cpp/language/type#Incomplete_type)

<strong>與 Prvalue Expression 共用的特性</strong>

+ 不能被取址

    因為取址運算符 `&` 的後方必須要接上 Lvalue Expression 或一個 [Qualified-id](https://en.cppreference.com/w/cpp/language/identifiers#Qualified_identifiers)  [(來源)](https://timsong-cpp.github.io/cppwp/n4659/expr.unary.op#3)

+ 不能擺在賦值運算子和複合賦值運算子的左邊

+ Xvalue Expression 和 Prvalue Expression 可以拿來初始化「唯讀」的 lvalue reference，如此一來暫時物件的生命週期會被延長，直到這個 reference 所處的範圍 (scope) 結束

+ Xvalue Expression 和 Prvalue Expression 可以拿來初始化 rvalue reference，如此一來暫時物件的生命週期會被延長，直到這個 reference 所處的範圍 (scope) 結束

+ 當 Xvalue Expression 或 Prvalue Expression 被當作函式的參數傳入時，如果這個函式有多載(overload)，一個參數吃 rvalue reference，一個參數吃「唯讀」的 lvalue reference，那麼會調用的是吃 rvalue reference 的函式：

    ```cpp
    #include <iostream>

	void test( const int &a ) {
	    puts( "test function with const lvalue reference parameter" );
	}

	void test( int &&a ) {
	    puts( "test function with rvalue reference parameter" );
	}

	int main() {
	    int a{};
	    test( std::move( a ) );
	}
    ```
    輸出：
    ```sh
    test function with rvalue reference parameter
    ```

    可以看見的它傳入的是 `int &&a` 版本的 `test` 函式，如果這邊還看不懂什麼是 rvalue reference 沒關係，可以等後面讀完再回來看前面的例子

### Prvalue Expression

Prvalue，中文翻作純右值，一個 Prvalue Expression 回傳的是一個常量，或是沒有身分標識符的物件，在定義上 Prvalue 可以被移動

雖然因為 Copy Elison 與 TMC 的關係，在 C\+\+17 時 Prvalue 的定義又稍微有了變動，但沒關係，我們這邊先以 C\+\+17 前的定義來解釋，後面講完 Copi Elision 和 TMC 時再談談 Prvalue 的改動

我們看一下這個例子：

```cpp
int foo() {
    return 0;
}

int main () {
    0;    // 5 is prvalue
    foo();    // foo() is prvalue
}
```

上例中 `0` 是一個常量，所以它是 Prvalue，而 `foo()` 回傳的是一個整數，是一個純量，沒有 identity ，所以它是 prvalue

#### 種類

那 Prvalue Expression 有以下這些 [(來源)](https://en.cppreference.com/w/cpp/language/value_category#prvalue)：

+ [字面常量](https://en.cppreference.com/w/cpp/language/expressions#Literals)，像是 `42`、`true` 或 `nullptr`

+ 回傳非參考類型的運算子重載運算式或函式呼叫(function call)，像是 `str.substr(1,2)` `str1 + str2` 或 `it++`，

+ 利用了[後置遞增、遞減運算子](https://en.cppreference.com/w/cpp/language/operator_incdec#Built-in_postfix_operators)的運算式，像是 `a++`、`a--`

+ 利用了[算術運算子](https://en.cppreference.com/w/cpp/language/operator_arithmetic)的運算式，像是 `a + b`、`a % b`、`a & b`、`a << b` 等等

+ 利用了[邏輯運算子](https://en.cppreference.com/w/cpp/language/operator_logical)的運算式，像是 `a && b`、`a || b`、`!a`

+ 利用了[比較運算子](https://en.cppreference.com/w/cpp/language/operator_comparison)的運算式，像是 `a < b`、`a == b`、`a >= b`

+ 利用了[取址運算符](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_address-of_operator)的運算式，像是 `&a` 

+ 某些利用了 [成員物件訪問運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_member_access_operators) 的運算式，像是 `a.m`，且 `m` 是 `enum` 的成員、某些非靜態的成員函式，或是一個 Xvalue 或 Prvalue 內的非參考類型的非靜態成員(以上例來說就是說 `a` 是 Xvalue 或 Prvalue ，且 `m` 是非參考類型的非靜態成員)

+ 某些利用了 [成員指標訪問運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_member_access_operators) 的運算式，像是 `p->m`，且 `m` 是 `enum` 的成員或非靜態的成員函式

+ 某些利用了指向「資料成員」(物件) 的 [成員指標運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_pointer-to-member_access_operators) 的運算式，像是 `a.*mp`，`mp` 是一個「成員函式」的指標，或者 `a` 是一個 Xvalue 或 Prvalue，且 `mp` 是一個「資料成員」的指標

+ 利用了指向「成員函式的指標」的 [成員指標運算子](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_pointer-to-member_access_operators) 的運算式，像是 `p->*mp`，`mp` 是一個「成員函式」的指標

+ 利用了 [逗號運算子](https://en.cppreference.com/w/cpp/language/operator_other#Built-in_comma_operator) 的運算式，像是 `a, b` ，`b` 要是一個 Xvalue 或 Prvalue

+ (待補) 部分利用了 [三元運算子](https://en.cppreference.com/w/cpp/language/operator_other#Conditional_operator) 的運算式

+ 轉型(cast)成非參考類型的運算式，像是 `static_cast<double>(x)`、`std::string{}` 或 `(int)42`

+ `this` 指標

+ `enumerator`

+ [非型別樣板參數](https://en.cppreference.com/w/cpp/language/template_parameters#Non-type_template_parameter)，除非她的型態(type) 是個 `class` 或一個 lvalue reference

+ 沒有實體化的 [lmabda](https://en.cppreference.com/w/cpp/language/lambda) 運算式，像是 `[](int x){ return x*x; }`

+ [要求表達式 (Requires Expression)](https://en.cppreference.com/w/cpp/language/constraints#Requires_expressions)，像是 `requires (T i) { typename T::type; };`

+ [特殊化的概念 (Specialization of a Concept)](https://en.cppreference.com/w/cpp/language/constraints)，像是 `std::equality_comparable<int>`

#### 特性

那麼 Prvalue Expression 有一些特性，有些是只有 Prvalue Expression 有的，有些是 Xvalue Expression 也有的 (換句話說就是 rvalue 有的)

<strong>僅限 Prvalue Expression 有的特性</strong>

+ Prvalue Expression 不能是多型的，它回傳的物件的動態類型永遠跟表達式的類型一樣

+ 一個不是 Class 或 array 的 Prvalue 不能有[cv限定詞](https://en.cppreference.com/w/cpp/language/cv)。要注意的是函數呼叫(function call) 或轉型運算式可能會變成非 Class 的 [cv限定詞](https://en.cppreference.com/w/cpp/language/cv) 類型的Prvalue，但它的 cv限定詞會馬上被遺棄(無效)

+ Prvalue Expression 不能是除了 `void` [不完全型態](https://en.cppreference.com/w/cpp/language/type#Incomplete_type)，但如果是在 `decltype()` 內就可以

+ 不能有 Prvalue 的 [抽象類別類型](https://en.cppreference.com/w/cpp/language/abstract_class) 或陣列

<strong>與 Xvalue Expression 共有的特性</strong>

+ 不能被取址

    因為取址運算符 `&` 的後方必須要接上 Lvalue Expression 或一個 [Qualified-id](https://en.cppreference.com/w/cpp/language/identifiers#Qualified_identifiers)  [(來源)](https://timsong-cpp.github.io/cppwp/n4659/expr.unary.op#3)

+ 不能擺在賦值運算子和複合賦值運算子的左邊

+ Xvalue Expression 和 Prvalue Expression 可以拿來初始化「唯讀」的 lvalue reference，如此一來暫時物件的生命週期會被延長，直到這個 reference 所處的範圍 (scope) 結束

+ Xvalue Expression 和 Prvalue Expression 可以拿來初始化 rvalue reference，如此一來暫時物件的生命週期會被延長，直到這個 reference 所處的範圍 (scope) 結束

+ 當 Xvalue Expression 或 Prvalue Expression 被當作函式的參數傳入時，如果這個函式有多載(overload)，一個參數吃 rvalue reference，一個參數吃「唯讀」的 lvalue reference，那麼會調用的是吃 rvalue reference 的函式：

    ```cpp
    #include <iostream>

	void test( const int &a ) {
	    puts( "test function with const lvalue reference parameter" );
	}

	void test( int &&a ) {
	    puts( "test function with rvalue reference parameter" );
	}

	int main() {
	    int a{};
	    test( std::move( a ) );
	}
    ```
    輸出：
    ```sh
    test function with rvalue reference parameter
    ```

    可以看見的它傳入的是 `int &&a` 版本的 `test` 函式，如果這邊還看不懂什麼是 rvalue reference 沒關係，可以等後面讀完再回來看前面的例子

## 參考 (Reference)

參考並不是我們今天的主軸，但由於 Rvalue Reference 對理解 Value Categories 挺重要的，所以我這邊稍微提一下 Lvalue Reference 和 Rvalue Reference，至於 Forward Reference 就太遠了，所以這邊就不提了

如果你已經知道什麼是 Lvalue Reference 和 Rvalue Reference 了，那就大膽的跳過這段吧 XD

### 什麼是參考 (Reference) ?

參考是一種變數，其型別(Type) 是他連結到的東西的型態的引用(reference to type)，這邊不講物件的原因是因為他連結到的東西不一定是個物件，也有可能是函式之類的東西，他會像是被連結到的東西的別名一樣，呼叫他，就等於呼叫被連結到的東西

實際上參考並不一定會有 memory allocate，而且參考也不是一個 object，這樣才符合他的精神「別名」。儘管為了實現語意，Compiler 常常會給他 memory allocate，看看這個[例子](https://stackoverflow.com/questions/45821678/is-always-the-address-of-a-reference-equal-to-the-address-of-origin)：

```cpp
#include <iostream>
int main() {
    int var = 10;
    int &real_ref = var;
    struct {
        int &ref;
    } fake_ref = { var };

    std::cout << &var << std::endl;    // var 的位址 (0xa4a7fff6f4)
    std::cout << &real_ref << std::endl;    // 仍然是 var 的位址 (0xa4a7fff6f4)
    std::cout << &fake_ref << std::endl;    // ref 的位址 (0xa4a7fff6e8)

    std::cout << sizeof var << std::endl;    // size of var (4)
    std::cout << sizeof real_ref << std::endl;    // 仍然是 size of var (4)
    std::cout << sizeof fake_ref << std::endl;    // size of ref (4 on 32 bit and 8 on 64 bit compiler)

    fake_ref.ref = 5;
    std::cout << var << std::endl;    // 5 (改到實際的 var)

    return 0;
}
```

我們透過「僅有單一成員的 struct 的位址、大小與第一個元素相同」的特性將 `ref` 的位址與 `size` 給印了出來，算是有點破壞規則，但在這個情況下，將 Compiler 的優化關掉，他就會擁有 memory location，如此一來我們便能看見 Reference 的位址了，記得在寫程式的時候別這樣做呀XD 不然就違反 Refernce 的精神了，這只是個方便大家思考的例子

再來，參考一定要被初始化，來看下面這個[例子](https://eel.is/c++draft/dcl.init.ref)：
```cpp
int g( int ) noexcept;    //回傳 int 的函式

void f() {
    int i;
    int &r = i;    // r 與 i 連結
    r = 1;    // r 連結到的變數的值變為 1，也就是說 i 變為 1
    int *p = &r;    // p 為 i 的指標
    int &rr = r;    // rr 連結到 r 連結的變數，也就是說 rr 連結到 i
    int ( &rg )( int ) = g;    // rg 是函式 g 的參考，
    rg( i );    // 呼叫 g 函式

    int a[3];
    int( &ra )[3] = a;    // ra 連結到 a，ra 是 a 陣列的參考
    ra[1] = i;    // 改變 a[1] 的值
}
```

一但 reference 被初始化，我們就無法再對 reference 本身進行操作了。我們也不能有參考的參考(reference to reference)，參考的陣列(arrays of reference)，和參考的指標(pointer to references)，因為 reference 並不是物件(object)[(來源)](https://eel.is/c++draft/dcl.ref#5)

這樣看起來，一旦參考被初始化後，參考「本身」好像就消失在程式當中，真的變成一個「別名」了，那麼底層是如何實作的呢? 不一定! 通常是利用指標來實作，但要記得，他不一定要是指標，參考與指標本質上是不同的東西，僅僅是因為他們倆個的性質很相似而已

那麼，Compiler 知道參考本身的存在嗎? 答案是知道：

```cpp
int a = 0;
int &b = a;

std::cout << std::boolalpha 
          << std::is_same_v<decltype(a), decltype(b)>; // false
```

可以看見 Compiler 可以分出來兩者的差別，因為兩者的 type 是不同的。那麼我們來看看一個有編譯器優化的狀況：

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Value_Categories/image/image5.png?raw=true">

</center><br>

[連結](https://godbolt.org/z/8KqxsKrPM)在這裡，記得要把編譯器優化打開。 我們可以看見 `std::cin >> r;` 像是被替換成 `std::cin >> a;` 了，跟 `inline` 類似 (感謝Cy大神補充)

如果對 Referecne 的定義與相關的詳細規範還有興趣，可以到[這裡](https://eel.is/c++draft/dcl.ref)看看

### Lvalue Reference

那麼現在大家都有背景知識了，就來看一下什麼是 Lvalue Referecne 和 Rvalue Referecne 吧

首先我們看 Lvalue Reference，語法長這樣 [(來源)](https://en.cppreference.com/w/cpp/language/reference)：

> & attr(可加可不加) declarator

相信從前面這樣看下來大家都很熟了，`int &r = a;` 這樣的話 r 就是一個 Lvalue Referecne，型態是 `int&`。那麼他有幾個用處：

+ Lvalue Reference 可以用來連結一個已經存在的 object，也就是 Lvalue Expression 回傳的物件：
	```cpp
	#include <iostream>
	#include <string>

	int main() {
	    std::string s = "Ex";
	    std::string &r1 = s;
	    const std::string &r2 = s;

	    r1 += "ample";           // 改動了 s
	//  r2 += "!";               // error: cannot modify through reference to const (不能更改一個 const string 的 reference)
	    std::cout << r2 << '\n'; // Example
	}
	```

+ 可以用來當作 pass-by-reference 的函式參數：
	```cpp
	#include <iostream>
	#include <string>

	void double_string(std::string &s) {
	    s += s; // 's' 和 main() 裡面的 `str` 是同一個物件
	}

	int main() {
	    std::string str = "Test";
	    double_string(str);
	    std::cout << str << '\n';
	}
	```

+ 可以用來當作函式回傳的物件，如此一來函式回傳的型態會是 `int&`：
	```cpp
	#include <iostream>
	#include <string>

	char& char_number(std::string &s, std::size_t n) {
	    return s.at(n); // string::at() 回傳一個reference，型態是 char&
	}

	int main() {
	    std::string str = "Test";
	    char_number(str, 1) = 'a';
	    std::cout << str << '\n'; // Tast
	}
	```

這些差不多就是 Lvalue Reference 的作用了，應該非常符合他是一個「別名」的意義吧 XD

### Rvalue Reference

#### 先談談歷史

在進到 Rvalue Reference 前，我們要先稍微了解一下歷史，才會知道為何要有 Rvalue Reference，所以我們先從舊的 C++ 講起

在以前，我們只擁有 Lvalue Reference，但 Lvalue Reference 沒有辦法連結到沒有 cv 限定詞的暫時物件或常量：

```cpp
#include <iostream>

struct S {
    int i;
};

int main() {
    int &r = 0;    //error: cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'
    int &r2 = S().i;    //error: cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'

    return 0;
}
```

因為 Lvalue Reference 只能綁定到一個 Lvalue Expression 回傳的物件，從語意上來講，更改暫時物件的值不一定是合理的，而常量值更不用說了，他甚至可能不是個物件，沒有儲存位址，要怎麼改動他呢? 由於 Lvalue Refference 綁定到的物件基本上可以被更改(除非他有加上 const 限定詞)，所以我們無法綁定 Prvalue Expression 與 Xvalue Expression (統稱 Rvalue)

那麼這會出現什麼問題呢? 最明顯的問題出在函式 call-by-reference 的身上：

```cpp
#include <iostream>

int func(int &a, int &b) {
    return a + b;
}

int main() {
    int a = 1, b = 1;
    int c = func(a, b);    // ok

    int d = func(1, 1);    //error: cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'
}
```

可以看見 `func(a, b)` 是合法的，這很合理，我們傳了兩個 Lvalue Expression 進去，但是 
`func(1, 1)` 就有問題了，因為這兩個是 Prvalue Expression 但我們的需求也很直覺，就是將傳入的兩個整數相加後回傳

怎麼解決呢? 照剛剛的邏輯，不能利用 Lvalue Reference 做連結的原因是因為 Prvalue 與 Xvalue 回傳的物件照理說不應該被更改。那麼只要我們保證不更改他就好了對吧! 所以答案是<strong>加上 const</strong>：

```cpp
#include <iostream>

int func(const int &a, const int &b) {
    return a + b;
}

int main() {
    int a = 1, b = 1;
    int c = func(a, b);    // ok

    int d = func(1, 1);    // ok
}
```

我們先在這邊暫停一下，剛剛不是說了常量值可能沒有記憶體位址? 那麼就算加上了 `const` 也不該可行呀! 所以這時候出現了一個東西叫做臨時物化(TMC)，我們來看看利用 `const int&` 型態的 reference 綁定 `1` 時到底發生了什麼：

```cpp
int main() {
    const int &cr = 1;
}
```

組語輸出：

```armasm
main:
        push    rbp
        mov     rbp, rsp
        mov     eax, 1
        mov     DWORD PTR [rbp-12], eax
        lea     rax, [rbp-12]
        mov     QWORD PTR [rbp-8], rax
        mov     eax, 0
        pop     rbp
        ret
```

不會組語的朋友沒關係，我這邊寫個偽代碼來示意：

```cpp
int main() {
    int __e = 1;    //mov     DWORD PTR [rbp-12], eax
    const int &cr = __e;    //mov     QWORD PTR [rbp-8], rax
}
```

發現了嗎? 產生了一個新的變數! 其實他是個匿名物件，但為了方便我把它取名叫 `__e`，如此一來我們的確無法透過 `cr` 來更改到 `1`(\_\_e) 的值，卻又連結到了 `1` 這個常量，就好像替 `1` 取了一個別名叫做 `cr`

但這樣又出現了兩個問題，首先是我們無法區分 Rvalue 與 Lvalue，剛剛的例子中可以看見 `1` 和 `a`、`b` 都一樣可以傳進 `func()` 裡面

再來是既然產生了臨時物件，我們也透過 Reference 連結到它了，而且它本身也不是個不可更改的物件，因為在被 Reference 連結後，它的生命週期已經大幅增加，不再是個暫時物件了，那麼也就是說：

```cpp
int main() {
    int __e = 1;
    const int &cr = __e;
    cr;    // cr 是 Lvalue Expression
}
```

`cr` 已經是一個 Lvalue Expression 了，既然我們都知道會有臨時物件出現，並且它的生命週期會因為 Reference 的連結而增加，那麼可以被更改也很合理吧? 但是在函式中很明顯它不這麼覺得：

```cpp
#include <iostream>

int func(const int &a, const int &b) {
    ++a;    //error: increment of read-only reference 'a'
    return a + b;
}

int main() {
    int a = 1, b = 1;
    int c = func(a, b);    // ok

    int d = func(1, 1);    // ok
}
```

`++a` 被擋了下來，這也很正常，因為它是唯讀的，那麼要怎麼做才可以讓它像這樣呢：

```cpp
int main() {
    int __e = 1;
    int &cr = __e;
}
```

如果能夠沒有 `const`，那麼 `++a` 就可以過，事情就解決了對吧？

但這樣就回到了一開始的問題，常量並沒有記憶體位址，語意上來說不該能被更改，我們會說它能更改是從臨時物化與記憶體的角度去看，但在寫程式的時候我們應該由語意方面來寫，而不是底層，因為還有 Compiler 優化與平台差異的問題，底層應該交給他們去判斷。 於是陷入了死胡同，直到 C++11 時為了滿足移動語意，Rvalue Reference 的出現及值類別、TMC、Copy Elision 等更詳細的定義，這個問題才被解決

#### Rvalue Reference 的出現

所以為了滿足移動語意，Rvalue Reference 出現了，大量取代了以前使用 const Reference 的情景。 Rvalue Reference 是 C\+\+11 後的東西，使用前記得看一下自己的 C\+\+ 版本，語法長這樣：

> && attr(可加可不加) declarator

沒錯，就只是多了一個 `&`，意義就大有不同，注意它不是參考的參考，前面已經說過不能有參考的參考了

Rvalue Reference 有幾種特性 [(來源)](https://en.cppreference.com/w/cpp/language/reference#Rvalue_references)：

+ 延長暫時物件的生命週期：

    ```cpp
    #include <iostream>
	#include <string>

	int main() {
	    std::string s1 = "Test";
	//  std::string&& r1 = s1;           // error: can't bind to lvalue

	    const std::string& r2 = s1 + s1; // okay: 利用唯讀的 Lvalue Reference 來延長 s1 + s2 的生命週期
	//  r2 += "Test";                    // error: can't modify through reference to const

	    std::string&& r3 = s1 + s1;      // okay: 利用 Rvalue Reference 來延長 s1 + s2 的生命週期
	    r3 += "Test";                    // okay: 可以更改連結到的物件的值
	    std::cout << r3 << '\n';    //TestTestTest
	}
    ```

    可以看見唯讀的 Lvalue Reference 跟 Rvalue Reference 都可以延長暫時物件的生命週期，但只有 Rvalue Reference 可以更改連結到的物件的值，因為其不是唯讀的型態

+ 函式 call-by-reference 時能夠區分 Rvalue 與 Lvalue：

    ```cpp
	#include <iostream>
	#include <utility>

	void f(int& x) {
	    std::cout << "lvalue reference overload f(" << x << ")\n";
	}

	void f(const int& x) {
	    std::cout << "lvalue reference to const overload f(" << x << ")\n";
	}

	void f(int&& x) {
	    std::cout << "rvalue reference overload f(" << x << ")\n";
	}

	int main() {
	    int i = 1;
	    const int ci = 2;
	    f(i);  // calls f(int&)
	    f(ci); // calls f(const int&)
	    f(3);  // calls f(int&&)
	           // would call f(const int&) if f(int&&) overload wasn't provided
	    f(std::move(i)); // calls f(int&&)

	    // rvalue reference variables are lvalues when used in expressions
	    int&& x = 1;
	    f(x);            // calls f(int& x)
	    f(std::move(x)); // calls f(int&& x)
	}
    ```

    可以看見 Prvalue Expression 與 Xvalue Expression 會傳入 Rvalue Reference 的版本，與 Lvalue Reference 的版本區分開來了

    這是我們能夠實現移動語意的關鍵，因為移動建構子或其他接受移動的函式吃的就是 Xvalue，建構子需要能夠區分 Lvalue 與 Rvalue

## 複製、移動省略 ( Copy / Move Elision )

Copy Elision 在 C\+\+11 與 C\+\+14，甚至更早期的 C\+\+03、C\+\+98 時就存在了，不同的是 C++11 有移動建構子的出現，導致能夠省略的建構子又多了一種。 之後 Copy Elision 在2015年時，於[ P0135R0 提案](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0135r0.html)中被提出應該要在某些情況下保證 Copy Elision 的發生，後來提案被採用，在 C\+\+17 時「部分」保證 Copy Elision 會發生，就算建構子與解構子有 side effect 也一

那麼我們就先來談談什麼是 Copy Elision 吧，如果大家想跟著測試 code，記得把 Copy Elision 關掉，指令是 -fno-elide-constructors (gcc的，其他編譯器我不清楚一不一樣)，然後版本記得在 C\+\+17 與 C\+\+14 間來回切換看一下差別，因為如果是在 C\+\+17 是「保證」會發生 Copy Elision 的狀況，它的 Copy Elision 是無法被關掉的，因為「保證」要發生，必須發生

例子我會附上 godbolt 的連結，避免大家無法重現環境，有興趣的可以點進連結看看

基本上 Copy Elision 的精神就是在條件允許下，直到必須建構前，都不能建構，在初始化物件時能直接以傳入的原料(Value) 來初始化就直接初始化，不要在那邊搞一堆複製或移動。另外，在下方，當我講 Copy Elision 的時候指的就是 Copy / Move Elision 了，因為習慣所以仍只講 Copy Elision，若是特殊情形我會特別標註，但應該是沒有啦

另外，Copy Elision 是現在唯二能改變外顯行為的優化，另一個是 [Allocate Elision](https://en.cppreference.com/w/cpp/language/new#Allocation)，可以當作小知識知道一下 XD

### 什麼是 Copy Elision?

Copy Elision 就是字面上的意思，能夠省略複製與移動建構子，直接看例子會比較好懂，那麼接下來我會用同一份 Code，看看在不同環境與設定下有什麼差別

Code：

```cpp
#include <iostream>

static int cnt{};

struct S {
    S() {
        std::cout << R"(S()
        Construct object at )"
                  << this << '\n' << '\n';
        ++cnt;
    }

    S( const S &other ) {
        std::cout << R"(S(const S& other)
        Construct object at )"
                  << this << '\n' << '\n';
        ++cnt;
    }

    S( S &&other ) {
        std::cout << R"(S(S&& other)
        Construct object at )"
                  << this << '\n' << '\n';
        ++cnt;
    }

    S& operator=(const S &other) {
        std::cout << R"(S& operator=(const S& other)
        Construct object at )"
                  << this << '\n' << '\n';
        ++cnt;
        return *this;
    }

    S& operator=(S &&other) {
        std::cout << R"(S& operator=(S&& other)
        Construct object at )"
                  << this << '\n' << '\n';
        ++cnt;
        return *this;
    }

    ~S() {
        std::cout << R"(~S()
        Destructed the object at )"
                  << this << '\n' << '\n';
    }
};

S func() {
    S B;
    return B;
}

int main() {
    S A = func();
    std::cout << "\nthe address of A in main is " << &A << '\n'
              << '\n';

    std::cout << "cnt = " << cnt << '\n' << '\n';
    return 0;
}
```

因為他有點長，看起來可能有點可怕，所以我解釋一下 Code

我實作了三種建構子：`S()`、`S( const S &other )`、`S( S &&other )`，和兩種運算子 `=` 的重載：`S& operator=(const S &other)` 和 `S& operator=(S &&other)`

每個建構子裡面做的事情都一樣，把呼叫的建構子的名稱，也就是現在的建構子名稱給印出來，然後把 instance 的位址印出來，之後將 `cnt` 加一。解構子也一樣，將正在解構的實例的位址印出來，然後 `cnt` 加一

`main` 內我們定義了一個 `A`，型態是 `S`，用 `func` 回傳的物件來建構。 `func` 會實例化一個 `B`，型態也是 `S`，然後回傳，所以 `func()` 是一個 Prvalue Expression，可以回去前面查表看看

最後我們把 `A` 的位址印出來，然後把 `cnt` 印出來，看看總共呼叫了幾次建構子

現在知道 Code 在幹嘛了，那就來看輸出吧!

#### 環境 1： C\+\+14，關掉 Copy Elsion ( <strong>-std=c++14 -fno-elide-constructors</strong> )

[測試連結](https://godbolt.org/z/ejvWaWrEE)，輸出如下：
```cpp
S()
        Construct object at 0x7ffd48647a9f

S(S &&other)
        Construct object at 0x7ffd48647acf

~S()
        Destructed the object at 0x7ffd48647a9f

S(S &&other)
        Construct object at 0x7ffd48647ace

~S()
        Destructed the object at 0x7ffd48647acf
the address of A in main is 0x7ffd48647ace

cnt = 3

~S()
        Destructed the object at 0x7ffd48647ace
```

可以看見呼叫了 3 次建構子。那麼我們接著來看下一個環境

#### 環境 2：C\+\+17，關掉 Copy Elsion ( <strong>-std=c++17 -fno-elide-constructors</strong> )

[測試連結](https://godbolt.org/z/sv75ErEKG)，輸出如下：

```cpp
S()
        Construct object at 0x7fff9d04743f

S(S &&other)
        Construct object at 0x7fff9d04746f

~S()
        Destructed the object at 0x7fff9d04743f
the address of A in main is 0x7fff9d04746f

cnt = 2

~S()
        Destructed the object at 0x7fff9d04746f
```

呼叫了 2 次建構，太厲害了，少了一次! 看來 C\+\+17 說保證會發生並不是唬爛的，那我們把 Copy Elision 打開看看

#### 環境 3：C\+\+17，打開 Copy Elsion ( <strong>-std=c++17 </strong> )

[測試連結](https://godbolt.org/z/TYo3e5dqq)，輸出如下：

```cpp
S()
        Construct object at 0x7ffc79411f7f
the address of A in main is 0x7ffc79411f7f

cnt = 1

~S()
        Destructed the object at 0x7ffc79411f7f
```

只呼叫了 1 次建構，太神啦，又少了一次

#### <strong>小整理</strong>

我們來統整一下這三個例子，用個表格來看：

|                      |   C++14  | C++17    |
| --------             | -------- | -------- |
| 關掉 Copy Elision     | 3 次     | 2 次     |
| 打開 Copy Elision     | ?        | 1 次     |

這樣是關掉了 Copy Elision，從 C\+\+14 換到 C\+\+17 時少呼叫了一次建構子，這證明了 C\+\+17 「保證」了部分 Copy Elision 的發生，而同樣是 C\+\+17，關掉與打開 Copy Elision，也少呼叫了一次建構子，這則證明了 C\+\+17 只保證「部分」的 Copy Elision 會發生，其餘的部分則看 Compiler 如何處理，標準也有將這些狀況列下來，等等會談到

那麼四格中還有一格沒有檢查到，那就是在 C\+\+14 時打開 Copy Elision 的狀況，所以我們現在來看看這個狀況下的輸出 [(連結)](https://godbolt.org/z/hGxjPYbrf)：

```cpp
S()
        Construct object at 0x7ffca23c2e6f
the address of A in main is 0x7ffca23c2e6f

cnt = 1

~S()
        Destructed the object at 0x7ffca23c2e6f
```

一樣只有一次! 所以表格變成了這樣：

|                      |   C++14  | C++17    |
| --------             | -------- | -------- |
| 關掉 Copy Elision     | 3 次     | 2 次     |
| 打開 Copy Elision     | 1 次     | 1 次     |

可以看見 Compiler 一直以來其實都默默地在幫我們做事，只是妳可能都沒發現 XD

### 保證會發生 Copy / Move Elision 的狀況

那我們現在就來看看這神奇的東西什麼時候會發生吧！首先是「保證」會發生的情況 [(來源)](https://eel.is/c++draft/class.copy.elision)，在這些情況下，Compiler 必須省略複製與移動建構子的呼叫，即使它們有 side effect 也一：

+ 在一個回傳 `T` 型態的[回傳陳述句](https://en.cppreference.com/w/cpp/language/return)，像是函式，如果回傳的是一個相同型態 `T` 的 Prvalue Expression (無視 cv 限定詞)，那麼他必須發動：

    環境：C\+\+17，關掉 Copy Elision (-std=c\+\+17 -fno-elide-constructors)，[測試連結](https://godbolt.org/z/d9c86xfTb)

    ```cpp
	#include <iostream>

	struct T {
	    T() {
	        puts("T()");
	    }
	    T(T &&other) {
	        puts("T(T &&other)");
	    }
	    T(const T &other) {
	        puts("T(const T &other)");
	    }
	    ~T() {
	        puts("~T()");
	    }
	};

	T f() {
	    return T();
	}

	int main() {
	    f();    // 只會 call 一次建構子
        T x = f();    // 只會 call 一次建構子
	}
    ```

    輸出：
    ```cpp
    T()
    ~T()
    ```

    可以看見 `f()` 和 `T x = f();`，都被省略到只呼叫了一次建構子，調回 C\+\+14 的話會輸出：

    ```cpp
    T()
    T(T &&other)
    ~T()
    ~T()
    ```

    要注意的是他不一定需要移動建構子與複製建構子，因為我們沒用到他(看起來很像廢話)，妳可以把他 `delete` 掉試試看，但要有預設的建構子和解構子，不然他沒辦法建構解構 (因為會用到)
+   再來是一個大重點，在初始化物件時，如果初始化器(initializer)內是一個相同型態的 Prvalue Expression (無視 cv 限定詞)，那麼 Copy Elision 也保證發生，而且他必須直接初始化那個物件，不能複製和移動

    那我們就來看看例子：

    環境：C\+\+17，關掉 Copy Elision (-std=c\+\+17 -fno-elide-constructors)，[連結](https://godbolt.org/z/zY35f9W9z)

    ```cpp
    #include <iostream>

	struct T {
	    T() {
	        puts( "T()" );
	    }
	    T( T &&other ) {
	        puts( "T(T &&other)" );
	    }
	    T( const T &other ) {
	        puts( "T(const T &other)" );
	    }
	    ~T() {
	        puts( "~T()" );
	    }
	};

	T f() {
	    return T();
	}

	int main() {
	    T x = f();    //只呼叫一次建構子
	    T y = T( T( f() ) );    //只呼叫一次建構子
	    T *z = new T( T( f() ) );    //只呼叫一次建構子
	    delete z;
	}
    ```

    輸出：
    ```cpp
    T()
	T()
	T()
	~T()
	~T()
	~T()
    ```

    `T x = f();` 直接被初始化，和 `T x;` 等價，而 `T y;` 和 `T y = T( T( f() ) );` 等價。所以你可能會聽見有些人說「Copy Elision 根本沒省略東西」，指的就是這個情況，因為直接初始化了，某種層面上根本沒有複製可以讓我們來初始化，但只是一個說法，可以不用太在意他XD

    而 Prvalue Expression 的定義也因為這項規定而改變了，還記得「回傳非參考類型的運算子重載運算式或函式呼叫(function call)」被歸類在 Prvalue Expression 嗎? 以上方的例子來說就是 `f()`

    看到這裡妳應該覺得很合理了，因為「沒有暫時物件」了，就好像常量一樣，也因此，我們才會說 C\+\+17 後 Prvalue Expression 不可被移，因為根本沒有暫時物件生出來給我們移動，並且依然沒有身分標識符 (identity)，原因一樣，因為沒有物件產生

    要注意的是被初始化的物件不能是[潛在重疊的物件](https://eel.is/c++draft/intro.object#7)，潛在重疊的物件包含：

    + 子類物件 (a base class subobject)
    + 一個有 `no_unique_address` attr 的非靜態的資料成員 (from C++20)

    下面這個大家應該沒什麼看過，我也是第一次看到 XD，但草案內也只是提到而已，沒特別去說明，看看這個[例子](https://godbolt.org/z/8d5ercfYM)：

    ```cpp
    #include <iostream>

	struct T {
	    T() {
	        puts( "T()" );
	    }
	    T( T &&other ) {
	        puts( "T(T &&other)" );
	    }
	    T( const T &other ) {
	        puts( "T(const T &other)" );
	    }
	    ~T() {
	        puts( "~T()" );
	    }
	};

	T f();

	struct D;

	D g();

	struct D : T {
	    D() : T( f() ) {
	        puts( "D()" );
	    }

	    D( int ) : D( g() ) {
	        puts( "D(int) : D(g())" );
	    }
	};

	T f() {
	    return {};
	}

	D g() {
	    return {};
	}

	int main() {
	    D x;
	    D y = g();
	    D z{ 5 };
	}
    ```

    輸出：
    ```cpp
    T()
	T(T &&other)
	~T()
	D()
	T()
	T(T &&other)
	~T()
	D()
	T()
	T(T &&other)
	~T()
	D()
	D(int) : D(g())
	~T()
	~T()
	~T()
    ```

    超亂 XD，而且就算把 Copy Elision 開起來也一樣，所以就自己在寫的時候注意一下囉！

### 不保證發生 Copy Elision 的狀況

那這邊我就大概列一列，翻譯一下 Cppreference 就好 [(來源)](https://en.cppreference.com/w/cpp/language/copy_elision#Non-mandatory_elision_of_copy.2Fmove_.28since_C.2B.2B11.29_operations)，因為我覺得最重要的是 NRVO，但 NRVO 後面會單獨拉出來講，所以就先不自己寫例子或找例子了 XD，~~不然好多怕字不夠，寫到這裡的時候，把網址那些的字元都算進來，已經六萬五千字ㄌ~~。但如果大家願意幫我補例子當然是歡迎XD 我對例外處理也不太熟，不敢亂誤人子弟。 不過草案裡面倒是有給初一些例子，有興趣的話可以點進[這裡](https://eel.is/c++draft/class.copy.elision)看看

那麼項目大概有這些，這個很看 Compiler，然後我不確定官方是不是把所有有可能的狀況都列下來了，但我想不太可能，畢竟 C\+\+ 挺複雜也挺自由的，我主要在讀的也只是 draft 和 Cpprefence，字密密麻麻的我也有可能漏看，不過我是沒特別看到她說這是全部的狀況啦，如果有人知道可以幫我補充一下，感恩！

+ 命名回傳值優化 (NRVO)，這等後面談回傳值優化的時候再一起講吧 XD
+ 某些條件下的 throw 表達式

    在 [throw 表達式](https://en.cppreference.com/w/cpp/language/throw)中，如果妳的運算元是一個 non-volatile 的物件，而且這個物件不是函式的參數或 [catch clause](https://en.cppreference.com/w/cpp/language/try_catch) (不會翻QQ) 的參數，然後它的 scope 範圍沒有超過最裡面的 try-block (如果裡面有 try-block 的話)，然後！ 運算元有[自動的儲存期限](https://en.cppreference.com/w/cpp/language/storage_duration#Storage_duration)，那他可能發生 Copy Elision

    「scope 範圍沒有超過最裡面的 try-block (如果裡面有 try-block 的話)」這句我翻得不太確定，因為我自己不常用例外處理，相對沒那麼熟，他的原文是「whose scope does not extend past the innermost try-block (if there is a try-block).」，如果有翻錯還請幫忙糾正一下，謝謝！ <img src = "https://cdn.discordapp.com/emojis/784176641428553777.gif?v=1" height = 30>

+ 某些狀況下的 catch clause

    在 [catch clause](https://en.cppreference.com/w/cpp/language/try_catch) 中，如果它的參數和 `exception` 丟出來的物件型態一樣(無視 cv 限定詞)，那麼這個例外物件的複製可以被省略，然後 catch clause 的 body 會直接訪問例外物件，就像是 catch by reference 一樣，但不能移動例外物件，因為例外物件永遠是 Lvalue Expression 回傳的物件

    如果這類的 Copy Elision 會因為任何原因 ( 除了跳過 catch clause 的參數的複製建構子和解構子 )，導致程式的外顯行為改變，舉個例子，如果妳 catch clause 的參數已經被修改了，但例外對像被重新丟出，像是這樣的某種原因，它會更改到這個程式的外顯行為，那麼就會禁止這個 Copy Elision 

+ 協同程式 (Coroutines)

    在[協同程式](https://eel.is/c++draft/support.coroutine)中，除了參數的建構子和解構子，於 coroutine state 的參數的移動或複製可能會被省略，因為這不會改變程式的外顯行為，換句話說，如果妳在斷點後從沒引用過這個參數，或者妳在整個 coroutine state ，從一開始就沒有 heap-allocate，那麼 Copy Elision 可能會發生

    這段我也翻得不太確定，我也只稍微看過 Coroutines 而已，沒有實際用過，但大概知道他想講什麼，我把原文也附上，如果我有錯還請大家糾正一下，謝謝！

    原文：

    In coroutines, copy/move of the parameter into coroutine state may be elided where this does not change the behavior of the program other than by omitting the calls to the parameter's constructor and destructor. This can take place if the parameter is never referenced after a suspension point or when the entire coroutine state was never heap-allocated in the first place.

### Copy / Move Elision 小整理

恭喜妳看到這裡了，但後面還很長，這裡先幫大家整理一下，並且分享一個好用的小東西

複製省略「保證」會發生在一個回傳同型態 Prvalue Expression 的[回傳語句](https://en.cppreference.com/w/cpp/language/return)中，通常舉例是舉函式，又或一個回傳同類型的 Prvalue Expression

另外，一開始在介紹什麼是 Copy Elision 時，給大家看了一份很長的 code 然後在不同環境下去跑他對吧? 但不知道大家有沒注意到有時候是呼叫 Rvalue Reference 版本的建構子，有時候是預設的建構子

這邊有一個要注意的地方是我們也有實作 const Lvalue Reference 的版本，但他傳入的是 Rvalue Reference 的版本，這代表什麼? 代表有「暫時物件」的產生，在什麼時候?! 在 `return` 的時候，這邊我給了一個簡單的 code 讓大家看，[測試連結](https://godbolt.org/z/To7198TGa)：

環境：C\+\+14，關掉 Copy Elision

```cpp
#include <iostream>

struct Test {
    int i;
    Test() : i{} {
    }
    Test( const Test &other ) : i{} {
    }
    ~Test() {
    }
};

Test test_fn() {
    Test B;
    return B;    // 回傳的這個我們先叫他 X
}

int main() {
    Test A = test_fn();
    return 0;
}
```

在 `test_fn()` 內我回傳了一個 Lvalue Expression，這是因為在這個例子內，他和回傳 `Test{}` 的結果會是一樣，為了方便大家看，我就把 `B` 給打出來了，在這個例子，他等價於：
```cpp
Test test_fn() {
    return Test{};
}
```

注意只是這個例子剛好一樣，情況一複雜起來，他不一定會一樣。 

為了方便大家理解，附上一張手寫圖：

<center>

<img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Value_Categories/image/image6.png?raw=true">

</center><br>

今天熟組語的朋友真是有福了，每個例子旁邊都配了組語讓你看 XD，但不會組語的朋友不用擔心，我照順序翻譯成中文敘述給你們看，`test_fn()` 回傳的物件我們暫時叫他 `X`，妳可能會想說他不是 `return B` 嗎? 怎麼會多一個 `X`? 欸沒錯，它們其實不一樣，想不到吧ㄏㄏ，如果大家想跟著確認，可以在建構子裡面，像是一開始的例子那樣把正在呼叫的建構子印出來看：

```cpp
1. Test A = 「test_fn()」; 右邊的部分先執行
> 記憶體裡面挖一塊空間，拿來放等等 test_fn() 回傳的物件，位址就叫 "&X"，代表在 Main 裡面 X 的位址，準備呼叫 test_fn()，將 &X 丟到暫存器裡面

1. 進入 test_fn()， &X 跟著傳入

2. Test B;
> 記憶體裡面挖一塊空間，拿來放 B，位址我們叫他 "&B"，準備呼叫 B 的建構子，將 &B 丟到暫存器裡面
> 進入 B 的建構子，進入的建構子是預設建構子 Test()，&B 跟著傳入
> B 建構完成，i 被初始化為 0

1. return B;
> 準備呼叫 X 的建構子，也就是準備呼叫回傳物件的建構子，將 &B 和 &X 丟到暫存器裡
> 進入 X 的建構子，進入的建構子是 Test( const Test &other )，&B 和 &X 都被傳入
> 會進入 Test( const Test &other ) 的原因是因為我們傳了 B 進去

1. 離開 test_fn()
> B 解構，解構完後要離開函式，回傳 &X，所以剛剛挖好的空間裡面有建構好的 X 了

1. Test A = test_fn(); 右邊執行完了，換初始化
> 記憶體裡面挖一塊空間，拿來放A，位址就叫 "&A"
> 準備呼叫 A 的建構子，將 &X 和 &A 丟到暫存器裡面
> 進入 A 的建構子，進入的建構子是 Test( const Test &other )，&X 和 &A 都被傳入
> 會進入 Test( const Test &other ) 的原因是因為我們傳了 X 進去
> 離開 A 的建構子

1. Test A = test_fn(); 此 Statement 執行完畢
> X 的生命週期結束
> X 解構

題外話：這邊其實我挺好奇 mov ebx, 0 是什麼意思的，有人知道的話可以告訴我一下XD 是在 padding 之類的嗎？

8. 準備離開 main 函式，程式要結束了
> A 解構
> 做離開函式的處理，return 0 是其中一項
```

我省略了很多繁瑣的過程，但大致上就長這樣

如果你有自己在建構子和解構子裡面加上輸出，會看到輸出長這樣，然後我幫大家加一下註解：

```cpp
Test()    // B 建構
Test( const Test &other )    // X建構
~Test()    // B 解構
Test( const Test &other )    // A 建構
~Test()    // X 解構
~Test()    // A 解構
```

如果有比較敏銳的朋友可能會發現這是我們說 Prvalue Expression 沒有身分標識符(identity) 的原因，因為事實上有兩個不同的物件，就算在建構子裡面輸出了 `this` (B 的位址)，與 Rvalue Reference 輸出的位址也不同個 (一個是 &X，一個是 &B)，當然在我們不應該用這個方式來思考文法的問題，只是幫助大家理解而已 XD，那麼 C\+\+17 後的 Prvalue 又是另一件事，這剛剛解釋過了

另外可能有人會發現如果有實作 Rvalue Reference 的版本，那麼他會優先呼叫這個版本的建構子，原因牽涉到隱式移動(implicit move)，後面再講 XD，這邊只是為了讓大家知道建構解構的詳細過程而已

另外讀到這裡大家已經知道了由於 Copy Elision 的關係，Prvalue Expression 直到必須被實例化時才會有實例的出現，所以分享一個方便的偵測器給大家，可以幫你判斷值類別，這是我問 kris 問題時，他給我的例子裡面用到的，出處在[這裡](https://medium.com/@barryrevzin/value-categories-in-c-17-f56ae54bccbe)：

```cpp
#include <iostream>
#include <utility>

// From: https://medium.com/@barryrevzin/value-categories-in-c-17-f56ae54bccbe
namespace detail {
  template <class T> struct value_category      {
    static constexpr char const * value = "prvalue"; };

  template <class T> struct value_category<T&>  {
    static constexpr char const * value = "lvalue";  };

  template <class T> struct value_category<T&&> {
    static constexpr char const * value = "xvalue";  };
}

#define PRINT_VALUE_CAT(expr)    \
  std::cout << #expr << " is a " \
            << ::detail::value_category<decltype((expr))>::value << '\n'

int main() {
  struct A {
    int value = 42;
  };

  PRINT_VALUE_CAT([]{});

  // The next two lines are practically identical:
  PRINT_VALUE_CAT(A{}.value);
  PRINT_VALUE_CAT([]{ return A{}; }().value);
}
```

輸出：
```cpp
[]{} is a prvalue
A{}.value is a xvalue
[]{ return A{}; }().value is a xvalue
```

超方便的呀！有了這個誰還在跟你慢慢判斷阿，~~大人，時代變了呀~~，上方名稱空間裡面的內容就是用來判斷值類別的主體，而下方的巨集函式則幫助我們輸出

由於值類別會影響呼叫的順序，幫大家複習一下，Prvalue 優先傳入吃 non-reference type 參數的函式，Lvalue 優先傳入吃 lvalue reference 參數的函式，Xvalue 優先傳入吃 Rvalue Reference 參數的函式。那麼因為模板參數的優先度也一樣，於是，我們就可以利用這個特性來判斷值類別了

會到這邊才講是因為知道 Copy Elision 後會比較好理解她在幹嘛，雖然原因可能有點不一樣就是了，這邊利用了 `decltype` 來幫忙，但不是今天的主題，就先不講太多了，之後有時間再寫一篇有關 `decltype` 和 `auto` 的文章 (亂開坑)

## 臨時物化 (TMC)

好了現在我們完全了解 Copy Elision 了，那還有另一個重要的東西叫 TMC，TMC 是 [Temporary materialization conversion](https://en.cppreference.com/w/cpp/language/implicit_conversion#Temporary_materialization) 的縮寫，前面講 Rvalue Reference 時有先提到，那我們這邊再更詳細的探討一下，因為標準實際上有規定 C\+\+17 後的哪些時刻「必須」要有 TMC 的出現

基本上 TMC 的精神就是在必要的時候，一個完全型(Complete Type) 的 Prvalue 能夠被轉換成同型態(Type) 的 Xvalue。TMC 會透過計算評估相關的 Prvalue Expression 來初始化一個相同型態的暫時物件當作它的結果，也就是說 TMC 是一個 Prvalue to Xvalue 的橋樑。前面的 Copy Elision 負責減少實例，而 TMC 則幫忙於必須要有實例的時候建構實例，看看這個[例子](https://godbolt.org/z/hbexrMKTc)，如果你想複習 Copy Elision，也可以把版本改回 C\+\+14 去看看：

```cpp
#include <iostream>

int s_cnt{};

struct S {
    int m;
    S() : m{} {
        puts( "S()" );
        ++s_cnt;
    }
    S( const S &other ) : m{ 2 } {
        puts( "S(const S &other)" );
        ++s_cnt;
    }
    ~S() {
        puts( "~S()" );
    }
};

S fn() {
    return S{};
}

int main() {
    int i = S( S( S( S( S( S( S( S( S( S( S( S( S( fn() ) ) ) ) ) ) ) ) ) ) ) ) ).m;
    std::cout << "i = " << i << " s_cnt = " << s_cnt;

    int &&rr = 5;
}
```

這是經典的 TMC 例子，main 裡面那一大串會因為 Copy Elision 的關係被簡化成 `S().m`，這時候 TMC 發現 `S()` 是 Prvalue Expression，而我們訪問了一個 Prvalue 內的成員 `i` 裡面的 `m`，所以此時的 `S` 必須被建構！於是 TMC 呼叫 `S()` 建構暫時匿名物件，也因此 `S().c` 是一個 Xvalue。而 Rvalue Reference 連結到常量的情況我們前面解釋過了，後面我會再給一個例子配上組語，忘記的朋友可以回去複習一下或看後面的例子。 

所以從這裡你可以感覺到 TMC 主要會在我們

+ 訪問一個 Prvalue 的成員時發生
+ 讓一個 Reference 與 Prvalue 連結的時候發生

當然不只這兩個，但這兩個是最常碰到的

那麼這邊我就把詳細的情況列出來，在下面這些情況時 TMC 「保證」會發動：

+ 將一個 Reference 與 Prvalue 連結

    前面談 Reference 的時候已經對這個狀況解釋了很多，所以我們這邊再看一個例子就帶過：
    ```cpp
    int main() {
        int a{};

        ++a;    // Lvalue Expression
        a = 0;

        int &r = ++a;    // Lvalue Reference Bind to Lvalue Expression
        a = 0;

        a++;    // Rvalue Expression
        a = 0;

        int &&rr = a++;
        a = 0;
    }
    ```
    附上我精美的手寫圖：

    <center>
    
    <img src = "https://github.com/Mes0903/Cpp-Miner/blob/standard-markdown/Miner_main/Value_Categories/image/image7.png?raw=true">
    
    </center><br>

    可以看見有暫時物件的產生。我在每一個操作結束時都加上 `a = 0;`，並用粉色的螢光筆標起來了，當作區間的紀錄點，方便大家閱讀

+ 拜訪或使用了 Class Prvalue 的成員時：

    ```cpp
	#include <iostream>

	int main() {
	    class C {
	      public:
	        int i;
	        C() {
	            puts("C()");
	        }
	        C(const C& other) {
	            puts("C(const C& other)");
	        }
	        ~C() {
	            puts("~C()");
	        }
	    };

	    C();    // Prvalue
	    C().i;    // Xvalue，TMC 產生暫時物件
	}
    ```

    `C()` 是個 Prvalue，這大家應該不陌生，前面的例子也解釋過了，當我們拜訪她的成員時，TMC 會產生一個暫時物件，這也是我們說 `C().i` 是 Xvalue 的原因

+ 拜訪或使用了 Array Prvalue 的成員時，或 Array Prvalue 退化成指標時：
    ```cpp
    #include <iostream>

	void f( int( &&x )[2][3] ) {
	}

	template <typename T>
	using identity = T;

	int main() {
	    using arr_t = int[2][3];

        //拜訪、使用了成員
	    for ( const auto &x : arr_t{ { 1, 2, 3 }, { 4, 5, 6 } } ) {
	    }
	    for ( const auto &x : identity<int[2][3]>{ { 1, 2, 3 }, { 4, 5, 6 } } ) {
	    }

	    f( arr_t{ { 1, 2, 3 }, { 4, 5, 6 } } );    //退化成指標
	    f( identity<int[2][3]>{ { 1, 2, 3 }, { 4, 5, 6 } } );    //退化成指標
	}
    ```

+ 利用 [braced-init-list](https://en.cppreference.com/w/cpp/language/list_initialization) 來初始化一個型態為 `std::initializer_list<T>` 的物件時

+ 對 Prvalue 使用 `typeid` 時 (這是一部分的[無運算表達式](https://en.cppreference.com/w/cpp/language/expressions#Unevaluated_expressions))

+ 對 Prvalue 使用 `sizeof` 時 (這是一部分的[無運算表達式](https://en.cppreference.com/w/cpp/language/expressions#Unevaluated_expressions))

+ 當一個 Prvalue 作為 [棄值表達式](https://en.cppreference.com/w/cpp/language/expressions#Discarded-value_expressions) 出現時

要注意的是當我們拿 Prvalue 來初始化物件時，TMC 並不會發動，因為有 Copy ELision 的存在，保證了直接初始化

那我們來看個小總結：

```cpp
#include <iostream>

int c_cnt{}, s_cnt{};

struct C {
    int m;
    C() : m{} {
        puts( "C()" );
        ++c_cnt;
    }
    C( const C &other ) : m{ 100 } {
        puts( "C( const C &other )" );
        ++c_cnt;
    }
    ~C() {
        puts( "~C()" );
    }
};

struct S {
    C c;
    S() {
        puts( "S()" );
        ++s_cnt;
    }
    S( const S &other ) {
        puts( "S(const S &other)" );
        ++s_cnt;
    }
    ~S() {
        puts( "~S()" );
    }
};

S fn() {
    return S{};
}

int main() {
    int i = S( S( S( S( S( S( S( S( S( S( S( S( S( fn() ) ) ) ) ) ) ) ) ) ) ) ) ).c.m;
    std::cout << "i = " << i << " c_cnt = " << c_cnt << " s_cnt = " << s_cnt;
}
```

這個例子混合了 Copy Elision 和 TMC 的應用，大家可以先自己判斷看看
main 裡面那一大串會因為 Copy Elision 的關係被簡化成 `S().c.m`，這時候 Compiler 發現 `S()` 是 Prvalue Expression，而我們訪問了一個 Prvalue 的成員 `c` 裡面的 `m`，所以此時的 `c` 必須被建構！但是 `S` 還沒被實例化，所以 `S` 會先被實例化，然後 `c` 在被建構

所以從定義上來看， `S().c` 是一個 Xvalue，然後我們利用 TMC 評估 `S` 內的 `m`，返回一個相同型態(`int`)的暫時物件 `S().c.m`。 
## 回傳值優化 Return value optimization (RVO)

好了，標準保證會發生的東西都談的差不多了，那我們來說說不一定會發生的東西

RVO 的全名叫做 Return Value Optimization，字面上的意思，他是對回傳值進行的優化，主要的精神就是消除函式回傳的臨時對

那麼 RVO 分成兩種，URVO 和 NRVO，兩者的差別是函式內回傳的物件有沒有 identifie，換句話說就是「是不是一個匿名物件」。那麼我們就來看看吧！

### 匿名回傳值優化 (URVO)

URVO 全名叫 Unnamed Return Value Optimization。 當我們用一個回傳同型態的匿名物件的回傳表達式來初始化一個物件時，URVO 會發動來省略複製

等等，這句話有點眼熟，不就是 Copy Elision 嗎? 沒錯，所以在 C\+\+17 後，URVO 被保證了，也就是 Copy Elision 的其中一部份

### 有名回傳值優化 (NRVO)

NRVO 全名叫 Named Return Value Optimization ，那麼 NRVO 就是比較重點的地方了

在一個回傳語句中，當我們回傳一個與語句回傳型態同型態(無視 cv 限定詞)，自動儲存週期的有名物件，簡單來說，函式回傳 `int` 而我們也真的 `return` 一個 `int`，而且這個回傳的物件不是被吃進來的參數或 catch clause 的參數，那麼 NRVO 可會發動來省略複製

NRVO 有沒有發動實際上還是看 Compiler 與 ABI，標準只講了哪些時刻不能發動，所以前面講 Copy Elision 時我們才會說 NRVO 不保證會發

來看個最簡單的例子，版本選 C\+\+17，打開回傳值優化：

```cpp
#include <iostream>

struct T {
    T() {
        puts( "T()" );
    }
    T( T &&other ) {
        puts( "T(T &&other)" );
    }
    T( const T &other ) {
        puts( "T(const T &other)" );
    }
    ~T() {
        puts( "~T()" );
    }
};

T f() {
    T result;
    return result;
}

int main() {
    T x = f();

    return 0;
}
```

輸出：
```cpp
T()
~T()
```

可以看見和 Copy Elision 的狀況很像，複製都不見了，差別是我們前面在講 Copy Elision 時，在 `f()` 裡面回傳的是一個匿名物件，而這邊是一個有名字的物件

那我們可以稍微讓他變得複雜一點，加個分支進去：

```cpp
#include <iostream>

struct T {
    T() {
        puts( "T()" );
    }
    T( T &&other ) {
        puts( "T(T &&other)" );
    }
    T( const T &other ) {
        puts( "T(const T &other)" );
    }
    ~T() {
        puts( "~T()" );
    }
};

T f(bool condition) {
    T result;

    if(condition)
        return result;

    return result;
}

int main() {
    T x = f(true);
    T y = f(false);

    return 0;
}
```

輸出：
```cpp
T()
T()
~T()
~T()
```

可以看見一樣會發動，但是！NRVO 是非常脆弱的，很容易就不會發動，我們再改一下上面這個[例子](https://godbolt.org/z/q658Ge1TY)：
```cpp
#include <iostream>

struct T {
    T() {
        puts( "T()" );
    }
    T( T &&other ) {
        puts( "T(T &&other)" );
    }
    T( const T &other ) {
        puts( "T(const T &other)" );
    }
    ~T() {
        puts( "~T()" );
    }
};

T f(bool condition) {
    T result;

    if(condition)
        return T{};

    return result;
}

int main() {
    T x = f(true);
    T y = f(false);

    return 0;
}
```

輸出：

```cpp
T()
T()
~T()
T()
T(T &&other)
~T()
~T()
~T()
```

我並沒有刻意將優化關掉，環境一樣是 C\+\+17，但我們可以看見優化消失了，多了很多額外的建構出來

那麼前面還有提到 NRVO 有一些條件，在這邊我也把它們寫出來給大家看看，所以下面這兩項保證不會發動。 (感謝 kris 提供例子)

+ [(連結)](https://godbolt.org/z/fbMe66asq) 發生物件切片(object slicing) 時：

	```cpp
	#include <iostream>

	struct T {
	    T() {
	        puts( "T()" );
	    }
	    T( T &&other ) {
	        puts( "T(T &&other)" );
	    }
	    T( const T &other ) {
	        puts( "T(const T &other)" );
	    }
	    ~T() {
	        puts( "~T()" );
	    }
	};

	struct U : T {
	    U() {
	        puts( "U()" );
	    }
	    U( U &&other ) {
	        puts( "U(U &&other)" );
	    }
	    U( const T &other ) {
	        puts( "U(const U &other)" );
	    }
	    ~U() {
	        puts( "~U()" );
	    }
	};
	T f() {
	    U result;
	    return result;
	}

	int main() {
	    T x = f();

	    return 0;
	}
	```

    輸出：

    ```cpp
    T()
	U()
	T(const T &other)
	~U()
	~T()
	~T()
    ```

    物件切片(object slicing) 的意思就是你拿一個子類去喂父類，那麼有一些資訊就會被切掉，有興趣可以到[這裡](https://stackoverflow.com/questions/274626/what-is-object-slicing)看看

    我們可以看見在上例中 NRVO 一樣沒有發動，因為回傳物件的型態與函式回傳的型態不一樣

+ [(連結)](https://godbolt.org/z/fMEKa48GW) 回傳的物件為函式吃進來的參數時：

    ```cpp
    #include <iostream>

	struct T {
	    T() {
	        puts( "T()" );
	    }
	    T( T &&other ) {
	        puts( "T(T &&other)" );
	    }
	    T( const T &other ) {
	        puts( "T(const T &other)" );
	    }
	    ~T() {
	        puts( "~T()" );
	    }
	};
	T f(T arg) {
	    return arg;
	}

	int main() {
	    T x = f(T{});

	    return 0;
	}
    ```

    輸出：

    ```cpp
    T()
    T(T &&other)
    ~T()
    ~T()
    ```

    可以看見 NRVO 一樣沒有發動
## 隱式移動

講了這麼多，那如果 Copy Elision 和 NRVO 都沒有發動的話怎麼辦呢？不用擔心，標準告訴我們，在 C\+\+11 之後，於回傳語句中，如果在某些特殊情況下，Copy Elision 應該要發動但沒發動，或是感覺可以發動但還沒被保證，所以沒發動，像是我們在 C\+\+14 的版本內把回傳優化關掉的狀況。 這種時候 Compiler 會優先選用移動建構子來取代複製，除非回傳的物件是吃進來的參數

要注意的是即使回傳這個物件的表達式是 Lvalue Expression，他仍然會優先選用移動建構子，好像偷偷的轉型了一樣，詳細可以看[這裡](https://en.cppreference.com/w/cpp/language/return#Notes)

看到這裡，大家應該知道為什麼前面的例子中，如果有 `T(T &&other)` 這類的建構子時優先度會比 `T(const T &other)` 還高了，當然，你需要提供移動建構子就是了

## 結尾與一些小建議

好啦，這就是全部了，恭喜你讀完了！！

如果各位想要讀近代的 C\+\+，值類別是一個很重要的東西，你會在大量文獻中看到她，那麼這邊 kris 有給了幾個小建議，我在這邊會轉述給大家看，並補一些自己的意見：

+ 不用害怕以一個值來回傳物件

    如果你讓自己接受這件事，你可以很輕鬆的寫出很棒的函式，因為最終，函式回傳的終究是那些不是參數，在函式內定義出來的物件，如果你回傳了函式的參數的物件，那這會很怪，你大可以直接傳參考或指標進去改動他，又或在函式內複製一個出來回傳

    另外，不要回傳對區域物件的參考或指，一個函式執行完畢後裡面的區域物件會被銷毀，回傳它們的參考或指標是一件很奇怪的事，除非他們在這個函式結束後不會被解構，不然不要這樣做。如果你讀過 C\+\+Primer，你可能會看過類似的提醒，裡面有警告過一樣的事

    如果你在函式中大量的處理了這些管理權的問題，會導致你的可讀性變得很差，而且一沒處理好，你可能還會讓 Compiler 的優化失效

+ 讓 Compiler 幫你做多一點事

    現在妳知道了這些特性，不代表你一定要完美的處理這些複製和移動，像上面講的，如果你在函式中大量的處理了這些管理權的問題，會導致你的可讀性變得很差，而且一沒處理好，你可能還會讓 Compiler 的優化失效，先確保可讀性和可維護性再來講求效

+ 實作移動建構子和移動指派運算子

    在設計 Class 時，記得實作移動建構子(move constructor) 和移動指派運算子(move operator=)，具體可以參考 [Rule of five](https://en.cppreference.com/w/cpp/language/rule_of_three)

    因為如果 Copy Elision 和 NRVO 沒有發動，你還有隱式移動(implicit move，另外有移動建構子在某些處理上也比較方便

+ 盡量讓錯誤能在編譯期檢查出來，多使用編譯期運算

    這項可能每個人看法不太一樣，但這裡的原因是，普遍來說執行期的 code 很難被優化，錯誤也比較不好找，所以盡量讓錯誤和 code 在編譯期就可以被運算好

    有興趣的可以看看 [TMP](https://zh.wikipedia.org/wiki/%E6%A8%A1%E6%9D%BF%E8%B6%85%E7%B7%A8%E7%A8%8B)，也是個酷東西XD，TSJW 有寫過一篇[編譯期算完八皇后所有解](https://tjsw.medium.com/%E6%BD%AE-c-%E7%B7%A8%E8%AD%AF%E6%9C%9F%E7%AE%97%E5%85%AB%E7%9A%87%E5%90%8E%E6%89%80%E6%9C%89%E8%A7%A3-compile-time-8-queens-d872bc063bdc)的 Medium。但 TMP 也有它的缺點，平衡就看大家的需求自己取捨了

+ 讓你的函式短一點

    這會讓你和你的夥伴比較好維護你們寫的 code，然後可讀性也會增加，這是一大好處

    另一大好處是較短的函式能減輕 Compiler 的負擔，讓 Compiler 更好的利用前面提到的那些優化來優化你的 code

那麼本篇文章就到這裡啦，沒想到寫一寫，加上網址那些的竟然八萬多字了，一篇值類別就快比之前寫的 C\+\+ 基礎筆記還長了 

最後再次感謝 kris，marty，Cy 與 Discord 上一些不認識的人很友善且很有耐心的回答了我很多問題，尤其是 kris XD，非常有耐心的回答了我每一個問題，非常謝謝

## 參考資料

- [1. [CppDay20] Understanding value categories in C++ (Kris van Rens)](https://www.youtube.com/watch?v=km3Urog_wzk&ab_channel=ItalianCppCommunity") (文章部分來源，感謝 kris 提供授權)
- [2. Identifiers (cppreference)](https://en.cppreference.com/w/cpp/language/identifiers")
- [3. Value Category（值類別）- 1 / 2](https://eopxd.com/2021/02/03/value-category-1/")
- [4. Value Category（值類別）- 2 / 2](https://eopxd.com/2021/02/03/value-category-2/")
- [5. C++ Type and Value Category for Expression and Variable](https://stackoverflow.com/questions/36349360/c-type-and-value-category-for-expression-and-variable")
- [6. Lvalue to rvalue reference binding](https://stackoverflow.com/questions/20583531/lvalue-to-rvalue-reference-binding#comment-30793019")
- [7. Value categories (cppreference)](https://en.cppreference.com/w/cpp/language/value_category?fbclid=IwAR0MGKszwMzsKrkc-OHb5cHIUrF5hTWelt1xzqBrCoooXCGKhrsSkqEycWo") (文章部分來源)
- [8. What are rvalues, lvalues, xvalues, glvalues, and prvalues?](https://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues?fbclid=IwAR1BIns58XNgIy3KmXgFuc33KHkUC1mS10nUYvQec6BqxXK5BlUXM_f9UfE")
- [9. A Taxonomy of Expression Value Categories](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3055.pdf")
- [10. [C++] move semantic 的誤解](https://shininglionking.blogspot.com/2018/05/c-move-semantic.html")
- [11. [心得] Compound literals, flexible array members](https://www.ptt.cc/man/C_and_CPP/DB9B/DC33/M.1271034153.A.7F9.html")
- [12. How can a string literal be an lvalue?](https://www.reddit.com/r/cpp_questions/comments/8anfud/how_can_a_string_literal_be_an_lvalue/")
- [13. Why are string literals l-value while all other literals are r-value?](https://stackoverflow.com/questions/10004511/why-are-string-literals-l-value-while-all-other-literals-are-r-value")
- [14. compound literals](https://en.cppreference.com/w/c/language/compound_literal") (cppreference)
- [15. Statements](https://en.cppreference.com/w/cpp/language/statements") (cppreference)
- [16. Type](https://en.cppreference.com/w/cpp/language/type") (cppreference)
- [17. lvalues and rvalues in C++](https://www.youtube.com/watch?v=fbYknr-HPYE&ab_channel=TheCherno")
- [18. Move Semantics in C++](https://www.youtube.com/watch?v=ehMg6zvXuMY&ab_channel=TheCherno")
- [19. std::move and the Move Assignment Operator in C++](https://www.youtube.com/watch?v=OWNeCTd7yQE&ab_channel=TheCherno")
- [20. 值類別，以及其參考](https://docs.microsoft.com/zh-tw/windows/uwp/cpp-and-winrt-apis/cpp-value-categories?fbclid=IwAR0pCGputntm0KZ1AgDiysUB4m8HNjRQqgNUeCao8mhYLec9i3nFDZMst94")
- [21. identity VS identifier in c++](https://stackoverflow.com/questions/50785535/identity-vs-identifier-in-c)
- [22. Types of Expressions](https://docs.microsoft.com/zh-tw/cpp/cpp/types-of-expressions?view=msvc-160")
- [23. What is an Expression and What are the types of Expressions?](https://www.geeksforgeeks.org/what-is-an-expression-and-what-are-the-types-of-expressions/")
- [24. Identifier expressions (C++ only)](https://www.ibm.com/docs/en/i/7.3?topic=expressions-identifier-c-only")
- [25. Expressions](http://eel.is/c++draft/expr#conv.rval")
- [26. Integral Types](https://www.cs.auckland.ac.nz/references/unix/digital/AQTLTBTE/DOCU_032.HTM")
- [27. Boolean conversions](https://en.cppreference.com/w/cpp/language/implicit_conversion#Boolean_conversions") (cppreference)
- [28. Static vs dynamic type checking in C++](https://stackoverflow.com/questions/1347691/static-vs-dynamic-type-checking-in-c")
- [29. What is an rvalue reference to function type?](https://stackoverflow.com/questions/7016777/what-is-an-rvalue-reference-to-function-type")
- [30. 淺析std::ref與reference_wrapper](https://blog.csdn.net/u014645632/article/details/78966340")
- [31. rvalue reference to function](https://stackoverflow.com/questions/25146508/rvalue-reference-to-function")
- [32. Is it legal to cast a non-class non-array prvalue to a cv-qualified type?](https://stackoverflow.com/questions/42930598/is-it-legal-to-cast-a-non-class-non-array-prvalue-to-a-cv-qualified-type")
- [33. abstract class](https://en.cppreference.com/w/cpp/language/abstract_class") (cppreference)
- [34. Guaranteed Copy Elision Does Not Elide Copies](https://devblogs.microsoft.com/cppblog/guaranteed-copy-elision-does-not-elide-copies/")
- [35. storage duration vs lifetime](https://stackoverflow.com/questions/21392160/storage-duration-vs-lifetime")
- [36. Object](https://en.cppreference.com/w/cpp/language/object") (cpprefernce)
- [37. Why are function expressions returning non-reference types considered to be prvalues and not xvalues?](https://stackoverflow.com/questions/42043414/why-are-function-expressions-returning-non-reference-types-considered-to-be-prva")
- [38. What are copy elision and return value optimization?](https://stackoverflow.com/questions/12953127/what-are-copy-elision-and-return-value-optimization/12953150?fbclid=IwAR3nCd45hj09HtaKwDydF3Ouje_VzfQKP336gJPbRjNnmflu7fzlBmqJByA#12953150")
- [39. Copy elision](https://en.cppreference.com/w/cpp/language/copy_elision") (cppreference)
- [40. Reference declaration](https://en.cppreference.com/w/cpp/language/reference") (cppreference)
- [41. Reference initialization](https://en.cppreference.com/w/cpp/language/reference_initialization#Lifetime_of_a_temporary") (cppreference)
- [42. 淺談C++11標準中的複製省略 (copy elision, 也叫RVO返回值優化)](https://blog.csdn.net/davidhopper/article/details/90696200")
- [43. Wording for guaranteed copy elision through simplified value categories](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0135r1.html")
- [44. What does it mean “xvalue has identity”?](https://stackoverflow.com/questions/50783525/what-does-it-mean-xvalue-has-identity")
- [45. Is there any way to find the address of a reference?](https://stackoverflow.com/questions/1950779/is-there-any-way-to-find-the-address-of-a-reference")
- [46. What are the differences between a pointer variable and a reference variable in C++?](https://stackoverflow.com/questions/57483/what-are-the-differences-between-a-pointer-variable-and-a-reference-variable-in?page=2&tab=votes#tab-top")
- [47. What does it mean that a reference must refer to an object, not a dereferenced null pointer?](https://isocpp.org/wiki/faq/references#refs-not-null")
- [48. Is always the address of a reference equal to the address of origin?](https://stackoverflow.com/questions/45821678/is-always-the-address-of-a-reference-equal-to-the-address-of-origin")
- [49. Is a struct's address the same as its first member's address?](https://stackoverflow.com/questions/9254605/is-a-structs-address-the-same-as-its-first-members-address")
- [50. Variables and Memory](http://annwm.lbl.gov/~leggett/vars.html")
- [51. C++ how are variables accessed in memory?](https://stackoverflow.com/questions/35175795/c-how-are-variables-accessed-in-memory")
- [52. Is the cppreference definition of non-static data member wrong?](https://stackoverflow.com/questions/41148054/is-the-cppreference-definition-of-non-static-data-member-wrong")
- [53. Why is a non-static data member reference not a variable?](https://stackoverflow.com/questions/12987259/why-is-a-non-static-data-member-reference-not-a-variable")
- [54. Guaranteed copy elision through simplified value categories ](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0135r0.html") (P0135R0)
- [55. How does guaranteed copy elision work?](https://stackoverflow.com/questions/38043319/how-does-guaranteed-copy-elision-work")
- [56. Temporary materialization during array-to-pointer conversion](https://stackoverflow.com/questions/57392150/temporary-materialization-during-array-to-pointer-conversion")
- [57. standard conversions: Array-to-pointer conversion](https://stackoverflow.com/questions/4223617/standard-conversions-array-to-pointer-conversion")
- [58. I think I may have come up with an example of rvalue of array type](https://stackoverflow.com/questions/4058151/i-think-i-may-have-come-up-with-an-example-of-rvalue-of-array-type")
- [59. static_cast conversion](https://en.cppreference.com/w/cpp/language/static_cast") (cppreference)
- [60. copy elision in c++03](https://stackoverflow.com/questions/66422584/copy-elision-in-c03")
- [61. Guaranteed Copy Elision](https://jonasdevlieghere.com/guaranteed-copy-elision/")
- [62. No unique address attribute](https://eel.is/c++draft/dcl.attr.nouniqueaddr")
- [63. C++ attribute: no_unique_address](https://en.cppreference.com/w/cpp/language/attributes/no_unique_address") (cppreference)
- [64. Placement new base subobject of derived in C++](https://stackoverflow.com/questions/52884257/placement-new-base-subobject-of-derived-in-c")
- [65. The Coroutine in C++ 20 協程初探](https://zhuanlan.zhihu.com/p/237952115")
- [66. What are unevaluated operands in C++?](https://humanreadablemag.com/issues/0/articles/what-are-unevaluated-operands-in-c")
- [67. return statement](https://en.cppreference.com/w/cpp/language/return") (cppreference)
- [68. [計程] 變數的儲存週期](https://www.ptt.cc/bbs/b97902HW/M.1226692638.A.F2B.html")
- [69. 變數儲存期間](https://sites.google.com/a/qtm.ks.edu.tw/qtm052-org/cheng-shi-she-ji/visual-c/5-bian-shu-chu-cun-qi-jian")
- [70. C++軟體開發 - 返回值優化、具名返回值優化、複製省略 概念與實例](http://wucodingroad.blogspot.com/2017/05/Cplusplus-copyelision.html")
- [71. C++ 編譯器優化之RVO 與NRVO](https://www.yhspy.com/2019/09/01/C-%E7%BC%96%E8%AF%91%E5%99%A8%E4%BC%98%E5%8C%96%E4%B9%8B-RVO-%E4%B8%8E-NRVO/")
- [72. 詳解RVO與NRVO（區別於網上常見的RVO）](https://blog.csdn.net/Virtual_Func/article/details/48709617")
- [73. C++編譯器優化技術：RVO、NRVO和復制省略](https://www.cnblogs.com/kekec/p/11303391.html")
- [74. Why does/ does not NRVO kick in g++ in the code below?](https://stackoverflow.com/questions/9227608/why-does-does-not-nrvo-kick-in-g-in-the-code-below")
- [75. Explicit NRVO Proposal](https://www.reddit.com/r/cpp/comments/j25t46/explicit_nrvo_proposal/")
- [76. Return value optimization (RVO)](https://sigcpp.github.io/2020/06/08/return-value-optimization")
- [77. What is object slicing?](https://stackoverflow.com/questions/274626/what-is-object-slicing")
- [78. Why does C++11 have implicit moves for value parameters, but not for rvalue parameters?](https://stackoverflow.com/questions/9779079/why-does-c11-have-implicit-moves-for-value-parameters-but-not-for-rvalue-para")
- [79. Move constructors](https://en.cppreference.com/w/cpp/language/move_constructor") (cppreference)
- [80. More implicit moves](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1155r0.html") (P1155R0)
- [81. More implicit moves](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1155r3.html") (P1155R3)
- [82. The rule of three/five/zero](https://en.cppreference.com/w/cpp/language/rule_of_three") (cppreference)
- [83. 模板超程式設計](https://zh.wikipedia.org/wiki/%E6%A8%A1%E6%9D%BF%E8%B6%85%E7%B7%A8%E7%A8%8B")
- [84. 潮．C++ | 編譯期算八皇后所有解 Compile-time 8-Queens](https://tjsw.medium.com/%E6%BD%AE-c-%E7%B7%A8%E8%AD%AF%E6%9C%9F%E7%AE%97%E5%85%AB%E7%9A%87%E5%90%8E%E6%89%80%E6%9C%89%E8%A7%A3-compile-time-8-queens-d872bc063bdc")
- [85. Value Categories in C++17](https://medium.com/@barryrevzin/value-categories-in-c-17-f56ae54bccbe")