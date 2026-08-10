## ADDED Requirements

### Requirement: C99 j 长度限定符基础支持

系统 SHALL 支持 C99 标准 `j` 长度限定符，用于格式化 `intmax_t` 和 `uintmax_t` 类型。

`j` 限定符 SHALL 与 `d`、`i`、`u`、`o`、`x`、`X` 类型字符组合使用，对应 `intmax_t`（有符号）或 `uintmax_t`（无符号）参数。

#### Scenario: 有符号十进制输出(%jd)
- **WHEN** 调用 `xwsnpf(buf, 64, "%jd", (intmax_t)-42)`
- **THEN** buf 内容为 `"-42"`，返回值为 3

#### Scenario: 无符号十进制输出(%ju)
- **WHEN** 调用 `xwsnpf(buf, 64, "%ju", (uintmax_t)12345)`
- **THEN** buf 内容为 `"12345"`，返回值为 5

#### Scenario: 八进制输出(%jo)
- **WHEN** 调用 `xwsnpf(buf, 64, "%jo", (uintmax_t)255)`
- **THEN** buf 内容为 `"377"`，返回值为 3

#### Scenario: 小写十六进制输出(%jx)
- **WHEN** 调用 `xwsnpf(buf, 64, "%jx", (uintmax_t)255)`
- **THEN** buf 内容为 `"ff"`，返回值为 2

#### Scenario: 大写十六进制输出(%jX)
- **WHEN** 调用 `xwsnpf(buf, 64, "%jX", (uintmax_t)255)`
- **THEN** buf 内容为 `"FF"`，返回值为 2

### Requirement: %j 与 XWOS 扩展二进制格式配合

系统 SHALL 支持 `j` 限定符与 XWOS 扩展的 `b`/`B` 二进制格式配合使用。

#### Scenario: 小写二进制输出(%jb)
- **WHEN** 调用 `xwsnpf(buf, 64, "%jb", (uintmax_t)42)`
- **THEN** buf 内容为 `"101010"`，返回值为 6

#### Scenario: 大写二进制输出(%jB)
- **WHEN** 调用 `xwsnpf(buf, 64, "%jB", (uintmax_t)42)`
- **THEN** buf 内容为 `"101010"`，返回值为 6

### Requirement: %j 与标志位配合

系统 SHALL 支持 `j` 限定符与所有标准标志位（`-`、`+`、空格、`#`、`0`）组合使用，行为与 `%lld` 一致。

#### Scenario: 正数显式加号标志
- **WHEN** 调用 `xwsnpf(buf, 64, "%+jd", (intmax_t)42)`
- **THEN** buf 内容为 `"+42"`

#### Scenario: 空格标志
- **WHEN** 调用 `xwsnpf(buf, 64, "% jd", (intmax_t)42)`
- **THEN** buf 内容以空格开头，为 `" 42"`

#### Scenario: 零填充标志
- **WHEN** 调用 `xwsnpf(buf, 64, "%08ju", (uintmax_t)42)`
- **THEN** buf 内容为 `"00000042"`

#### Scenario: 左对齐标志
- **WHEN** 调用 `xwsnpf(buf, 64, "%-8ju", (uintmax_t)42)`
- **THEN** buf 内容为 `"42      "`

#### Scenario: 十六进制特殊前缀标志
- **WHEN** 调用 `xwsnpf(buf, 64, "%#jx", (uintmax_t)255)`
- **THEN** buf 内容为 `"0xff"`

### Requirement: %j 与宽度和精度配合

系统 SHALL 支持 `j` 限定符与宽度和精度说明符组合使用。

#### Scenario: 最小宽度
- **WHEN** 调用 `xwsnpf(buf, 64, "%10ju", (uintmax_t)42)`
- **THEN** buf 内容为 10 字符宽，右对齐，`"        42"`

#### Scenario: 精度控制
- **WHEN** 调用 `xwsnpf(buf, 64, "%.5jd", (intmax_t)42)`
- **THEN** buf 内容为 `"00042"`

#### Scenario: 动态宽度(星号)
- **WHEN** 调用 `xwsnpf(buf, 64, "%*ju", 8, (uintmax_t)42)`
- **THEN** buf 内容为 8 字符宽，右对齐

### Requirement: 现有格式不受影响

系统 SHALL 保持所有现有格式说明符的行为不变。`j` 限定符的新增 MUST NOT 破坏现有的 `%d`、`%ld`、`%lld`、`%zd`、`%td` 等格式的输出。

#### Scenario: 无 j 限定符的格式化保持不变
- **WHEN** 调用 `xwsnpf(buf, 64, "%d %ld %lld", (int)1, (long)2L, (long long)3LL)`
- **THEN** buf 内容为 `"1 2 3"`

#### Scenario: 单个 j 字符不破坏普通文本
- **WHEN** 调用 `xwsnpf(buf, 64, "abc j def %d", 42)`
- **THEN** buf 内容为 `"abc j def 42"`
