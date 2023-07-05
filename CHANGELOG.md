# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.0.0] - beta

### Added

- 增加对Rust语言的支持。
- 增加对picolibc的支持。
- 增加对 `_Thread_local` , `thread_local` , `__thread` 关键字的支持。

### Changed

- 简化初始化流程。
- 调整xwos的API，使其更接近POSIX。
- 升级 `xwem/vm/lua` 至5.4.6。
- 升级 `xwem/fs/fatfs` 至0.15。
- 升级到C标准到C18。
- 按照MISRA-C:2012标准重构源码。


## [2.0.0] - 2021-11-13

### Added

- 增加对 `<stdlib.h>` 中动态内存管理的支持。
- 增加对 `<stdio.h>` 中 `printf()` 等函数的支持。
- 增加对 `<stdio.h>` 中文件操作的支持（依赖FatFS）。
- 增加对 `<setjmp.h>` 的支持。
- 增加对C++语言的支持。
- 增加对Lua语言的支持。
- 增加对llvm编译器的支持。

## [1.0.0] - 2020-1-18

### Added

- 内核
  - 中断管理
  - 调度器
  - 线程
  - 定时器
  - 电源管理
  - 锁
    - 自旋锁
    - 顺序锁
    - 互斥锁
  - 同步
    - 信号量
    - 条件量
    - 事件标志
    - 线程栅栏
    - 信号选择器
- 中间件
  - 循环队列
  - 消息队列
  - SOC间点对点通讯协议
- BSP
  - 设备栈
    - 部分chip的驱动
  - CPU架构
    - ARMv6m/ARMv7m
    - Embedded PowerPC
    - RISC-V
- 编译系统
