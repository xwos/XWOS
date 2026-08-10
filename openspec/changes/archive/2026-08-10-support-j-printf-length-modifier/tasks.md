## 1. 枚举扩展

- [x] 1.1 在 `xwvsnpf_format_type_em` 枚举中新增 `XWVSNPF_FT_INTMAX_T` 和 `XWVSNPF_FT_UINTMAX_T`（第 48 行后 `XWVSNPF_FT_PTRDIFF` 之后）

## 2. 格式解析

- [x] 2.1 在 `xwvsnpf_format_decode()` 限定符检测（第 977 行）中加入 `('j' == *fmt)` 条件
- [x] 2.2 在 `xwvsnpf_format_decode()` 限定符分派（第 1116 行后）新增 `'j'` 分支：有符号→`FT_INTMAX_T`，无符号→`FT_UINTMAX_T`

## 3. 主分派循环

- [x] 3.1 在 `xwvsnpf()` 的 `default` 子 `switch`（第 1267 行）中新增 `FT_INTMAX_T` 和 `FT_UINTMAX_T` case，分别用 `intmax_t` 和 `uintmax_t` 调用 `va_arg`，转换为 `xwu64_t`

## 4. 编译验证

- [x] 4.1 编译整个工程（`xwm -B`），确认无编译错误和新增警告
- [x] 4.2 运行 MISRA-C 检查（`make mc`），确认无新增违规
