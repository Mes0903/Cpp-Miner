---
title: (WIP) 礦坑系列 ── Allocator & PMR
date: 2025-04-12
tag: C++ Miner-main
category: C++ Miner
---

<h1><div class = "center-column"><img src = "https://i.imgur.com/thmVmX6.png?" height = 50> 礦坑系列 <img src = "https://i.imgur.com/thmVmX6.png?" height = 50><br>Allocator & PMR</div></h1>

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
另外，我也不確定是不是所有的編譯器實作中 Allocator 底層都會呼叫到 `new`，也許有些編譯器會直接跳過 new 用 malloc（純猜測），這待商榷  
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

template <class T, class U>
bool operator==(Allocator<T> const&, Allocator<U> const&) noexcept {
	return true;
}

template <class T, class U>
bool operator!=(Allocator<T> const& x, Allocator<U> const& y) noexcept {
	return !(x == y);
}
```

其中註解的部分是 `std::allocator_traits` 有預設版本的成員，所以如果沒有用到你可以把他們都刪了。 不過這篇文章是 2016 寫的，所以是比較以前的版本，但基本上這個東西沒什麼太大的更動，所以還是很好用的

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