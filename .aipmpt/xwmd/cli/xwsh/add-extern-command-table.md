# Role

+ 经验丰富的嵌入式软件工程师，精通C语言，并熟悉MISRA-C:2012标准


# Task

帮助xwsh实现外部的命令列表

+ 增加API: `xwsh_set_ext_cmd_table(const struct xwsh_cmd cmd[], xwsz_t num)`
  + 功能: 设置外部命令列表
  + 参数:
    + `cmd` : 命令数组
    + `num` : 命令的数量


# Context

+ xwsh codebase: `xwmd/cli/xwsh`
+ xwsh只有一个命令列表 `xwsh_cmd_table` 定义在 `cmd.c` ，
  若用户需要增加自定义命令，需要更改 `cmd.c` 文件，
  但 `cmd.c` 属于开源文件，不属于项目相关文件，不利于项目源码管理。
+ `core.c` 中 `xwsh_show_all_cmds()` 用于打印所有命令，若用户增加外部命令列表，需要增加打印
+ `core.c` 中 `xwsh_find_cmd()` 用于查找所有命令，若用户增加外部命令列表，需要增加查找
+ `mi.h` 是xwsh的API头文件，需要增加API的声明，并增加doxygen风格API注释
+ 如果需要编译验证，需要切换到 `xwbd/WeActMiniStm32H750` 执行 `make -j22` ，
  输出内容位于 `xwbd/WeActMiniStm32H750/wkspc` ，输出的二进制是嵌入式固件，不可在本机执行。


# Constraints

+ 代码风格跟随 `xwmd/cli/xwsh/cmd.c`
+ 遵循MISRA-C:2012标准
  + 函数至多在尾部拥有两个 `return`
    + 一个无错误 `return`
    + 一个发生错误通过 `goto` 跳转过来 `return`
  + 尽量不使用 `continue`
+ 如果外部命令与内部命令重名，报错
+ 设计要点
  + 优先级：内置命令优先，外部命令次之
  + 显示方式：合并显示内置和外置命令
  + 重名检查：是，若重名，返回错误码
  + 多线程：不支持
  + 初始化：初始化为NULL，大小为0表示没有外部命令
  + 重复调用：允许重复调用覆盖旧的设置
  + 清空命令：允许 `xwsh_set_ext_cmd_table(NULL, 0);` 清楚设置
