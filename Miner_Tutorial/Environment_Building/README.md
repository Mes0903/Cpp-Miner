<h1><center><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++ 教學系列 Windows 內的 C++ 與 Vscode 環境建置 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50></center></h1>

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

# 前言

這篇文章以 windows 為例，教大家如何建置 windows 上能寫 C++ 的環境。文字編輯器的部分我使用 Vscode，因為比較方便，擴充功能也挺多的，但這系列應該只會用到最基本的功能，擴充功能的部分大家可以自己多多摸索。

為了在 windows 上寫 C++，我們會需要兩個東西

1. 文字編輯器
    
    本系列使用 Vscode，如果你有其他的選項當然也可以，自己習慣就好，甚至是記事本也 ok，或是你要用 IDE 也可以

2. 編譯器

    編譯器是負責幫我們把 C++ code 翻譯給電腦看的軟體，本文使用 Mingw-gcc，如果你有其他選項也可以，一樣，習慣就好
    
之後的教學我可能是在 Linux 環境下撰寫的，但考慮到大多數人的環境可能還是 windows，所以寫了這篇讓大家能夠跟著建置可用的環境。

# Vscode 安裝

首先到 Vscode 官網的[下載頁面](https://code.visualstudio.com/download)下載自己對應的系統版本，這邊我們選 Windows

![](https://i.imgur.com/56JpJz7.png)

下載後開啟，並同意合約，然後下一步

![](https://i.imgur.com/QD5XzvG.png)

這邊除了最後一個要勾之外其他的都看個人喜好，我自己是全勾，然後下一步並確認安裝，先不用重開機，後面裝完編譯器再重開就好。

![](https://i.imgur.com/nsJWPVE.png)

安裝好後開啟 Vscode，進到 Extension(圖片標示 1 處)，搜尋 `C++` (圖片標示 2 處)，然後安裝，我這邊是已經安裝好的畫面，各位左邊那條可能不會那麼多東西，數量不一樣是正常的，不用擔心。

![](https://i.imgur.com/fqfjlXi.png)

如果看不習慣英文的可以下載中文模組，在剛剛那個搜尋欄裡面搜尋 Chinese，然後安裝：

![](https://i.imgur.com/nvssG0p.png)

# 安裝 Mingw-gcc

本文使用的是 winlibs 提供的 Mingw-w64 GCC，你可以直接點[這裡](https://github.com/brechtsanders/winlibs_mingw/releases/download/11.2.0-9.0.0-ucrt-r5/winlibs-x86_64-posix-seh-gcc-11.2.0-mingw-w64ucrt-9.0.0-r5.7z)下載，或是進到 [winlibs](https://winlibs.com/) 的網站，在下方選擇你想要的版本，我上方連結給的版本是下圖紅圈圈起來的版本：

![](https://i.imgur.com/7BgII7U.png)

下載好後解壓縮，解壓縮出來的資料夾裡面會有一個叫 mingw 的資料夾：

![](https://i.imgur.com/qTV67LN.png)

把這個資料夾移到 C 槽或你想要的地方：

![](https://i.imgur.com/BgbgUDi.png)

之後在 windows 搜尋「編輯系統環境變數」，點進去：

![](https://i.imgur.com/w8yfayQ.png)

在進階裡面有一個環境變數，點進去：

![](https://i.imgur.com/pgMTKuw.png)

會發現下方系統變數的地方有一個 `Path`，連點它兩下，或選取後按編輯：

![](https://i.imgur.com/ptIGjnL.png)

點擊右方的新增，把剛剛的 mingw 下的 `bin` 資料夾位置貼進去：

![](https://i.imgur.com/VLd7vOi.png)

之後按確定出來，<strong><span class = "yellow">重開機</span></strong>。

# 測試第一個 C++ 程式

開啟 Vscode，選取左上方的檔案，然後選取開啟資料夾

![](https://i.imgur.com/xJd0BNT.png)

![](https://i.imgur.com/aKxOwxu.png)

然後創建一個資料夾，這個資料夾拿來放你要寫的 cpp 檔，名稱盡量用英文的比較好

![](https://i.imgur.com/eC9x6wG.png)

然後選取這個資料夾開啟，開啟後 Vscode 可能會問你是否要信任這個資料夾的作者，選是

![](https://i.imgur.com/K7E8Vai.png)

此時你的畫面會長這樣，資料夾名稱的右邊有四個按鈕：

![](https://i.imgur.com/cSabeng.png)

![](https://i.imgur.com/oqp9k55.png)

這四個按鈕的功能由左至右分別為新增檔案、新增資料夾、重新整理和摺疊資料夾。

我們選取新增檔案，檔名取為 `test.cpp`，`.cpp` 代表這是一個 C++ 的檔案，test 則是檔名，你可以取你喜歡的，但一樣檔名盡量取英文的：

![](https://i.imgur.com/RlMEPwt.png)

此時你的資料夾裡面應該長這樣：

![](https://i.imgur.com/3pXrpSs.png)

![](https://i.imgur.com/EzGA3D9.png)

在 Vscode 裡面點選剛剛創好的檔案，此時右邊會出現讓你編輯文字的畫面，輸入以下這段文字：

```cpp
#include <iostream>

int main()
{
    std::cout << "test";
    return 0;
}
```

![](https://i.imgur.com/trtTKqf.png)

![](https://i.imgur.com/FS4ehKL.png)

按 ctrl+s 存檔後按 F5，此時會跳出一個選單，選取「C++ (GDB/LLDB)」：

![](https://i.imgur.com/n0YekM9.png)

![](https://i.imgur.com/eWxGvYW.png)


然後選取 「g++.exe - 建置及偵錯使用中的檔案」，後面的檔案位址要確認一下是不是剛剛安裝的 mingw：

![](https://i.imgur.com/bHFNHMM.png)

![](https://i.imgur.com/b0GDfWb.png)

此時你的資料夾裡面會多一個叫做 `.vscode` 的資料夾，這個是 Vscode 的設定檔，然後右方(或下方)可能會跳出一個視窗，視窗上方由左至右分別為問題、輸出、偵錯主控台和終端機，這邊點選終端機，就可以看到剛剛程式執行的結果了

![](https://i.imgur.com/zDhGEbK.png)

![](https://i.imgur.com/m4TrOQm.png)

![](https://i.imgur.com/oLBBwCM.png)

這樣一來你的環境就建好了，好好享受 C++ ㄅ