<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++ 教學系列 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50><br>Array 與 Pointer</center></h1>  
  
###### tags: `C++ Miner_tutorial`  
點此回到礦坑系列首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>  
  
# Array(陣列)  
  
Array 與 int、char、bool 一樣是一種型態，稱為 array type，不過 array 並不是 fundamental type，而是 compound type。  
  
Array 常用來儲存同一種資料，像是我們今天要儲存全班 40 人的成績，正常來說，我們需要 40 個變數，這太麻煩了，這種有多筆單一型別資料的狀況我們可以用陣列來幫助我們儲存。  
  
Array 是一段連續的記憶體空間，由許多小物件組成，這些小物件稱為 element，陣列可以擁有一個名字方便我們操作元素。  
  
> n4868(9.3.4.5)：An object of type “array of N U” consists of a contiguously allocated non-empty set of N subobjects of type U, known as the elements of the array, and numbered 0 to N-1.  
  
## 語法  
  
我們可以透過 simple declaration 來宣告出一個陣列型態的變數，陣列的宣告語法會長：  
  
> decl-specifier-seq(我自己加上來方便解釋的) noptr-declarator [ expr(optional) ] attr(optional)  
  
+ decl-specifier-seq  
      
    基本上跟上次講的一樣，type 的部分可以是 fundamental type(除了 void)、pointer type、pointer to member type、class、enumeration，或其他<span class = "yellow">已知元素數量的陣列型態</span>  
  
後方整坨都是宣告器，attr 我們先忽略它，也就是說宣告器的部分長：  
  
> noptr-declarator [ expr(optional) ]  
  
他們分別為：  
  
+ noptr-declarator  
      
    可以是任何合法的宣告器，如果沒有接初始化器，那麼陣列的元素內容會是未知的。如果 Pointer Declarator/Reference 要作用在指標/參考本身，而不是目標的話，那要加上括號把它們括住，例如  
      
    ```cpp  
    int main()  
	{  
	  int arr[2] = { 1, 2 };    // 一個有兩個整數元素的陣列型態，其變數名為 arr  
	  int arr2[2];    // 一個有兩個整數元素的陣列型態，其變數名為 arr2，但沒有初始化，內部元素可能為隨機值  
	  int *p_arr[2];    // 一個有兩個整數指標元素的陣列型態，其變數名為 p_arr，也沒有初始化  
	  int(*p_arr2[2])[2] = { &arr, &arr2 };    // 一個有兩個指向 int[2] 元素的陣列型態，其變數名為 p_arr2  
	  int(*a1)[2] = &arr;    // 一個指向 int[2] 型態的指標，其變數名為 a1，指向 arr  
	  int(&a2)[2] = arr;    // 一個參考 int[2] 型態的別名，其變數名為 a2，參考 arr  
	  return 0;  
	}  
    ```  
      
    這個部分基本上就是你陣列的變數名，視情況會再加上其他你需要的東西，讀的方法是從右至左，以 `int *p_arr[2]` 來說，`int` 屬於 type specifier，`*p_arr[2]` 屬於 declarator。  
      
    另外補充一點，Array 無法使用 assignment，也不能 copy，因為 array type 的物件無法一次修改全部的元素，也就是說像這樣是不行的：  
      
    ```cpp  
	int main()  
	{  
	  int a[3] = { 1, 2, 3 }, b[3] = { 4, 5, 6 };  
	  int c[3] = a;    // error: array must be initialized with a brace-enclosed initializer  
	  a = b;    // error: invalid array assignment  
	}  
    ```  
      
      
      
+ expr  
      
    一個可以轉換為 `const std::size_t` 型態的表達式，<span class = "yellow">必須大於 0</span>，`std::size_t` 你可以把它想像成一個正整數，專門拿來表示大小。  
      
    這個部分基本上是你陣列元素的數目，所以需要大於 0，且如果不是使用動態配置，那麼需要是個<span class = "yellow">編譯時期就能算出的值</span>，所以才會說需要是一個 constant expression，例如：  
      
    ```cpp  
    int main()  
	{  
	  int i = 40;    // i 不是 constant expression  
	  const int ci = 40;    // ci 是 constant expression  
	  int arr[i];    // error，i 不是 constant expression  
	  int arr2[ci];    // ok  
	  return 0;  
	}  
    ```  
  
    上面這段 code 你可能可以編譯成功，這是因為有些編譯器有幫忙做特殊的處理，像是先在 stack 上分配一些空間之類的，但這種 code 換個編譯器可能就不會過了，這種拿非編譯時期的值拿來當作其大小的陣列叫做 VLA(variable-length array)，有興趣的可以看看。  
      
    如果你沒有寫 expression，那就需要有初始化器，此時編譯器會幫忙從初始化器推算出陣列的元素個數，例如：  
    ```cpp  
    int main()  
	{  
	  int arr[] = { 1, 2 };  
	  return 0;  
	}  
    ```  
      
    這裡的 `arr` 是個整數的陣列，編譯器會幫忙推算出陣列有兩個元素。  
      
    > n4868(9.3.4.5)：An array bound may also be omitted when an object (but not a non-static data member) of array type is initialized and the declarator is followed by an initializer. In these cases, the array bound is calculated from the number of initial elements (say, N) supplied, and the type of the array is “array of N U”.  
      
    額外閱讀：[Why is new int[n] valid when int array[n] is not?](https://stackoverflow.com/questions/27495806/why-is-new-intn-valid-when-int-arrayn-is-not?fbclid=IwAR3z9HE5GLnUcPhavgUNpZWOTV35JVpzYXvNQfrcdUnWmp8zFQ5Wt_UeEv8)  
    額外閱讀：[In the standard, what is "derived-declarator-type"?](https://stackoverflow.com/questions/13779273/in-the-standard-what-is-derived-declarator-type)  
    額外閱讀：[Why aren't variable-length arrays part of the C++ standard?](https://stackoverflow.com/questions/1887097/why-arent-variable-length-arrays-part-of-the-c-standard)  
  
## 使用 Array  
  
`T a[N];` 這樣宣告了一個變數叫 `a`，型態為 `T[N]`，念法為 `array of N T`，array 中的元素會從 0 到 N - 1 的被編號，我們可以利用 `[]` operator 來使用這些元素，`[]` 內放要使用的元素的編號。  
  
`[]` 並不會對你填入的數字做檢查，也就是說如果你填了 N+1，編譯也會過，但這可能會導致我們訪問一個非法的記憶體位址，導致執行期的錯誤，舉個例子：  
  
```cpp  
int main()  
{  
  int arr[5] = { 1, 2, 3, 4, 5 };  
  int i = arr[0];    // arr[0] 為陣列中第一個元素，因此 i == 1  
  int i2 = arr[4];    // arr[4] 為陣列中第五個元素，因此 i2 == 5  
  int i3 = arr[5];    // 5 號超出 array 的元素個數了，這可能會導致執行期的錯誤，  
                      // 因為 arr[5] 這個記憶體位址可能不是可以使用的空間  
  return 0;  
}  
```  
  
## 多維陣列  
  
我們一開始提到了陣列的 decl-specifier-seq，那段講的意思簡單來說就是陣列的元素可以是 fundamental type(除了 void)、pointer、pointer to member、classes、enumeration 或<span class = "yellow">已知元素數量的陣列</span>。  
  
最後一項我特別標成了黃色，因為這代表陣列的元素也可以是陣列，也就是說我們可以有多維陣列這種東西，舉個例子：  
  
```cpp  
int main()  
{  
  int arr[2][3] = { { 1, 2, 3 },  
                    { 4, 5, 6 } };  
}  
```  
  
上面這樣的 `arr` 就是一個二維陣列，`arr` 的有兩個元素，這兩個元素的型態都是`int[3]`，也就是一個有三個整數元素的陣列，我們依樣可以使用 `[]` 來使用陣列中的元素：  
  
```cpp  
int main()  
{  
  int arr[2][3] = { { 1, 2, 3 },  
                    { 4, 5, 6 } };  
  
  int i = arr[0][0];    // i == 1  
  int i2 = arr[1][2]; // i == 6  
}  
```  
  
`arr[0][0]` 代表第一個小陣列裡面的第一個元素，而 `arr[1][2]` 則代表第二個小陣列的第三個元素，使用時一樣要注意編號的問題，小的陣列編號一樣是從 0 開始的。  
  
更高維的陣列規則都一樣，依樣畫葫蘆就好。  
  
## 字元陣列  
  
char array 是一種特殊的陣列，可以用來表示一組字串，一組字串以 `'\0'` 結尾，因此在使用字元陣列時記得要多加一個位址給 `'\0'`，舉個例子：  
  
```cpp  
#include <iostream>  
int main()  
{  
  char c1[] = { 'h', 'e', 'l', 'l', 'o' };    // 沒有 '\0'，陣列元素個數為 5 個  
  char c2[] = { 'h', 'e', 'l', 'l', 'o', '\0' };    // 有 '\0'，陣列元素個數為 6 個  
  char c3[] = "hello";    // 使用 string literal 初始化字元陣列的話會自動加上 '\0'，陣列元素個數為 6 個  
  char c4[5] = "hello";    // error: initializer-string for 'char [5]' is too long  
  
  std::cout << c1 << '\n'    // 可能後面會有亂碼，因為要讀到 '\0' 電腦才會知道這個字串結束了  
            << c2 << '\n'  
            << c3 << '\n';  
}  
```  
  
使用 list initialize 字元陣列時我們需要手動加上 `'\0'`，但如果是使用 string literal 來初始化就不用。  
  
## 搭配 for 迴圈遍歷陣列  
  
上週我們教了 for 迴圈，如果我們想要遍歷整個陣列，那搭配 for 迴圈來操作是個很好的選擇，能方便很多：  
  
```cpp  
#include <iostream>  
int main()  
{  
  int arr[2][4] = { { 1, 2, 3, 4 },  
                    { 5, 6, 7, 8 } };  
  
  for (int i = 0; i < 2; ++i) {  
    for (int j = 0; j < 4; ++j) {  
      std::cout << arr[i][j] << " ";  
    }  
    std::cout << '\n';  
  }  
}  
```  
  
# Pointer 指標  
  
Pointer 型態的變數存的值有四種：  
  
+ 物件或函式的位址  
+ 一物件尾端後的位址  
+ null pointer value  
+ 無效的值  
  
無效的值通常指沒有初始化的指標，或是指向一個未知、出界位址的指標。  
  
而第二項則是專門在處理出界問題的指標，如  
```cpp  
int main()  
{  
  int arr[2] = { 1, 2 };  
  int *p = &arr[2];    // 尾端指標  
}  
```  
  
裡面的 `p`，這東西會衍生出一系列的規則，挺麻煩的，這邊講一個比較重要的影響就好，在對指標進行比較與算術運算時，指向非陣列元素的指標會被視作一個指向「只有一個元素的陣列」的第一個元素的指標，講起來很繞口，直接看例子比較快：  
  
```cpp  
int main()  
{  
  int i = 0;  
  int *p = &i;  
  
  /**  
   * 在 p+1; 中，此時 p 被當作一個指向 int[0] 的指標，  
   * 你可以想像成：  
   * int arr[1] = {&i};  
   * int *p = &arr[0];  
   */  
  p + 1;  
}  
```  
  
這能幫助嚴謹定義出界的行為，更進一步處理指標加法、減法等等越界的問題，如果你看不懂沒關係，只要記住它衍生出來的重點就好：  
  
```cpp  
#include <cassert>  
int main()  
{  
  int i = 0;  
  int *p = &i;  
  assert(&(p[0]) == &i);    // 理論上要恆為 true  
}  
```  
  
也就是 `&(p[0])` 與 `&i` 理論上永遠是等價的。  
  
後面我會簡單複習一下語法，然後討論一下第一項與第三項，但先跳過函式。  
  
## 語法  
  
宣告的語法之前宣告器的部分有提過了，這邊正式且詳細的再寫一次，宣告的語法長這樣：  
  
> decl-specifier-seq \* attr(optional) cv(optional) derived-declarator-type-list  
  
這邊的 `*` 稱為 Pointer Declarator，假設我們寫 `S *D;`，則宣告了一個指標 `D` 指向 `S` 型態。除了一開始的 specifier，後面的東西都屬於宣告器。  
  
另外 Pointer 不能指向 reference 與 bit fields。  
  
舉個例子：  
  
```cpp  
#include <iostream>  
  
int main()  
{  
  int i = 20;  
  int *p1 = &i;  
  int **p2 = &p1;  
  
  std::cout << "i 儲存的值 : " << i << '\n'  
            << "i 的位址   : " << &i << '\n'  
            << "p1 儲存的值: " << p1 << '\n'  
            << "p1 的位址  : " << &p1 << '\n'  
            << "p2 儲存的值: " << p2 << '\n'  
            << "p2 的位址  : " << &p2 << '\n';  
  
  std::cout << "\n*p1 = " << *p1 << '\n'  
            << "*p2 = " << *p2 << '\n'  
            << "**p2 = " << **p2;  
}  
```  
  
圖解就會長這樣：  
  
<center><img src="https://i.imgur.com/lUlVnFM.png"></center><br>  
  
`i` 是一個整數變數，儲存的值為 20；`p1` 是個整數的指標，指向 `i`，儲存的值為 `&i`；`p2` 是個整數的指標的指標，指向 `p1`，儲存的值為 `&p1`。  
  
  
## Pointer to objects  
  
我們可以用 `address-of operator` 的回傳值或別的指標來初始化指標變數，舉個例子：  
  
```cpp  
int n;  
int *np = &n;    // pointer to int  
int *const *npp = &np;    // non-const pointer to const pointer to non-const int  
  
int a[2];  
int (*ap)[2] = &a;    // pointer to array of int  
  
struct S {  
  int n;  
};  
S s = { 1 };  
int *sp = &s.n;    // pointer to the int that is a member of s  
```  
source：[cppreference](https://en.cppreference.com/w/cpp/language/pointer#Pointers_to_objects)  
  
### 指標的運算  
  
指標的運算，如 `+`、`-` 等 operator 有自己的定義，這邊簡單講一下 `+`、`-` 與 `==`：  
  
前兩個合稱為 Additive operators，當他們的運算元為一個指標和一個整數時，會根據指標指向的型態大小來位移，舉個例子：  
```cpp  
#include <iostream>  
  
int main()  
{  
  int a = 0;  
  int *p = &a;  
    
  std::cout << "p 指向:" << p << '\n'  
            << "p + 1:" << p + 1 << '\n'  
            << "p + 2:" << p + 2 << '\n'  
            << "2 + p:" << 2 + p;  
  
  return 0;  
}  
```  
source：[C++ Gossip](https://openhome.cc/Gossip/CppGossip/PointerArithmetic.html)  
  
這個輸出的是記憶體位址，因為這邊是 `int`，在我的電腦上是 4 byte，所以每對指標加一，其值就會加上 4(位移 4)，減法同理；另外 `N + ptr` 與 `ptr + N` 等價，這邊 `N` 為整數，`ptr` 為指標變數名。  
  
而指標也可以與指標相減，其結果會是兩個記憶體間的差 (difference)：  
  
```cpp  
#include <iostream>  
int main()  
{  
  int a[4] = { 1, 2, 3, 4 };  
  int *p = &a[1];  
  int *p2 = &a[3];  
  std::cout << "p 指向:" << p << '\n'  
            << "p2 指向:" << p2 << '\n'  
            << "Pointer difference: " << p2 - p << '\n';  
}  
```  
  
source：改自 [cppreference](https://en.cppreference.com/w/cpp/language/operator_arithmetic#Additive_operators)  
  
而比較運算子的話只有 `==` 與 `!=` 能用在 pointer 上，詳細的狀況可以到 [cppreference](https://en.cppreference.com/w/cpp/language/operator_comparison#Pointer_comparison_operators) 上看，但簡單來說，如果兩者儲存的位址一樣，那 `==` 回傳 `true`，否則為 `false`。  
  
## Null pointers  
  
就像 int 有 0，char 有空字元這種基本單位一樣，pointer 也有，它是一個特殊的值，叫做 null pointer value，基本上它有三種表示方法，`NULL`、`nullptr` 與 `0`。  
  
一個值為 null 的 pointer 不會指向任何的物件或函式，且對一個 null pointer 做 dereference 是 UB，也就是說：  
  
```cpp  
int *p = nullptr;  
int a = *p;    // undefined behavior  
```  
  
這樣是 UB。  
  
不管什麼型態的指標，儲存的值如果是 `null`，那麼拿去與同型態的空指標做比較運算(`==`)，出來的結果也會是 `true`。  
  
`nullptr` 為 null pointer literal，`NULL` 為 null pointer constant，而 `0` 則是整數，會被隱式轉型為空指標，三者雖然出來的結果一樣，但過程有些許差異。  
  
通常一個指標被宣告出來，但不需要初始化時，我們通常會把指標初始化為 `nullptr`，又或是一個指標指向的物件已經被解構時，我們也會將其值設為 `nullptr`，這稱為指標歸位，能夠避免我們不小心用到儲存亂數的指標，後者這種指標稱為野指標(dangling pointer)  
  
## Constness on Pointer  
  
之前講宣告的時候我們有在宣告器與說明符的部分同時看到 `const`，也就是說一個宣告述句裡面就有兩個 `const` 出現，當時我們有簡單介紹一下，現在我們來詳細看一下情況：  
  
+ 如果 `cv-qualifier` 出現在 `*` 前，那 `*` 屬於 decl-specifier-seq，作用於指向的物件上  
+ 如果 `cv-qualifier` 出現在 `*` 後，那 `*` 屬於 declarator，作用於指標物件上  
  
所以我們可以簡單寫一個表出來：  
  
  
  
| 語法 | 意思 |  
| -------- | -------- |  
| `const T*` | 指向 「constant 物件」的指標 |   
| `T const*` | 指向 「constant 物件」的指標 |   
| `T* const` | 指向普通物件的「constant 指標」 |   
| `const T* const` | 指向「constant 物件」的「constant 指標」 |   
| `T const* const` | 指向「constant 物件」的「constant 指標」 |   
  
source：[cppreference](https://en.cppreference.com/w/cpp/language/pointer#Constness)  
  
可以看見就分三種，不寫法有五種，判斷方法建議大家記一下，接下來我們簡單看個例子：  
  
```cpp  
#include <iostream>  
  
int main()  
{  
  int i = 0;  
  const int ci = 0;  
  
  int *const ptr1_c = &i;    // constant pointer  
  int *const ptr2_c = &ci;    // error, 指向的物件型態應該要是一個 int，但 ci 是 const int  
  
  const int *c_ptr1 = &i;    // ok, 會間接限制不能透過 c_ptr1 改 i 的值  
  *c_ptr1 = 5;    // error  
  const int *c_ptr2 = &ci;    // ok  
  
  const int *const c_ptr1_c = &i;    // ok，c_ptr1_c 儲存的值不能再改，且也不能透過指標修改 i 的值  
  const int *const c_ptr1_c = &ci;    // ok，同上  
}  
```  
  
延伸閱讀：[How to interpret complex C/C++ declarations](https://www.codeproject.com/Articles/7042/How-to-interpret-complex-C-C-declarations)  
  
如果想要看 pointer of pointer 的版本的話，可以看一下上面這篇，主要是有個規則叫做 RTL Rule(right-to-left rule)。  
  
# Array to pointer decay(conversion)  
  
array 在「需要指標，卻填入 array」的狀況時會發生轉型，轉型為一個「指向第一個元素」的指標。  
  
這個情況很常發生，也就是說這個轉型很常發生，導致有許多人認為 Array 與 Pointer 是一樣的東西，但<span class = "yellow">不是，Array 與 Pointer 是不同的東西</span>。  
  
舉個簡單的例子：  
  
```cpp=  
#include <iostream>  
  
int main()  
{  
  int arr[3] = { 1, 2, 3 };  
  int *p = arr;    // 發生轉型，expression arr 的計算結果型態由 int[3] 轉型為 int*，指向陣列第一個元素  
}  
```  
  
上面這個例子第六行的地方就發生了轉型，`arr` 由一個陣列型態轉型為整數的指標了，此時 `p` 指向 `arr` 的第一個元素。  
  
<span class = "yellow">每一次的 array assign to pointer 都應該視為一種「轉型」</span>！！！！！！！！！  
  
> n4868(7.3.3)：An lvalue or rvalue of type “array of N T” or “array of unknown bound of T” can be converted to a prvalue of type “pointer to T”. The temporary materialization conversion ([conv.rval]) is applied. The result is a pointer to the first element of the array.  
  
延伸閱讀：[[重新理解 C++] Array 和 Pointer 的差異](https://zh-tw.coderbridge.com/series/9c0fd91d2bbb4986b0b451aed1319325/posts/eec6d2b3309e4cafbda712cae3b46f32?fbclid=IwAR1ZBMdoZ_nfQURvsDpZiqT9zgiTwYQJpH4nSEeDK04EvSwNZDERRsLUJv0)  
  
## 何時發生?  
  
基本上原則就是上面講的那樣，是不用記得太詳細，除非踩到坑，但如果想看詳細一點的可以看看以下幾個條例：  
  
### Expressions  
  
> [n4868(7.2.1)](https://timsong-cpp.github.io/cppwp/n4868/basic.lval#6)：Whenever a glvalue appears as an operand of an operator that expects a prvalue for that operand, the lvalue-to-rvalue, array-to-pointer, or function-to-pointer standard conversions are applied to convert the expression to a prvalue.  
  
> [n4868(7.2.3)](https://timsong-cpp.github.io/cppwp/n4868/expr.context#2)：In some contexts, an expression only appears for its side effects. Such an expression is called a discarded-value expression. The array-to-pointer and function-to-pointer standard conversions are not applied.  
  
### Type identification  
  
> [n4868(7.6.1.8)](https://timsong-cpp.github.io/cppwp/n4868/expr.typeid#3)：When typeid is applied to an expression other than a glvalue of a polymorphic class type, the result refers to a std::type_info object representing the static type of the expression. Lvalue-to-rvalue, array-to-pointer, and function-to-pointer conversions are not applied to the expression.  
  
### sizeof operator  
  
> [n4868(7.6.2.5)](https://timsong-cpp.github.io/cppwp/n4868/expr.sizeof#3)：The lvalue-to-rvalue ([conv.lval]), array-to-pointer ([conv.array]), and function-to-pointer ([conv.func]) standard conversions are not applied to the operand of sizeof. If the operand is a prvalue, the temporary materialization conversion is applied.  
  
### Three-way comparison operator  
  
> [n4868(7.6.8)](https://timsong-cpp.github.io/cppwp/n4868/expr.spaceship#6)：If at least one of the operands is of object pointer type and the other operand is of object pointer or array type, array-to-pointer conversions ([conv.array]), pointer conversions ([conv.ptr]), and qualification conversions are performed on both operands to bring them to their composite pointer type ([expr.type]). After the conversions, the operands shall have the same type.  
>   
> [Note 1: If both of the operands are arrays, array-to-pointer conversions are not applied. — end note]  
  
### 關係運算子  
  
> [(7.6.9)](https://timsong-cpp.github.io/cppwp/n4868/expr.rel#1)：The lvalue-to-rvalue ([conv.lval]), array-to-pointer ([conv.array]), and function-to-pointer ([conv.func]) standard conversions are performed on the operands. The comparison is deprecated if both operands were of array type prior to these conversions ([depr.array.comp]).  
  
### 比較運算子  
  
> [(7.6.10)](https://timsong-cpp.github.io/cppwp/n4868/expr.eq#1)：The == (equal to) and the != (not equal to) operators group left-to-right. The lvalue-to-rvalue ([conv.lval]), array-to-pointer ([conv.array]), and function-to-pointer ([conv.func]) standard conversions are performed on the operands. The comparison is deprecated if both operands were of array type prior to these conversions ([depr.array.comp]).  
  
### 三元運算子  
  
這要點進去看一下  
  
> [(7.6.16)](https://timsong-cpp.github.io/cppwp/n4868/expr.cond#4.3.3)：...otherwise, the target type is the type that E2 would have after applying the lvalue-to-rvalue, array-to-pointer, and function-to-pointer standard conversions.  
  
# Subscript operator and Pointer  
  
除了計算順序上的不同，`[]` 基本上可以用 `*()` 來代替，`E1[E2]` 與 `*(E1 + E2)` 基本上完全等價。  
  
也就是說假設有個陣列，由於有 Array to pointer decay，又有指標的加法，因此在拜訪元素時我們可以透過 `*()` 來代替 `[]` 的操作，舉個例子：  
  
```cpp=  
int arr[3] = { 1, 2, 3 };  
int i1 = arr[2];  
int i2 = *(arr + 2);  
```  
  
這裡的 `i1` 與 `i2` 是完全相同的，在第三行中，`arr` 先被轉換為一個整數型態的指標，然後再透過指標的加法取得陣列第二個元素的位址，最後再 dereference，結果與 `arr[2]` 完全一樣。  
  
> [n4861(9.3.4.5)](https://timsong-cpp.github.io/cppwp/n4861/dcl.array#9)：  
> [Example 4:  
>> int x3d[3][5][7];  
>  
> ... The expression x3d[i] is equivalent to *(x3d + i); in that expression, x3d is subject to the array-to-pointer conversion ([conv.array]) and is first converted to a pointer to a 2-dimensional array with rank 5×7 that points to the first element of x3d. Then i is added, which on typical implementations involves multiplying i by the length of the object to which the pointer points, which is sizeof(int)×5×7. ... — end example]  
  
  
{%hackmd aPqG0f7uS3CSdeXvHSYQKQ %}