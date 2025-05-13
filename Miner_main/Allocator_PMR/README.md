---
title: (WIP) 礦坑系列 ── Allocator & PMR
date: 2025-04-12
tag: C++ Miner-main
category: C++ Miner
---

<h1><div class = "center-column"><img src = "https://walfiegif.wordpress.com/wp-content/uploads/2023/07/out-transparent-122.gif?" width = 50> 礦坑系列 <img src = "https://walfiegif.wordpress.com/wp-content/uploads/2023/07/out-transparent-122.gif?" width = 50>Allocator & PMR</div></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

## 前言

原本會有這篇是因為 MISRA C/C++ spec 內規定不能用 heap allocation，朋朋剛好遇到了這個需求，所以來問了一下能不能把 data 全部放在 stack 段上的 memory pool，剛好要讀書會了，就拿這個主題來寫一篇

Allocator 與 PMR 讓你能在 stack 上開一個 memory pool，利用 STL 給的 API 去操控它，STL 的容器也可以將 data 配置於其中，不必再使用到 heap，進而避免 heap allocation，帶來一些加速的效果，尤其是對樹狀結構的 heap allocation 而言，在 free memory 時其也會有一定的負擔，對於這種結構的加速會更好

這篇文會參考 Jason Turner 的影片（應該很多人看過?），還有 C++17 Complete Guide 裡面的章節，順便再整理一下幾場 CppCon 與 [N3916](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3916.pdf) 的內容

底下是一個 Jason Turner 在影片底下給的 `std::list` 的例子（[Quick C++ Benchmark 連結](https://www.youtube.com/redirect?event=video_description&redir_token=QUFFLUhqbkVWWEppM3NqTl9pWmRQQXJIcHpUMDNxcFlSUXxBQ3Jtc0ttX1d5TGRXSEluTU1PWWZlcHhCLUNwUE9LdUlYVml5ODNvNGZRdUVYX1R0N1kwYnpTYjBnT1k1bzlQeU05c1pBQ3kzb0ZVckdFLVAtdHlEcUtCM1RjeVNiOU5EX0RQYkFDb2E1OENKeTdpTlN4UE5qTQ&q=https%3A%2F%2Fquick-bench.com%2Fq%2FgyRE6p5EdZ-2P0XOAUwO99JRN7g&v=q6A7cKFXjY0)）：

```cpp
#include <memory_resource>
#include <list>

void StdList(benchmark::State& state) {
  for (auto _ : state) {
    std::list<int> list{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    benchmark::DoNotOptimize(list);
  }
}
// Register the function as a benchmark
BENCHMARK(StdList);

void PmrList(benchmark::State& state) {
  std::array<std::byte, 1024> buffer;
  for (auto _ : state) {
    std::pmr::monotonic_buffer_resource mem_resource(buffer.data(), buffer.size());
    std::pmr::list<int> list({1, 2, 3, 4 , 5, 6, 7, 8, 9, 10}, &mem_resource);
    benchmark::DoNotOptimize(list);
  }
}
// Register the function as a benchmark
BENCHMARK(PmrList);
```

輸出結果：

![alt text](image/quick_bench.png)

你可以看到，由於不需要 heap allocation，因此效能提升了許多。 那我們就先講如何使用，後面再來談談一些標準內的內容

## Allocator 與 `std::allocator`

### 歷史簡介

回顧一下我們的目的 — 不要使用 heap，對於一些類 C（POD type）的物件，我們可以使用 placement new 達到這點：

```cpp
#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <type_traits>

struct T {
	unsigned int i1;
	unsigned int i2;
	unsigned int i3;
	unsigned int i4;
};

int main() {
	static_assert(std::is_trivial<T>::value && std::is_standard_layout<T>::value, "T must be POD"); // passed

	// construct aligned buffer
	using StorageType = std::aligned_storage<sizeof(T), alignof(T)>::type;
	std::array<StorageType, 5> buffer;

	// construct object with type T
	void* raw_ptr = &buffer[1];
	T* t_ptr = new (raw_ptr) T();
	t_ptr->i1 = 0x00000001;
	t_ptr->i2 = 0x10000000;
	t_ptr->i3 = 0x01000001;
	t_ptr->i4 = 0xffffffff;

	std::cout << "sizeof(T): " << sizeof(T) << std::endl;
	std::cout << "alignment of T: " << alignof(T) << std::endl;
	std::cout << "sizeof(unsigned char): " << sizeof(unsigned char) << std::endl;
	std::cout << "sizeof(buffer): " << sizeof(buffer) << std::endl;

	// print the buffer
	unsigned char* p = reinterpret_cast<unsigned char*>(buffer.data());
	std::size_t total_size = sizeof(buffer);
	for (std::size_t i = 0; i < total_size; ++i) {
		std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(p[i]) << " ";
		if ((i + 1) % sizeof(unsigned int) == 0) std::cout << "| ";
		if ((i + 1) % 16 == 0) std::cout << '\n';
	}
}
```

[輸出（`-std=c++11`）]（(https://godbolt.org/z/16x4rPE3z)）：

```
sizeof(T): 16
alignment of T: 4
sizeof(unsigned char): 1
sizeof(buffer): 80
00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 
01 00 00 00 | 00 00 00 10 | 01 00 00 01 | ff ff ff ff | 
00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 
00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 
00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | 
```

:::info  
POD type 在 C++20 時正式被 `trivial` 與 `standard-layout` 這兩種型態取代了，傳統的 POD type 在 C++20 後被表達為 `trivial` 且 `standard-layout`，這邊就不展開了，可以看一下 [P0767R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0767r1.html) 提案的結論，或是[這篇 stackoverflow](https://stackoverflow.com/questions/48225673/why-is-stdis-pod-deprecated-in-c20)  
:::

但這種方法有幾個主要的問題：

1. 不方便對 STL 容器使用，因為 STL 容器使用 Allocator 來分配記憶體
2. 無法將記憶體空間的配置與建構分開
3. 麻煩，像是需要手動處理對齊等問題，上例使用 `std::aligned_storage` 來處理（deprecated in C++20）

1995 年 3 月，dr.dobb's journal 特約記者 Al Stevens 採訪了 STL 創始人 Alexander Stepanov，在訪談中他提到：

> People wanted containers independent of the memory model, which was somewhat excessive because the language doesn't include memory models. People wanted the library to provide some mechanism for abstracting memory models.<br><br>
> 
> Earlier versions of STL assumed that the size of the container is expressible as an integer of type `size_t` and that the distance between two iterators is of type `ptrdiff_t`. And now we were told, why don't you abstract from that?<br><br>
> 
> It's a tall order because the language does not abstract from that; C and C++ arrays are not parameterized by these types. We invented a mechanism called "allocator," which encapsulates information about the memory model. That caused grave consequences for every component in the library.<br><br>
> 
> You might wonder what memory models have to do with algorithms or the container interfaces. If you cannot use things like `size_t`, you also cannot use things like `T*` because of different pointer types (`T*`, `T huge *`, etc.). Then you cannot use references because with different memory models you have different reference types. There were tremendous ramifications on the library. 

簡單來說，Allocator 提供了抽象記憶體的機制，封裝了 memory model 的資訊。 其動機是讓容器能夠完全獨立於底層的 memory model，原本是打算讓 Allocator 完全封裝 memory model，但後來因為效率問題導致加了「億些」限制，在當時被稱為 Allocator Requirement（[N1804](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1804.pdf) 20.1.6）

對此 Stepanov 在另一場訪談中也有提到一些感想（[連結](http://www.stlport.org/resources/StepanovUSA.html)），再到更後來的 C++11，Allocator 的變化可以說是已經完全與當初的目的不同了。 在目前，Allocator 的作用主要是讓工程師可以控制容器內的記憶體配置，一樣是負責封裝了記憶體管理，但就不是當初的調整底層 memory model 的目的了

::: info  
Al Stevens 的這篇訪談紀錄侯捷有轉載翻譯版到他的網站上，但禁止二次轉載，因此這邊就貼上連結給大家去讀讀，是篇很好但很舊的文章（比我還老XD）

連結：[STL 之父訪談錄](https://web.archive.org/web/20140307111122fw_/http://jjhou.boolan.com/myan-alexander.htm)  
:::

### Allocator usage

剛剛提了三點主要的問題，第三點你應該可以很直觀的感受到，所以我們注重在第一、二點，對於第一點，其實是可以做到的，但挺噁心的，下面是個例子：

```cpp
#include <iostream>
#include <vector>

void* operator new(std::size_t size) {
  std::cout << "[global operator new] allocating " << size << " bytes\n";
  return malloc(size);
}

void operator delete(void* ptr) noexcept {
  std::cout << "[global operator delete]\n";
  free(ptr);
}

int main() {
  std::vector<int> vec(10);
}
```

輸出：

```
[global operator new] allocating 40 bytes
[global operator delete]
```

可以看到我們是能夠通過 operator new overloading 來達到目的的，但這個就...... 一言難盡，除了在做 address sanitizer，我是想不到有什麼非得用這個的場合，讓你需要去動到全域的 operator new

:::warning  
另外，我不確定是不是所有的編譯器實作中 Allocator 底層都會呼叫到 `new`，也許有些編譯器會直接跳過 new 用 malloc（純猜測），這待商榷，但能確定的是大部分的編譯器底層都會用到 operator new

<details> <summary>[點開]：一個簡單的 uftrace log 範例</summary>

```
            [210003] |           std::allocator_traits::construct() {
   0.034 us [210003] |             std::forward();
            [210003] |             std::pmr::polymorphic_allocator::construct() {
   0.034 us [210003] |               std::forward();
            [210003] |               std::uninitialized_construct_using_allocator() {
   0.038 us [210003] |                 std::forward();
            [210003] |                 _ZSt32uses_allocator_construction_argsIiNSt3pmr21polymorphic_>
   0.039 us [210003] |                   std::forward();
            [210003] |                   std::tuple::tuple() {
   0.035 us [210003] |                     std::forward();
            [210003] |                     std::_Tuple_impl::_Tuple_impl() {
   0.038 us [210003] |                       std::forward();
            [210003] |                       std::_Head_base::_Head_base() {
   0.036 us [210003] |                         std::forward();
   0.135 us [210003] |                       } /* std::_Head_base::_Head_base */
   0.337 us [210003] |                     } /* std::_Tuple_impl::_Tuple_impl */
   0.519 us [210003] |                   } /* std::tuple::tuple */
   0.702 us [210003] |                 } /* _ZSt32uses_allocator_construction_argsIiNSt3pmr21polymor>
            [210003] |                 std::apply() {
   0.036 us [210003] |                   std::forward();
   0.036 us [210003] |                   std::forward();
            [210003] |                   std::__apply_impl() {
   0.037 us [210003] |                     std::forward();
            [210003] |                     std::get() {
            [210003] |                       std::__get_helper() {
            [210003] |                         std::_Tuple_impl::_M_head() {
   0.035 us [210003] |                           std::_Head_base::_M_head();
   0.145 us [210003] |                         } /* std::_Tuple_impl::_M_head */
   0.228 us [210003] |                       } /* std::__get_helper */
   0.036 us [210003] |                       std::forward();
   0.407 us [210003] |                     } /* std::get */
   0.035 us [210003] |                     std::forward();
            [210003] |                     std::__invoke() {
   0.036 us [210003] |                       std::forward();
   0.038 us [210003] |                       std::forward();
            [210003] |                       std::__invoke_impl() {
   0.037 us [210003] |                         std::forward();
   0.037 us [210003] |                         std::forward();
            [210003] |                         std::uninitialized_construct_using_allocator::$_0::op>
   0.035 us [210003] |                           std::forward();
            [210003] |                           _ZSt12construct_atIiJiEEDTgsnwcvPvLi0E_T_pispcl7dec>
   0.036 us [210003] |                             operator new();
   0.037 us [210003] |                             std::forward();
   0.232 us [210003] |                           } /* _ZSt12construct_atIiJiEEDTgsnwcvPvLi0E_T_pispc>
   0.414 us [210003] |                         } /* std::uninitialized_construct_using_allocator::$_>
   0.694 us [210003] |                       } /* std::__invoke_impl */
   0.976 us [210003] |                     } /* std::__invoke */
   1.702 us [210003] |                   } /* std::__apply_impl */
   1.976 us [210003] |                 } /* std::apply */
   2.912 us [210003] |               } /* std::uninitialized_construct_using_allocator */
   3.099 us [210003] |             } /* std::pmr::polymorphic_allocator::construct */
   3.281 us [210003] |           } /* std::allocator_traits::construct */
```

</details>
:::

而第二點是最常被拿來與 `new` 討論的差別，這邊拿 C++ Primer 內的例子來舉例：

```cpp
std::string* const p = new std::string[n]; // 建構 n 個空的 std::string
std::string s;
std::string* q = p;							  // q 指向第一個 std::string
while (std::cin >> s && q != p + n) 
  *q++ = s; // 指定一個新的值給 *q

size_t const size = q - p;					  // 記住我們讀取了多少個 std::string
delete[] p;
```

這個 `new` operator 配置並初始化了 `n` 個 `std::string`，但我們可能不需要這麼多，這樣一來我們就有可能會建構從未被使用的物件。 而對於有用到的物件，我們需要再做一次額外的賦值，因此一個元素被寫入了兩次。 最後，沒有 default constructor 的類別沒有辦法用這種方法來配置一個陣列

使用 Allocator 時能讓我們將 allocation 與 construction 分開，讓我們可以操作一塊有 type info 且還沒被建構的原始記憶體。 前面也提到 Allocator 封裝了記憶體管理，STL 容器（像是 `std::vector`、`std::map` 等）在分配記憶體時都是透過 Allocator 提供的介面 `std::allocator_traits` 在操作的，因此流程基本上長這樣：

![alt text](image/STL_container.png)

對於 STL 容器，我們可以使用 `std::allocator` 來配置記憶體：

```cpp
std::allocator<std::string> alloc; // 能夠配置 std::string 的物件
auto const p = alloc.allocate(n);  // 配置 n 個未建構的 std::string
```

並透過 `allocate` 來分配記憶體：

```cpp
std::allocator<std::string> alloc;		  // 能夠配置 std::string 的物件
std::string* const p = alloc.allocate(n); // 配置 n 個未建構的 std::string
```

接著使用 `std::allocator_traits`（或 placement new）來建構：

```cpp
#include <iostream>
#include <memory>
#include <string>

int main() {
	const int n = 5;
	std::allocator<std::string> alloc;
	using traits = std::allocator_traits<decltype(alloc)>;

	std::string* p = traits::allocate(alloc, n);

	for (int i = 0; i < n; ++i)
		traits::construct(alloc, p + i, "Hello " + std::to_string(i));

	for (int i = 0; i < n; ++i) {
		std::cout << p[i] << '\n';
		traits::destroy(alloc, p + i);
	}

	traits::deallocate(alloc, p, n);
}
```

[輸出（`-std=c++11`）](https://godbolt.org/z/sW4fjTMP3)）：

```
Hello 0
Hello 1
Hello 2
Hello 3
Hello 4
```

::: info  
這邊我並不像 C++ Primer 內使用 `alloc.construct`，這是因為這東西在 C++17 後被遺棄，C++20 時被移除了，主要是因為它的功能與 `std::allocator_traits` 重複了，更詳細的請去看 [D0174R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0174r0.html#2.4) 或是這篇 stackoverflow：[Why are are std::allocator's construct and destroy functions deprecated in c++17?](https://stackoverflow.com/questions/39414610/why-are-are-stdallocators-construct-and-destroy-functions-deprecated-in-c17)  
:::  

### 自定義 Allocator

在標準中，允許使用自定義 Allocator 的容器被稱為 [AllocatorAwareContainer](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer)，基本上除了 `std::array` 以外的容器都是，它們在使用 Allocator 時不是只接使用 Allocator 本身，而是透過 `std::allocator_trait` 這個介面來去間接地使用 Allocator

:::info  
對於 `std::string` 來說比較特別，因為它有 SSO，所以在標準內算是個特例  
:::

:::tip  
[N4950（24.2.1）](https://timsong-cpp.github.io/cppwp/n4950/container.requirements#pre-3)：Allocator-aware containers ([container.alloc.reqmts]) other than `basic_string` construct elements using the function `allocator_traits<allocator_type>​::​rebind_traits<U>​::​​construct` and destroy elements using the function `allocator_traits<allocator_type>​::​rebind_traits<U>​::​​destroy` ([allocator.traits.members]), where U is either allocator_type​::​value_type or an internal type used by the container. These functions are called only for the container's element type, not for internal types used by the container.

[Note 1: This means, for example, that a node-based container would need to construct nodes containing aligned buffers and call construct to place the element into the buffer. — end note]  
:::

這麼做是因為標準對 Allocator 洋洋灑灑的列了許多要求，基本上就是規定要有哪些成員變數與成員函式，而且不同的成員還有各自需要滿足的要求，可以從 [cppreference](https://en.cppreference.com/w/cpp/named_req/Allocator) 上面看到許多表格來描述它們

但如果每次客製化時都要把這些要求一個一個完成，那就太麻煩了，畢竟真的很多，因此才需要 `std::allocator_trait` 這個中介層，這個東西對大部分的需求提供了一個「預設」的版本，如此一來我們只需要對在意的操作進行客製化，其他的部分使用 `std::allocator_trait` 的版本即可

因此，在我們需要自己管理容器的記憶體時，可以通過自己寫一個 Allocator，套用給 `std::allocator_traits` 來達到目的，不知道大家還記不記得這篇文的初衷，我們不想要 heap allocation

另外在一個還不錯的系列文 [C/C++ 修道院](https://zhuanlan.zhihu.com/p/185611161) 中還提到一些特殊但合理的需求：

- 有些嵌入式平台沒有提供預設的 `malloc`/`free` 等底層記憶體管理函式，你需要繼承 `std::allocator`，並封裝自訂版本的 `malloc`/`free` 等更底層的 heap 記憶體管理函式
- 自己實作的資料結構，有時會需要擴充(繼承) `std::allocator`

以 Design Pattern 來說，這在語言設計上用的是 Adapter Pattern，`std::allocator_traits` 是一個 Adapter，用來包裝我們自訂的 Allocator，以滿足較少的實作需求，一旦你把你的 Allocator 包進 `std::allocator_traits`，就等於你獲得了一個 stateless、無重複實作負擔、可共用的 Allocator 代理人；你寫的 custom Allocator 可能只提供了一些簡單功能，但 `std::allocator_traits` 可以幫你補齊介面

至於 `std::allocator_traits` 具體提供了哪些預設的成員可以到 [cppreference](https://en.cppreference.com/w/cpp/memory/allocator_traits) 上面看。 有一位 committee 的成員 Howard Hinnant 在[他的部落格](https://howardhinnant.github.io/allocator_boilerplate.html)上提供了一個可以讓我們快速複製貼上的版本，class 的名字可以改成你想要的名稱：

```cpp
template <class T>
class Allocator {
  public:
	using value_type = T;

	//     using pointer       = value_type*;
	//     using const_pointer = typename std::pointer_traits<pointer>::template
	//                                                     rebind<value_type const>;
	//     using void_pointer       = typename std::pointer_traits<pointer>::template
	//                                                           rebind<void>;
	//     using const_void_pointer = typename std::pointer_traits<pointer>::template
	//                                                           rebind<const void>;

	//     using difference_type = typename std::pointer_traits<pointer>::difference_type;
	//     using size_type       = std::make_unsigned_t<difference_type>;

	//     template <class U> struct rebind {typedef Allocator<U> other;};

	Allocator() noexcept {} // not required, unless used
	template <class U>
	Allocator(Allocator<U> const&) noexcept {}

	value_type* // Use pointer if pointer is not a value_type*
	allocate(std::size_t n) {
		return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
	}

	void deallocate(value_type* p, std::size_t) noexcept // Use pointer if pointer is not a value_type*
	{
		::operator delete(p);
	}

	//     value_type*
	//     allocate(std::size_t n, const_void_pointer)
	//     {
	//         return allocate(n);
	//     }

	//     template <class U, class ...Args>
	//     void
	//     construct(U* p, Args&& ...args)
	//     {
	//         ::new(p) U(std::forward<Args>(args)...);
	//     }

	//     template <class U>
	//     void
	//     destroy(U* p) noexcept
	//     {
	//         p->~U();
	//     }

	//     std::size_t
	//     max_size() const noexcept
	//     {
	//         return std::numeric_limits<size_type>::max();
	//     }

	//     Allocator
	//     select_on_container_copy_construction() const
	//     {
	//         return *this;
	//     }

	//     using propagate_on_container_copy_assignment = std::false_type;
	//     using propagate_on_container_move_assignment = std::false_type;
	//     using propagate_on_container_swap            = std::false_type;
	//     using is_always_equal                        = std::is_empty<Allocator>;
};

// template <class T, class U>
// bool operator==(Allocator<T> const&, Allocator<U> const&) noexcept {
// 	return true;
// }

// template <class T, class U>
// bool operator!=(Allocator<T> const& x, Allocator<U> const& y) noexcept {
// 	return !(x == y);
// }
```

其中註解的部分是 `std::allocator_traits` 有預設版本的成員，所以如果沒有用到你可以把他們都刪了。 不過這篇文章是 2016 寫的，所以是比較以前的版本，但基本上這個東西沒什麼太大的更動，所以還是很好用的

因此你可以看到基本上你需要的實作部分有：

- `value_type`
- `allocate`
- `deallocate`
- 基本上預設建構子和複製建構子都要有，因為你寫的東西高機率會用到 rebind 相關的函式（真的用不到還是可以不寫）

下面是一個簡單的最簡（幾乎）自定義 Allocator：

```cpp
#include <iostream>
#include <memory> // for allocator_traits
#include <vector>

template <typename T>
struct MinimalAllocator {
	using value_type = T;

	MinimalAllocator() = default;

	template <typename U>
	MinimalAllocator(MinimalAllocator<U> const&) {}

	T* allocate(std::size_t n) {
		std::cout << "[MinimalAllocator] allocate " << n << " elements\n";
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}

	void deallocate(T* p, std::size_t n) {
		std::cout << "[MinimalAllocator] deallocate " << n << " elements\n";
		::operator delete(p);
	}
};

int main() {
	std::vector<int, MinimalAllocator<int>> v;
	v.push_back(42);
	v.push_back(99);

	std::cout << "v[0] = " << v[0] << ", v[1] = " << v[1] << '\n';
}
```

[輸出（`-std=c++11`）](https://godbolt.org/z/oPcqG5hxo)：

```
[MinimalAllocator] allocate 1 elements
[MinimalAllocator] allocate 2 elements
[MinimalAllocator] deallocate 1 elements
v[0] = 42, v[1] = 99
[MinimalAllocator] deallocate 2 elements
```

接下來我們就可以照這個邏輯來設計一下我們的 string stack allocator 了：

```cpp
#include <array>
#include <iostream>
#include <memory>
#include <string>

template <typename T, size_t PoolSize>
class StackAllocator {
  public:
	using value_type = T;

	StackAllocator() : pool{}, offset(0) {}

	template <typename U>
	StackAllocator(StackAllocator<U, PoolSize> const&) {}

	T* allocate(size_t n) {
		size_t bytes = n * sizeof(T);
		if (offset + bytes > PoolSize) { throw std::bad_alloc(); }

		T* ptr = reinterpret_cast<T*>(&pool[offset]);
		offset += bytes;
		return ptr;
	}

	void deallocate(T* p, size_t n) {
		// Stack memory does not need to be deallocated in this simple example.
		// Optionally, you could implement a reset mechanism to reuse memory.
	}

	template <typename U>
	struct rebind {
		using other = StackAllocator<U, PoolSize>;
	};

  private:
	alignas(T) std::array<unsigned char, PoolSize> pool;
	size_t offset;
};

// Comparison operators for the Allocator
template <typename T1, typename T2, size_t S1, size_t S2>
bool operator==(StackAllocator<T1, S1> const&, StackAllocator<T2, S2> const&) {
	return S1 == S2; // Same pool size
}

template <typename T1, typename T2, size_t S1, size_t S2>
bool operator!=(StackAllocator<T1, S1> const&, StackAllocator<T2, S2> const&) {
	return !(S1 == S2);
}

int main() {
	constexpr size_t PoolSize = 1024; // 1 KB stack pool

	// Create a string with custom allocator
	using CustomString = std::basic_string<char, std::char_traits<char>, StackAllocator<char, PoolSize>>;

	// Stack memory pool allocator
	StackAllocator<char, PoolSize> allocator;

	{
		// Create a string
		CustomString s("Hello, StackAllocator!", allocator);

		std::cout << "String: " << s << std::endl;

		// Modify the string
		s += " Nice to meet you!";
		std::cout << "Modified String: " << s << std::endl;
	}

	return 0;
}
```

[輸出（`-std=c++11`）](https://godbolt.org/z/76fh59T9r)：

```
String: Hello, StackAllocator!
Modified String: Hello, StackAllocator! Nice to meet you!
```

可以看到用法很簡單，基本上 AllocatorAwareContainer 的模板參數都有一個 Allocator 的欄位，例如 `std::vector`：

```cpp
template<
    class T,
    class Allocator = std::allocator<T>
> class vector;
```

而因為 `std::string` 是 `std::basic_string<char>` 的 type alias，因此我們要看 `std::basic_string` 的部分：

```cpp
template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>
> class basic_string;
```

可以看到也是有 Allocator 的部分

### `std::scoped_allocator_adaptor`

對於巢狀容器，例如 `std::vector<std::vector<int>>` 等，我們需要使用 `std::scoped_allocator_adaptor` 來讓 Allocator 能夠「自動向巢狀物件（如成員變數或巢狀容器）傳遞」

在沒有 `std::scoped_allocator_adaptor` 的情況下，讓我們模改一下上面的例子，假設想要弄個 `CustomString` 的 `std::vector`，需要這麼寫：

```cpp
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct SharedPool {
	std::array<unsigned char, 4 * 1024 * 1024> buffer;
	size_t offset = 0;
};

template <typename T>
class StackAllocator {
  public:
	using value_type = T;

	StackAllocator(SharedPool& pool_ref) : shared(&pool_ref) {}

	template <typename U>
	StackAllocator(StackAllocator<U> const& other) noexcept : shared(other.shared) {}

	T* allocate(size_t n) {
		size_t bytes = n * sizeof(T);
		if (shared->offset + bytes > shared->buffer.size()) { throw std::bad_alloc(); }

		T* ptr = reinterpret_cast<T*>(&shared->buffer[shared->offset]);
		shared->offset += bytes;
		return ptr;
	}

	void deallocate(T* p, size_t n) {}

	template <typename U>
	struct rebind {
		using other = StackAllocator<U>;
	};

	SharedPool* shared;
};

// Comparison operators for the Allocator
template <typename T1, typename T2>
bool operator==(StackAllocator<T1> const& a, StackAllocator<T2> const& b) {
	return a.shared == b.shared;
}

template <typename T1, typename T2>
bool operator!=(StackAllocator<T1> const& a, StackAllocator<T2> const& b) {
	return !(a == b);
}

int main() {
	SharedPool pool;

	using CustomString = std::basic_string<char, std::char_traits<char>, StackAllocator<char>>;
	using CustomVector = std::vector<CustomString, StackAllocator<CustomString>>;

	StackAllocator<char> allocator_str(pool);
	StackAllocator<CustomString> allocator_vec(pool);

	{
		CustomVector v(allocator_vec);

		v.push_back(CustomString("Hello, StackAllocator!", allocator_str));
		v.push_back(CustomString("Hello, StackAllocator2!", allocator_str));

		std::cout << v[0] << '\n' << v[1] << '\n';
	}
}
```

[輸出（`-std=c++11`）](https://godbolt.org/z/cnrTsrvTe)：

```
Hello, StackAllocator!
Hello, StackAllocator2!
```

但這樣寫很不方便，而且很容易手殘，例如不小心寫成：

```cpp
v.push_back(CustomString("Hello, StackAllocator2!"));
```

就會整個爛掉，此時就可以使用 `std::scoped_allocator_adaptor` 來讓 Allocator 能夠「自動向巢狀物件傳遞」，此時上方的例子會變為：

```cpp
using CustomString = std::basic_string<char, std::char_traits<char>, StackAllocator<char>>;
using CustomVector = std::vector<CustomString, std::scoped_allocator_adaptor<StackAllocator<CustomString>>>;

StackAllocator<char> allocator_str(pool);
StackAllocator<CustomString> allocator_vec(pool);

{
  CustomVector v(allocator_vec);

  v.push_back(CustomString("Hello!"));
  v.push_back(CustomString("Hello2!"));

  std::cout << v[0] << '\n' << v[1] << '\n';
}
```

在 `CustomVector` 處我們加上了 `std::scoped_allocator_adaptor`，此時 `std::vector` 的 Allocator 會自動被用來建構它的元素，即使被插入的物件 `CustomString("Hello!")` 和 `CustomString("Hello2!")` 並不是用相同的 Allocator 建構的（你可以看見不再需要 `allocator_str` 了）

另外，由於 basic_string 可以從 const char* 隱含地製造出來，所以最後那兩行可以進一步簡化為：

```cpp
v.push_back("Hello!");
v.push_back("Hello2!");
```

當 `std::vector` 要建構一個元素（比如 `push_back(obj)`）時，他會呼叫：

```cpp
std::allocator_traits<allocator_type>::construct(get_allocator(), void_ptr, obj);
```

通常這底層會是：

```cpp
::new (ptr) value_type(obj);
```

但是如果這個 Allocator 是 `std::scoped_allocator_adaptor<A>`，那它會用 template metaprogramming（像 `std::uses_allocator<T, Alloc>`）去檢查你要建構的 `value_type` 是不是可以接收 Allocator，如果不行，其會 fallback 為正常的建構行為，也就是：

```cpp
std::allocator_traits<outer_allocator_type>::construct(outer_allocator(), void_ptr, obj);
```

如剛剛所說，這通常底層會是個 placement new

而如果可以接收 Allocator，`std::scoped_allocator_adaptor` 就會自動在建構子中注入 Allocator，形式會像是：

```cpp
std::allocator_traits<outer_allocator_type>::construct(outer_allocator(), void_ptr, obj, inner_allocator());
```

或是：

```cpp
std::allocator_traits<outer_allocator_type>::construct(outer_allocator(), void_ptr, std::allocator_arg, inner_allocator(), obj);
```

換句話說，Adaptor 會於其嵌套的 Allocator 呼叫 `construct()` 時會傳送額外的參數，因此物件才也會以 Allocator 來建構

上例中的 `inner_allocator_type` 會是 `std::scoped_allocator_adaptor` 的另一個特化，所以如果元素類型也是容器，它就會使用相同的方法來建構它的元素，而且分配器可以被下傳到每個元素，以滿足你有容器的容器的容器等需求

## PMR

### Allocator 的不完美之處

如果你有看完前面的例子，你一定會覺得有個東西很麻煩，那就是你每用一個 Allocator，就會多一個 Static Type，舉個例子，假設有以下 `std::vector`：

```cpp
auto my_vector = std::vector<int,my_allocator>();
```

此時這一整個 `std::vector<int,my_allocator>` 是他的 Static Type，而如果我們又用了另一個 `std::vector`，這時候會發生一個問題：

```cpp
auto my_vector = std::vector<int,my_allocator>();
auto my_vector2 = std::vector<int,other_allocator>();
auto vec = my_vector; // ok
vec = my_vector2; // error
```

因為兩個 `std::vector` 的 Static Type 不同，所以 `operator=` 自然就不能用了

再來，因為 Allocator 是在編譯期決定的，如果你想換一種配置策略，例如從 malloc 換成 memory pool，那你必須要把整個容器的型態改掉

### PMR 的解答

在 C++17 時為了解決這個問題引入了 PMR，為 Polymorphic Memory Resource 的縮寫，它會利用 Runtime 的 Polymorphism 來做到相同的 Static Type，但卻能在執行時做到類似用了不同的 Allocator 一樣的效果。 也因此他不是透過 template 在做，他是單一的 Static Type，通過 dynamic dispatch 來定義 Allocator 的行為，以達到我們的目的

他仍然是個 Allocator，你完全可以自己做出 PMR 的行為，用了之後 STL 容器底層也還是一樣依賴於 `std::allocator_traits` 已擁有的實作。 因此基本邏輯於上方一樣，要寫一個自定義的 Allocator，只是要用動態多型來做，實作上這通常會利用 virtual function 來做（應該很好猜?），以 PMR 來說，其還多弄了一個中間層 `std::pmr::memory_resource`，他是一個 virtual base class，PMR 當中衍生出的 Allocator 全部都基於它，其在 llvm frontend 的實作很簡單（[連結](https://github.com/llvm/llvm-project/blob/main/libcxx/include/__memory_resource/memory_resource.h)）：

```cpp
class _LIBCPP_AVAILABILITY_PMR _LIBCPP_EXPORTED_FROM_ABI memory_resource {
  static const size_t __max_align = alignof(max_align_t);

public:
  virtual ~memory_resource();

  [[nodiscard]] [[using __gnu__: __returns_nonnull__, __alloc_size__(2), __alloc_align__(3)]]
  _LIBCPP_HIDE_FROM_ABI void* allocate(size_t __bytes, size_t __align = __max_align) {
    return do_allocate(__bytes, __align);
  }

  [[__gnu__::__nonnull__]] _LIBCPP_HIDE_FROM_ABI void
  deallocate(void* __p, size_t __bytes, size_t __align = __max_align) {
    do_deallocate(__p, __bytes, __align);
  }

  _LIBCPP_HIDE_FROM_ABI bool is_equal(const memory_resource& __other) const noexcept { return do_is_equal(__other); }

private:
  virtual void* do_allocate(size_t, size_t)                       = 0;
  virtual void do_deallocate(void*, size_t, size_t)               = 0;
  virtual bool do_is_equal(memory_resource const&) const noexcept = 0;
};
```

可以看到 `std::pmr::memory_resource` 定義了三個 private 的虛擬函式與三個 public 成員函式，與 cppreference 上列出的完全一致（[連結](https://en.cppreference.com/w/cpp/memory/memory_resource)），但要注意它本身不是 Allocator，他沒有符合 Allocator 的要求，例如它並沒有定義 `value_type`

而標準 PMR 中自定義的 Allocator 叫做 `std::pmr::polymorphic_allocator<T>`，其繼承自 `std::pmr::memory_resource`，你可以看到一樣有模板，這是因為 PMR 是讓記憶體資源實現動態多型，而容器的型態仍然依賴 template allocator 來實例化

因為它是自定義的 Allocator，所以自然滿足剛剛要求的基本實作：

- `value_type`
- `allocate`
- `deallocate`
- 預設建構子和複製建構子

其他還定義了大大小小的東西，可以到 [cppreference](https://en.cppreference.com/w/cpp/memory/polymorphic_allocator) 上看

而與之前 Allocator 不同的是，原本我們都是在 Allocator 的 `allocate` 函式內操作記憶體，可能會用 placement new 或其他手段操作已經開好的 memory pool。 PMR 將這個步驟利用中間層 `memory_resource` 分了出去，`memory_resource` 負責主要的記憶體操作，而 `std::pmr::polymorphic_allocator` 雖然是一個完全符合 STL allocator requirement 的 Allocator，但它本身不負責實際記憶體分配，而是把所有分配/釋放的責任「委託」給指定的 `std::pmr::memory_resource`

換句話說，`polymorphic_allocator` 內的 `allocate` 會去直接/間接呼叫 `memory_resource->allocate`，其裡面會再去呼叫 `do_allocate`，這是一個純虛擬函式，每個子類都需要實作，利用這個達到動態多型的效果。 在實作上通常還會有個型態為 `memory_resource*` 的指標指向實際使用的 `memory_resource`，幫助 `polymorphic_allocator` 與 `memory_resource` 溝通

:::info  
可以搭配上方 llvm frontend 的例子觀看，有實際的 code 應該一看就懂

另外，對於記憶體資源不依賴執行時變數的簡單情況，好的編譯器會將記憶體資源 devirtualize，最後就會有一個沒有額外成本的 `polymorphic_allocator`（除了儲存指標的成本，很低XD）  
:::


下面是一個利用 `uftrace` 生的 call graph 範例，用來釐清呼叫流程：

```cpp
#include <iostream>
#include <memory_resource>
#include <vector>

int main() {
	std::byte buffer[1024];
	std::pmr::monotonic_buffer_resource pool(buffer, sizeof(buffer));
	std::pmr::vector<int> vec(&pool);

	vec.reserve(10);	  // 呼叫 allocate
	vec.emplace_back(42); // 呼叫 construct
}
```

對應的 `uftrace` 輸出：

```
...
1.903 us :  |  +-(1) std::vector::_M_allocate_and_copy
1.001 us :  |  |  +-(1) std::_Vector_base::_M_allocate
0.855 us :  |  |  | (1) std::allocator_traits::allocate
0.606 us :  |  |  | (1) std::pmr::polymorphic_allocator::allocate
0.460 us :  |  |  | (1) std::pmr::memory_resource::allocate
0.048 us :  |  |  | (1) operator new
...
```

### 簡單的小實作

至此，我們可以試著做一個簡化版的 `polymorphic_allocator` 來看看原理：

```cpp
#include <cstddef>
#include <iostream>
#include <memory_resource>
#include <new>
#include <vector>

template <typename T>
class MiniPolymorphicAllocator {
  public:
	using value_type = T;

	MiniPolymorphicAllocator(std::pmr::memory_resource* r = std::pmr::get_default_resource()) noexcept : res(r) {}

	template <typename U>
	MiniPolymorphicAllocator(MiniPolymorphicAllocator<U> const& other) noexcept : res(other.resource()) {}

	T* allocate(std::size_t n) { return static_cast<T*>(res->allocate(n * sizeof(T), alignof(T))); }

	void deallocate(T* p, std::size_t n) { res->deallocate(p, n * sizeof(T), alignof(T)); }

	std::pmr::memory_resource* resource() const { return res; }

  private:
	std::pmr::memory_resource* res;

	template <typename U>
	friend class MiniPolymorphicAllocator; // To make different static type can access each other
};

int main() {
	std::byte buffer[1024];
	std::pmr::monotonic_buffer_resource pool(buffer, sizeof(buffer));

	MiniPolymorphicAllocator<int> alloc(&pool);
	std::vector<int, MiniPolymorphicAllocator<int>> vec(alloc);

	vec.push_back(42);
	std::cout << vec[0] << '\n'; // output 42
}
```

[輸出（`-std=c++17`）](https://godbolt.org/)：

```
42
```

上例中我們自定義的 Allocator 為 `MiniPolymorphicAllocator`，`memory_resource` 為 `monotonic_buffer_resource`。 在 `MiniPolymorphicAllocator` 的 `allocate` 中我們直接利用指向 `memory_resource` 本體的指標 `res` 來呼叫真正的 `allocate`。 雖然少做了很多東西，但最基礎的記憶體配置流程是一樣的

在 llvm fronted 內的實作也長得幾乎一模一樣（[連結](https://github.com/llvm/llvm-project/blob/main/libcxx/include/__memory_resource/polymorphic_allocator.h#L65)）：

```cpp
[[nodiscard]] _LIBCPP_HIDE_FROM_ABI _ValueType* allocate(size_t __n) {
  if (__n > __max_size()) {
    std::__throw_bad_array_new_length();
  }
  return static_cast<_ValueType*>(__res_->allocate(__n * sizeof(_ValueType), alignof(_ValueType)));
}
```

### PMR Usage

粗略的介紹完整體的思想後我們就來看看該如何使用吧，主要可以分為兩個部分 ― Allocator 與 memory resource

#### memory resource

如同前面講的 `memory_resource` 是實際在操控記憶體的 class，在 PMR lib 中它提供了五個 `memory_resource`：

- `new_delete_resource()`：回傳一個調用 `new` 和 `delete` 的記憶體資源的指標
- `synchronized_pool_resource`：一個更少碎片化、Thread Safe 的 `memory_resource`
- `unsynchronized_pool_resource`：一個更少碎片化、但 Thread Unsafe 的 `memory_resource`
- `monotonic_buffer_resource`：一個從不釋放、可以傳遞一個可選的緩衝區、Thread Unsafe 的 `memory_resource`
- `null_memory_resource()`：傳回一個每次分配都會失敗的記憶體資源的指標

我們就從第一個開始開始看

##### `new_delete_resource()`

`new_delete_resource` 是預設的 `memory_resource`，也是 `get_default_resource` 的回傳值。 `new_delete_resource`和最一般在配置記憶體的方法一樣：

- `allocate()` 函式使用 `::operator new` 來分配記憶體
- `deallocate()` 函式使用 `::operator delete` 來刪除記憶體
- 對於任何 `memory_resource` r ，`p->is_equal(r)` 會返回 `&r == p`

它底層做的事基本上就是像這樣：

```cpp
void* do_allocate(size_t bytes, size_t alignment) override {
  return ::operator new(bytes, std::align_val_t{alignment});
}
```

也就是直接呼叫標準的 global placement new，因此會有 heap allocation

簡單的例子：

```cpp
std::pmr::memory_resource* r = std::pmr::new_delete_resource();

// 分配 3 個 int（共 12 bytes，對齊 4）
void* p = r->allocate(3 * sizeof(int), alignof(int));

std::cout << "memory allocated at: " << p << '\n';

// 回收記憶體
r->deallocate(p, 3 * sizeof(int), alignof(int));
```

##### `synchronized_pool_resource` 與 `unsynchronized_pool_resource`

- `(un)synchronized_pool_resource` 會嘗試在相鄰位置分配所有 `memory_resource` class，以減緩碎片化
- `(un)synchronized_pool_resource` 會將記憶體分成多個 `fixed-size pool`，並依據分配大小自動分類到不同 bucket，每個 bucket 對應一種 block size（例如 8B、16B、32B...）
- 兩者的差距主要在 Thread Safety，因此 `synchronized_pool_resource` 的效能會比較差一點
- 當你反覆分配、釋放相同大小的記憶體時，它會重複使用 memory pool 中已釋放的區塊，因此能進一步有效減緩 heap 的碎片化問題
- 若請求的大小超過某個閾值（如 4KB 以上），則會直接轉交給 upstream_resource（預設是 `new_delete_resource()`）

簡單的例子：

```cpp
std::pmr::unsynchronized_pool_resource pool;
std::pmr::polymorphic_allocator<int> alloc(&pool);

int* a = alloc.allocate(10); // 分配 10 * sizeof(int)
alloc.deallocate(a, 10);     // 回收給 pool

int* b = alloc.allocate(10); // 可能會從 pool 中的 free list 重複使用 a
```

##### `monotonic_buffer_resource`

- 以「單向遞增」的方式配置記憶體，一但分配出去，就永遠不會釋放回個別使用者，只有在整個 resource 被銷毀時才會一次釋放（這種 Allocator 有個別名叫 Bump Allocator）
  - 因此非常快，其 `deallocate` 實際上什麼都不會做
- Thread Unsafe
- 支援自動向 upstream memory resource 要更多記憶體（預設是 `new_delete_resource`）

簡單的例子：

```cpp
std::byte buffer[1024]; // 自訂固定大小的緩衝區

std::pmr::monotonic_buffer_resource pool(buffer, sizeof(buffer));

std::pmr::vector<int> vec(&pool); // 使用此 pool 作為 allocator
for (int i = 0; i < 10; ++i) vec.push_back(i * 10);

for (int i : vec) std::cout << i << " ";
std::cout << '\n';

// 不需要手動釋放 vec 的記憶體，pool 的記憶體會在作用域結束時整體釋放
```

##### `null_memory_resource()`

- 會使每一次記憶體配置都拋出 `bad_alloc` 異常
- 最主要的應用在於確保使用 stack 上的 memory pool 時不會突然意外在 heap 上分配額外的記憶體

下面是個來自 C++17 Complete Guide 的例子：

```cpp
#include <array>
#include <cstddef> // for std::byte
#include <iostream>
#include <memory_resource>
#include <string>
#include <unordered_map>

int main() {
	std::array<std::byte, 200000> buf; // stack memory pool
	std::pmr::monotonic_buffer_resource pool{buf.data(), buf.size(), std::pmr::null_memory_resource()};

	std::pmr::unordered_map<long, std::pmr::string> map{&pool}; // 嘗試分配過多的記憶體
	try {
		for (int i = 0; i < buf.size(); ++i) {
			std::string s{"Customer" + std::to_string(i)};
			map.emplace(i, s);
		}
	} 
  catch (std::bad_alloc const& e) { 
    std::cerr << "BAD ALLOC EXCEPTION: " << e.what() << '\n'; 
  }

	std::cout << "size: " << map.size() << '\n';
}
```

透過傳遞 `null_memory_resource()` 作為備選記憶體資源，我們可以確保任何嘗試分配過多記憶體的行為都會拋出異常，而不是在 heap 上分配記憶體

#### Allocator（`polymorphic_allocator`）

前面已經看過很多 use case 了，我們可以再來多看幾個，熟悉一下怎麼用：

```cpp
std::array<std::byte, 200000> buf;

std::pmr::monotonic_buffer_resource pool{buf.data(), buf.size()};
std::pmr::vector<std::string> coll{&pool};

for (int i = 0; i < 1000; ++i)
  coll.emplace_back("just a non-SSO string"); 
```

可以看到使用方法非常簡單，用 array 開個 memory pool，綁定給 `memory_resource`，在餵給 `std::pmr::vector` 就好

其中 `std::pmr::vector` 是下面的縮寫

```cpp
std::vector<std::string, std::pmr::polymorphic_allocator<std::string>> coll{&pool};
```

## References

- [Jason Turner 的影片](https://www.youtube.com/playlist?list=PLs3KjaCtOwSYX3X0L36NgwK0pxZZavDSF)
- [C++17 Complete Guide](https://www.cppstd17.com/)
- [Value Proposition: Allocator-Aware (AA) Software - John Lakos - CppCon 2019](https://www.youtube.com/watch?v=ebn1C-mTFVk)
- [CppCon 2017: Pablo Halpern “Allocators: The Good Parts”](https://www.youtube.com/watch?v=v3dz-AKOVL8)
- [CppCon 2016: Herb Sutter “Leak-Freedom in C++... By Default.”](https://www.youtube.com/watch?v=JfmTagWcqoE)
- [Local (Arena) Memory Allocators Part 1 - John Lakos - Meeting C++ 2017](https://www.youtube.com/watch?v=ko6uyw0C8r0)
- [Local (Arena) Allocators Part II - John Lakos - Meeting C++ 2017](https://www.youtube.com/watch?v=fN7nVzbRiEk)
- [N3916](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3916.pdf)
- [N1850](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1850.pdf)
- [What's the advantage of using std::allocator instead of new in C++?](https://stackoverflow.com/questions/31358804/whats-the-advantage-of-using-stdallocator-instead-of-new-in-c)
- [What is the purpose and usage of `memory_resource`?](https://stackoverflow.com/questions/44799321/what-is-the-purpose-and-usage-of-memory-resource)
- [polymorphic_allocator: when and why should I use it?](https://stackoverflow.com/questions/38010544/polymorphic-allocator-when-and-why-should-i-use-it)
- [Why is std::is_pod deprecated in C++20?](https://stackoverflow.com/questions/48225673/why-is-stdis-pod-deprecated-in-c20)
- [STL 之父訪談錄](https://web.archive.org/web/20140307111122fw_/http://jjhou.boolan.com/myan-alexander.htm)
- [An Interview with A. Stepanov by Graziano Lo Russo](http://www.stlport.org/resources/StepanovUSA.html)
- [第10篇:C++ 堆記憶體管理器-allocator](https://zhuanlan.zhihu.com/p/185611161)
- [Allocators; one of the ignored souls of STL](https://www.reddit.com/r/cpp/comments/14lf0mb/allocators_one_of_the_ignored_souls_of_stl/)
- [Allocator (C++) Wikipedia](https://en.wikipedia.org/wiki/Allocator_%28C%2B%2B%29)
- [How to explicitly call a namespace-qualified destructor?](https://stackoverflow.com/questions/24593942/how-to-explicitly-call-a-namespace-qualified-destructor)
- [Why isn't there a std::construct_at in C++17?](https://stackoverflow.com/questions/52966560/why-isnt-there-a-stdconstruct-at-in-c17)
- [Why are are std::allocator's construct and destroy functions deprecated in c++17?](https://stackoverflow.com/questions/39414610/why-are-are-stdallocators-construct-and-destroy-functions-deprecated-in-c17)
- [P0784R5 More constexpr containers](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0784r5.html)
- [What is the purpose of std::scoped_allocator_adaptor?](https://stackoverflow.com/questions/22148258/what-is-the-purpose-of-stdscoped-allocator-adaptor)
- [What is the purpose of allocator_traits\<T\> in C++0x?](https://stackoverflow.com/questions/4502691/what-is-the-purpose-of-allocator-traitst-in-c0x)
- [Should every fancy pointer be an iterator?](https://stackoverflow.com/questions/45132546/should-every-fancy-pointer-be-an-iterator)