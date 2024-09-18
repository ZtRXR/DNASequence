# DNASequence

[提问者问题原文链接](https://www.zhihu.com/question/36143261/answer/3624848144)

## 代码逻辑介绍

DNASequence处理

DNA是双链的，互为互补链，对DNA样本进行测序时不能确认测出的是哪条链，所以就把所有DNA片段的互补链全算出来，和原文件放在一起组装。

## 关于如何构建本项目

> 请确保安装了构建工具xmake，和任意C++构建工具并将路径添加到了PATH目录

[如何安装xmake?链接](https://gitee.com/tboox/xmake#%E5%AE%89%E8%A3%85)

```bash
#编译
xmake -v
# 运行
xmake r
```

## 关于版权

本项目算法版权归提问者所有，可不遵循开源协议，其它人使用请遵循开源协议
