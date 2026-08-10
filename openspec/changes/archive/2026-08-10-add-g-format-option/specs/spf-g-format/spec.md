## ADDED Requirements

### Requirement: 解析 %g/%G 转换说明符

`xwvsnpf_format_decode()` SHALL 支持解析 `%g` 与 `%G` 转换说明符（含宽度、精度、标志位与 `L` 修饰符）。`%g` 使用小写指数符号 `e`，`%G` 使用大写指数符号 `E`。该功能 SHALL 受 `XWLIBCFG_SPF_FLOAT` 编译开关控制，`%Lg`/`%LG` SHALL 受 `XWLIBCFG_SPF_LONG_DOUBLE` 编译开关控制。

#### Scenario: 解析 %g

- **WHEN** 格式字符串包含 `%g`
- **THEN** 解码为 general 模式的 double 类型，指数符号为小写 `e`

#### Scenario: 解析 %G

- **WHEN** 格式字符串包含 `%G`
- **THEN** 解码为 general 模式的 double 类型，指数符号为大写 `E`

#### Scenario: 解析 %Lg

- **WHEN** 格式字符串包含 `%Lg` 且启用了 `XWLIBCFG_SPF_LONG_DOUBLE`
- **THEN** 解码为 general 模式的 long double 类型，指数符号为小写 `e`

#### Scenario: 关闭浮点开关时不支持 %g

- **WHEN** `XWLIBCFG_SPF_FLOAT` 未定义或不为 1
- **THEN** `%g` 与 `%G` 按无效转换说明符处理，不产生浮点输出

### Requirement: %g 有效数字精度语义

`%g` SHALL 将精度解释为有效数字位数（P），而非小数位数：精度缺省时 P 为 6，显式精度 0 时 P 视为 1。输出 SHALL 先按 P 位有效数字四舍五入，再选择输出风格。

#### Scenario: 默认精度为 6 位有效数字

- **WHEN** 以 `%g` 格式化 123.4567
- **THEN** 输出 `123.457`（6 位有效数字，四舍五入）

#### Scenario: 显式精度 0 视为 1

- **WHEN** 以 `%.0g` 格式化 0.4
- **THEN** 输出 `0.4`（P=1）

#### Scenario: 显式精度限制有效数字

- **WHEN** 以 `%.3g` 格式化 1234.56
- **THEN** 输出 `1.23e+03`（3 位有效数字）

### Requirement: %g 风格选择

`%g` SHALL 根据舍入后的指数 X 选择输出风格：当 `P > X ≥ −4` 时使用定点（f）风格，小数位数为 `P − X − 1`；否则使用科学（e）风格，小数位数为 `P − 1`。风格选择的判断 SHALL 基于四舍五入后的指数（舍入进位可能改变指数并导致风格切换）。

#### Scenario: 大指数使用科学计数法

- **WHEN** 以 `%g` 格式化 1234567.0（X=6 ≥ P=6）
- **THEN** 输出 `1.23457e+06`

#### Scenario: 小指数使用科学计数法

- **WHEN** 以 `%g` 格式化 0.000012（X=−5 < −4）
- **THEN** 输出 `1.2e-05`

#### Scenario: 指数 −4 时使用定点风格

- **WHEN** 以 `%g` 格式化 0.00012（X=−4）
- **THEN** 输出 `0.00012`（定点风格）

#### Scenario: 指数在范围内使用定点风格

- **WHEN** 以 `%g` 格式化 12345.6（X=4 < P=6）
- **THEN** 输出 `12345.6`

#### Scenario: 舍入进位导致风格切换

- **WHEN** 以 `%.3g` 格式化 9999.0（归一化 X=3，四舍五入到 3 位有效数字后为 1.00e+04，X=4 ≥ P=3）
- **THEN** 输出 `1e+04`（科学计数法）

### Requirement: %g 移除尾随零

默认情况下（无 `#` 标志），`%g` SHALL 移除小数部分的尾随零；若小数部分全部为零，SHALL 同时移除小数点。带 `#` 标志时 SHALL 保留尾随零与小数点。

#### Scenario: 移除尾随零

- **WHEN** 以 `%g` 格式化 1.500
- **THEN** 输出 `1.5`

#### Scenario: 移除空小数点

- **WHEN** 以 `%.2g` 格式化 1500.0
- **THEN** 输出 `1.5e+03`

#### Scenario: # 标志保留尾随零

- **WHEN** 以 `%#.2g` 格式化 1500.0
- **THEN** 输出 `1.5e+03`（带 # 时保留尾随零，输出 `1.50e+03` 的规则适用于有小数位的情况，此处科学计数法小数位为 P−1=1，无尾随零可移除）

#### Scenario: 定点风格下 # 标志保留小数部分

- **WHEN** 以 `%#.3g` 格式化 1.5
- **THEN** 输出 `1.50`（# 保留尾随零）

### Requirement: %e/%E 舍入进位后重新归一化指数

`xwvsnpf_format_float()` 与 `xwvsnpf_format_long_double()` 的科学计数法模式（`%e`/`%E`/`%Le`/`%LE` 及 `%g`/`%G` 选中的 e 风格）SHALL 在四舍五入进位导致 mantissa 整数位变为 10 时，重新归一化 mantissa（整数位除以 10）并将指数加 1，确保 mantissa 位于 [1, 10) 区间。

#### Scenario: %e 进位后重新归一化

- **WHEN** 以 `%.3e` 格式化 9999.9
- **THEN** 输出 `1.000e+04`（而非 `10.000e+03`）

#### Scenario: %g 的 e 风格进位后重新归一化

- **WHEN** 以 `%.4g` 格式化 9.9999
- **THEN** 输出 `10`（P=4，X=0，四舍五入为 10.00，X 仍为 0 → 定点风格输出 `10`）

#### Scenario: 定点风格进位不改变指数

- **WHEN** 以 `%.2g` 格式化 9.99（P=2，X=0，进位后为 10，X=0 < P）
- **THEN** 输出 `10`（定点风格，无需科学计数法）

### Requirement: %g 标志位与填充行为

`%g` SHALL 支持 `-`（左对齐）、`+`（强制正号）、空格（正号显示空格）、`0`（零填充）、宽度与 `#` 标志，行为与 `%f`/`%e` 一致。符号与标志处理 SHALL 在尾随零移除后基于最终字符串长度计算填充。

#### Scenario: 宽度与零填充

- **WHEN** 以 `%010.3g` 格式化 123.456
- **THEN** 输出 `0000000123`（宽度 10，零填充）

#### Scenario: 左对齐

- **WHEN** 以 `%-10.3g` 格式化 123.456
- **THEN** 输出 `123       `（左对齐，右侧空格填充）

#### Scenario: 强制正号

- **WHEN** 以 `%+.3g` 格式化 1.5
- **THEN** 输出 `+1.5`

### Requirement: %g 特殊值与符号处理

`%g` SHALL 对 NaN 输出 `nan`/`NaN`（受 `%G` 大小写影响遵循现有 `%e`/`%E` 行为：`%g` 输出 `nan`，`%G` 输出 `NAN` 的方式应与现有 `%e` 的 nan/inf 输出风格一致，即小写 `nan`/`inf` 不变，大写仅影响指数符号），对无穷大输出 `inf`/`-inf`。负数的负号 SHALL 正常输出。

#### Scenario: 负零输出

- **WHEN** 以 `%.3g` 格式化 −0.0
- **THEN** 输出 `-0`

#### Scenario: 大整数部分不溢出缓冲区

- **WHEN** 以 `%g` 格式化 1.0e20
- **THEN** 输出 `1e+20`，不产生缓冲区溢出

### Requirement: long double 的 %g 支持

启用 `XWLIBCFG_SPF_LONG_DOUBLE` 时，`%Lg`/`%LG` SHALL 提供与 `%g`/`%G` 相同的行为，作用于 long double 类型。

#### Scenario: %Lg 格式化 long double

- **WHEN** 以 `%Lg` 格式化 long double 值 123.456
- **THEN** 输出 `123.456`
