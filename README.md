# DNASequence

[提问者问题原文链接](https://www.zhihu.com/question/36143261/answer/3624848144)

## 代码逻辑介绍

DNASequence处理

DNA是双链的，互为互补链，对DNA样本进行测序时不能确认测出的是哪条链，所以就把所有DNA片段的互补链全算出来，和原文件放在一起组装。

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

本项目算法版权归提问者所有，可不遵循开源协议，其它人使用请遵循开源协议，或者咨询我
