---
title: C++ 教學系列 ── Windows 內的 C++ 與 VSCode 環境建置
date: 2022-02-24
tag: C++ Miner-tutorial
category: C++ Miner
---

<h1><div class = "center-column"><img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50> C++ 教學系列 <img src = "https://i.imgur.com/thmVmX6.png?w=1000" height = 50><br>Windows 內的 C++ 與 VSCode 環境建置</div></h1>  

礦坑系列首頁：<strong><a href = "https://github.com/Mes0903/Cpp-Miner/tree/hackmd" class = "redlink">首頁</a></strong>

hackmd 版首頁：<strong><a href = "https://hackmd.io/@Mes/Cpp_Miner/https%3A%2F%2Fhackmd.io%2F%40Mes%2FPreface" class = "redlink">首頁</a></strong>

## 前言  

這篇文章以 windows 為例，教大家如何建置 windows 上能寫 C++ 的環境。文字編輯器的部分我使用 Vscode，因為比較方便，擴充功能也挺多的，但這系列應該只會用到最基本的功能，擴充功能的部分大家可以自己多多摸索。  

為了在 windows 上寫 C++，我們會需要兩個東西  

1. 文字編輯器  

    本系列使用 Vscode，如果你有其他的選項當然也可以，自己習慣就好，甚至是記事本也 ok，或是你要用 IDE 也可以  

2. 編譯器  

    編譯器是負責幫我們把 C++ code 翻譯給電腦看的軟體，本文使用 Mingw-gcc，如果你有其他選項也可以，一樣，習慣就好  

之後的教學我可能是在 Linux 環境下撰寫的，但考慮到大多數人的環境可能還是 windows，所以寫了這篇讓大家能夠跟著建置可用的環境。  

## Vscode 安裝  

首先到 Vscode 官網的[下載頁面](https://code.visualstudio.com/download)下載自己對應的系統版本，這邊我們選 Windows  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/download1.png"><br>  

下載後開啟，並同意合約，然後下一步  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/download2.png"><br>  

這邊除了最後一個要勾之外其他的都看個人喜好，我自己是全勾，然後下一步並確認安裝，先不用重開機，後面裝完編譯器再重開就好。  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/download3.png"><br>  

安裝好後開啟 Vscode，進到 Extension(圖片標示 1 處)，搜尋 `C++` (圖片標示 2 處)，然後安裝，我這邊是已經安裝好的畫面，各位左邊那條可能不會那麼多東西，數量不一樣是正常的，不用擔心。  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/download4.png"><br>  

如果看不習慣英文的可以下載中文模組，在剛剛那個搜尋欄裡面搜尋 Chinese，然後安裝：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/download5.png"><br>  

## 安裝 Mingw-gcc  

本文使用的是 winlibs 提供的 Mingw-w64 GCC，你可以直接點[這裡](https://github.com/brechtsanders/winlibs_mingw/releases/download/11.2.0-9.0.0-ucrt-r5/winlibs-x86_64-posix-seh-gcc-11.2.0-mingw-w64ucrt-9.0.0-r5.7z)下載，或是進到 [winlibs](https://winlibs.com/) 的網站，在下方選擇你想要的版本，我上方連結給的版本是下圖紅圈圈起來的版本：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/mingw1.png"><br>  

下載好後解壓縮，解壓縮出來的資料夾裡面會有一個叫 mingw 的資料夾：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/mingw2.png"><br>  

把這個資料夾移到 C 槽或你想要的地方：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/mingw3.png"><br>  

之後在 windows 搜尋「編輯系統環境變數」，點進去：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/mingw4.png"><br>  

在進階裡面有一個環境變數，點進去：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/mingw5.png"><br>  

會發現下方系統變數的地方有一個 `Path`，連點它兩下，或選取後按編輯：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/mingw6.png"><br>  

點擊右方的新增，把剛剛的 mingw 下的 `bin` 資料夾位置貼進去：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/mingw7.png"><br>  

之後按確定出來，<strong><span class = "yellow">重開機</span></strong>。  

## 測試第一個 C++ 程式  

開啟 Vscode，選取左上方的檔案，然後選取開啟資料夾  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp1.png"><br>  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp2.png"><br>  

然後創建一個資料夾，這個資料夾拿來放你要寫的 cpp 檔，名稱盡量用英文的比較好  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp3.png"><br>  

然後選取這個資料夾開啟，開啟後 Vscode 可能會問你是否要信任這個資料夾的作者，選是  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp4.png"><br>  

此時你的畫面會長這樣，資料夾名稱的右邊有四個按鈕：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp5.png"><br>  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp6.png"><br>  

這四個按鈕的功能由左至右分別為新增檔案、新增資料夾、重新整理和摺疊資料夾。  

我們選取新增檔案，檔名取為 `test.cpp`，`.cpp` 代表這是一個 C++ 的檔案，test 則是檔名，你可以取你喜歡的，但一樣檔名盡量取英文的：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp7.png"><br>  

此時你的資料夾裡面應該長這樣：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp8.png"><br>  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp9.png"><br>  

在 Vscode 裡面點選剛剛創好的檔案，此時右邊會出現讓你編輯文字的畫面，輸入以下這段文字：  

```cpp
#include <iostream>  

int main()  
{  
    std::cout << "test";  
    return 0;  
}  
```

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp10.png"><br>  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp11.png"><br>  

按 ctrl+s 存檔後按 F5，此時會跳出一個選單，選取「C++ (GDB/LLDB)」：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp12.png"><br>  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp13.png"><br>  

然後選取 「g++.exe - 建置及偵錯使用中的檔案」，後面的檔案位址要確認一下是不是剛剛安裝的 mingw：  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp14.png"><br>  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp15.png"><br>  

此時你的資料夾裡面會多一個叫做 `.vscode` 的資料夾，這個是 Vscode 的設定檔，然後右方(或下方)可能會跳出一個視窗，視窗上方由左至右分別為問題、輸出、偵錯主控台和終端機，這邊點選終端機，就可以看到剛剛程式執行的結果了  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp16.png"><br>  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp17.png"><br>  

<img src = "https://raw.githubusercontent.com/Mes0903/Cpp-Miner/refs/heads/standard-markdown/Miner_Tutorial/Environment_Building/image/test-cpp18.png"><br>  

這樣一來你的環境就建好了，好好享受 C++ ㄅ

## 手動建立設定檔

如果按下 F5 後你的資料夾內沒有自動建立 `.vscode` 的資料夾，導致程式跑不起來，那你就需要自己手動建立這些設定了

首先在你的專案資料夾內新增名叫 `.vscode` 的資料夾，接著將建立兩個檔案 `launch.json` 與 `tasks.json`，內容分別如下：

- `launch.json`
    ```json
    {
      "configurations": [
        {
          "name": "C/C++: g++.exe 建置及偵錯使用中的檔案",
          "type": "cppdbg",
          "request": "launch",
          "program": "${fileDirname}\\${fileBasenameNoExtension}.exe",
          "args": [],
          "stopAtEntry": false,
          "cwd": "${fileDirname}",
          "environment": [],
          "externalConsole": false,
          "MIMode": "gdb",
          "miDebuggerPath": "C:\\mingw64\\bin\\gdb.exe",
          "setupCommands": [
            {
              "description": "啟用 gdb 的美化顯示",
              "text": "-enable-pretty-printing",
              "ignoreFailures": true
            },
            {
              "description": "將反組譯碼變體設為 Intel",
              "text": "-gdb-set disassembly-flavor intel",
              "ignoreFailures": true
            }
          ],
          "preLaunchTask": "C/C++: g++.exe 建置使用中檔案"
        }
      ],
      "version": "2.0.0"
    }
    ```
- `tasks.json`
    ```json
    {
      "tasks": [
        {
          "type": "cppbuild",
          "label": "C/C++: g++.exe 建置使用中檔案",
          "command": "C:\\mingw64\\bin\\g++.exe",
          "args": [
            "-fdiagnostics-color=always",
            "-g",
            "${file}",
            "-o",
            "${fileDirname}\\${fileBasenameNoExtension}.exe"
          ],
          "options": {
            "cwd": "${fileDirname}"
          },
          "problemMatcher": [
            "$gcc"
          ],
          "group": {
            "kind": "build",
            "isDefault": true
          },
          "detail": "偵錯工具產生的工作。"
        }
      ],
      "version": "2.0.0"
    }
    ```

要注意的事情有兩點

1. 這份設定檔是給 C++ 用的
2. 在兩份檔案中的 `"C:\\mingw64\\bin\\"` 需要改成你 mingw 安裝的位置
