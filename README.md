# 注意！

## 项目前置

> 本项目使用了OpenMP进行并行化加速，默认开启，OpenMP在C++编译器中默认都装了的，请使用较新的编译器
> 在win平台上似乎无法使用mingw对OpenMP加速，但是Visual Studio（MSVC） 和 Clang 在win平台上都是可以编译的
> 不要使用Mingw编译，win上可以使用Clang,VS(MSVC)，linux上使用gcc(g++)即可

## 块内存大小默认4G，如有更多请更改

```cpp
//原理是这里定义处理函数，将函数传入dna::open_file_and_calculate，在open_file_and_calculate中会调用传入的函数
//参数列表 <文件分块内存大小，单个DNA序列最长大小>("输入文件名","输出文件名",序列处理函数);
//这个函数在src/tools/dna里面
dna::open_file_and_calculate<(size_t)4 * 1024 * 1024 *1024 , (size_t)5e4+5>("filteredReads.txt", "reversedSequence.txt",reverseComplement);
```

## ***请详细阅读xmake.lua项目配置文件，可能涉及到性能优化和计算精度的问题***

> 最好不要使用mingw，使用mingw+clang(就是clang)或者msvc(visual studio)
>
> mingw的IO优化不行

# DNASequence

[提问者问题原文链接](https://www.zhihu.com/question/36143261/answer/3624848144)

## 代码逻辑介绍

DNASequence处理

DNA是双链的，互为互补链，对DNA样本进行测序时不能确认测出的是哪条链，所以就把所有DNA片段的互补链全算出来，和原文件放在一起组装。

> 输入格式：只是演示样例，不保证其生物上的准确性，默认最大dna序列长度支持5e4，可自行修改代码扩容
>
> 程序将会从项目的根目录中打开filteredReads.txt并处理类似以下若干条dna序列

```
@SRR13280199.1 1 length=32
ACGTACACATTGCTGTCTGCTGAACCACCTAG
@SRR13280199.1 2 length=32
ACGTACACATTGCTGTCTGCTGAACCACCTAG
```

## 关于如何构建本项目

> 请确保安装了构建工具xmake，和任意C++构建工具并将路径添加到了PATH目录

[如何安装xmake?链接](https://gitee.com/tboox/xmake#%E5%AE%89%E8%A3%85)

```bash
#编译 -v 表示 verbose 输出详细编译信息具体说明去上面的xmake链接看看
xmake -v
# 运行
xmake r

# 生成 visual studio 文件夹，点进去打开sln文件即可使用visual studio编辑和调试，很方便
xmake project -k vsxmake
```

## 性能展示

> 什么环境下性能最好？

> 经过测试，在windows环境下，前端使用mingw，后端使用llvm，也就是clang的时候性能最好
>
> 可以酌情开启xmake.lua中的avx512加速

> perf

```
Samples: 31K of event 'task-clock:ppp', Event count (approx.): 7866750000
Overhead  Command  Shared Object         Symbol
  90.39%  test     [unknown]             [k] 0xffffffffa84435e1
   5.48%  test     test                  [.] reverseComplement(std::array<char, 50005ul>&, unsigned long)
   1.58%  test     [unknown]             [k] 0xffffffffc06abd30
   0.59%  test     [unknown]             [k] 0xffffffffa83ab787
   0.51%  test     [unknown]             [k] 0xffffffffa842aee0
```

> 800MB fastq DNA 序列处理性能展示

```
[Timer: All spent] Start timing
Open input file stream to value [input_file_stream] ok , from ["filteredReads.txt"]
Open output file stream to value [output_file_stream] ok , from ["reversedSequence.txt"]
Undergoing transformation
[Timer: All spent] Stop timing , using 5960ms
```

## 关于版权

本项目算法版权归提问者所有，可不遵循开源协议，其它人使用请遵循开源协议，或者欢迎咨询我
