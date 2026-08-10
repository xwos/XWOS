## Task

为 `xwvsnpf()` 增加格式化选项 `%g`


## Context

+ 文件 `xwos/lib/xwspf.c` 中的函数 `xwvsnpf()` : 功能与 `vsnprintf()` 相同


# Constraints

+ 遵循MISRA-C:2012标准
  + 函数至多在尾部拥有两个 `return`
    + 一个无错误 `return`
    + 一个发生错误通过 `goto` 跳转过来 `return`
  + 尽量不使用 `continue`
