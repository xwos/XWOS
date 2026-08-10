# 任务：修复 xwvsnpf() 浮点特殊值输出

## 1. 修复 %#.0f 家族的冗余零（决策 1 方案 A）

- [x] 1.1 修改 `xwvsnpf_put_float_decimal()`（xwspf.c:421）：`num == 0` 分支仅当 `digits > 0` 时写入 `'0'`；`digits == 0` 时不写入任何字符
- [x] 1.2 确认修改后 `digits > 0` 的既有调用点（int_part 恒 1、exp 恒 1）输出不变

## 2. 修复 nan/inf 特殊值符号（决策 2 第 2 档）

- [x] 2.1 `xwvsnpf_format_float()` nan 分支（xwspf.c:504）：`signbit(num)` → `-`；否则 `XWVSNPF_F_PLUS` → `+`；否则 `XWVSNPF_F_SPACE` → 空格
- [x] 2.2 `xwvsnpf_format_float()` inf 分支（xwspf.c:511）：保留 `num < 0` → `-`，补充 `+` 与空格标志
- [x] 2.3 `xwvsnpf_format_long_double()` nan 分支（xwspf.c:688）：与 2.1 相同修改
- [x] 2.4 `xwvsnpf_format_long_double()` inf 分支（xwspf.c:695）：与 2.2 相同修改

## 3. 验证

- [x] 3.1 宿主对比 glibc 测试：沿用 `/tmp/opencode/test_spf_g.c` 框架，加入本次场景并全部通过：`%#.0f` 3.0→`3.`、`%#.0f` 0.0→`0.`、`%#.0g` 3.0→`3.`、`%#.0g` 1234567.0→`1.e+06`、`%#.0e` 3.0→`3.e+00`、`%L` 变体对应场景、`-nan`→`-nan`、`%+f` nan→`+nan`、`% f` nan→` nan`、`%+f` inf→`+inf`、`%f` −inf→`-inf`
- [x] 3.2 确认原 48/50 用例无回归（`%.0f`→`3`、`%g`/`%G`/`%Lg` 既有场景不变）
- [x] 3.3 RPi4B 板级编译：`source env.sh && xwm`，`xwspf.o` 无警告无错误
- [x] 3.4 MISRA-C:2012 检查：`make xwos.mc`（或 `make mc`）确认新代码无违规
