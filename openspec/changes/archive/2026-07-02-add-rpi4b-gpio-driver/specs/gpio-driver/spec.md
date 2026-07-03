## 新增需求

### 需求:GPIO引脚申请与释放

系统必须支持通过 `xwds_gpio_req` 和 `xwds_gpio_rls` 申请和释放 GPIO 引脚。Port 参数在多路映射场景中标识端口组，BCM2711 仅使用单一端口（PortA），Port 参数被忽略，引脚号直接映射到 BCM2711 的 GPIO[0:57]。

#### 场景:成功申请单个引脚

- **当** 调用方调用 `xwds_gpio_req(soc, XWDS_GPIO_PORT_A, XWDS_GPIO_PIN(5))`
- **那么** 系统返回 `XWOK`，PIN 5 被标记为已占用

#### 场景:成功申请多个引脚

- **当** 调用方调用 `xwds_gpio_req(soc, XWDS_GPIO_PORT_A, 0x0F)`
- **那么** 系统返回 `XWOK`，PIN 0~3 全部被标记为已占用

#### 场景:申请已被占用的引脚

- **当** PIN 3 已被申请，再次调用 `xwds_gpio_req(soc, XWDS_GPIO_PORT_A, XWDS_GPIO_PIN(3))`
- **那么** 系统返回 `-EBUSY`

#### 场景:释放引脚

- **当** 调用 `xwds_gpio_rls(soc, XWDS_GPIO_PORT_A, XWDS_GPIO_PIN(5))` 且 PIN 5 处于已申请状态
- **那么** 系统返回 `XWOK`，PIN 5 被标记为未占用

#### 场景:释放未被申请的引脚

- **当** PIN 5 未被申请，调用 `xwds_gpio_rls(soc, XWDS_GPIO_PORT_A, XWDS_GPIO_PIN(5))`
- **那么** 系统返回 `-EPERM`

#### 场景:Port超出范围

- **当** 调用 `xwds_gpio_req(soc, 非零Port, pinmask)`
- **那么** 系统返回 `-ERANGE`

---

### 需求:GPIO功能与上下拉配置

系统必须支持通过 `xwds_gpio_cfg` 配置 GPIO 引脚的功能选择（输入/输出）和上下拉状态。

#### 场景:配置为输出模式

- **当** 调用 `xwds_gpio_cfg(soc, port, XWDS_GPIO_PIN(5), cfg)` 且 `cfg->function = SOC_GPIO_ALT_GPO`
- **那么** PIN 5 被配置为 GPIO 输出功能

#### 场景:配置为输入模式

- **当** 调用 `xwds_gpio_cfg(soc, port, XWDS_GPIO_PIN(5), cfg)` 且 `cfg->function = SOC_GPIO_ALT_GPI`
- **那么** PIN 5 被配置为 GPIO 输入功能

#### 场景:配置上拉

- **当** 调用 `xwds_gpio_cfg(soc, port, XWDS_GPIO_PIN(5), cfg)` 且 `cfg->pud = SOC_GPIO_PUPDC_PU`
- **那么** PIN 5 的内部上拉被使能

#### 场景:配置下拉

- **当** 调用 `xwds_gpio_cfg(soc, port, XWDS_GPIO_PIN(5), cfg)` 且 `cfg->pud = SOC_GPIO_PUPDC_PD`
- **那么** PIN 5 的内部下拉被使能

#### 场景:cfg指针为空

- **当** 调用 `xwds_gpio_cfg(soc, port, pinmask, NULL)`
- **那么** 系统返回 `-EFAULT`

---

### 需求:GPIO置高

系统必须支持通过 `xwds_gpio_set` 将指定引脚的电平置为高电平。

#### 场景:置高

- **当** 调用 `xwds_gpio_set(soc, XWDS_GPIO_PORT_A, XWDS_GPIO_PIN(5))` 且 PIN 5 为输出模式
- **那么** PIN 5 的电平被置为高

---

### 需求:GPIO置低

系统必须支持通过 `xwds_gpio_reset` 将指定引脚的电平置为低电平。

#### 场景:置低

- **当** 调用 `xwds_gpio_reset(soc, XWDS_GPIO_PORT_A, XWDS_GPIO_PIN(5))` 且 PIN 5 为输出模式
- **那么** PIN 5 的电平被置为低

---

### 需求:GPIO翻转

系统必须支持通过 `xwds_gpio_toggle` 翻转指定引脚的电平。

#### 场景:翻转高电平

- **当** PIN 5 当前为低电平，调用 `xwds_gpio_toggle(soc, XWDS_GPIO_PORT_A, XWDS_GPIO_PIN(5))`
- **那么** PIN 5 的电平翻转为高

#### 场景:翻转低电平

- **当** PIN 5 当前为高电平，调用 `xwds_gpio_toggle(soc, XWDS_GPIO_PORT_A, XWDS_GPIO_PIN(5))`
- **那么** PIN 5 的电平翻转为低

---

### 需求:GPIO并行输出

系统必须支持通过 `xwds_gpio_output` 对多个引脚同时输出指定的电平值，未被 pinmask 覆盖的引脚电平禁止改变。

#### 场景:并行输出混合电平

- **当** 调用 `xwds_gpio_output(soc, port, 0x0F, 0x0A)`（pinmask=0x0F, out=0x0A）
- **那么** PIN 0 = 0, PIN 1 = 1, PIN 2 = 0, PIN 3 = 1，其他引脚不变

---

### 需求:GPIO输入读取

系统必须支持通过 `xwds_gpio_input` 并行读取多个引脚的电平。

#### 场景:读取输入

- **当** 调用 `xwds_gpio_input(soc, port, 0x0F, &buf)` 且 PIN 0=1, PIN 1=0, PIN 2=1, PIN 3=0
- **那么** `buf` 的值为 `0x05`（仅 PIN 0 和 PIN 2 对应的位为1）

---

### 需求:SMP并发安全

系统必须保证在多核并发访问 GPIO 寄存器时操作的原子性。

#### 场景:多核并发访问保护

- **当** 两个 CPU 核同时调用 GPIO 操作函数
- **那么** 自旋锁保证同一时刻只有一个核访问 GPIO 寄存器

---

### 需求:MISRA-C:2012合规

GPIO 驱动代码必须遵循 MISRA-C:2012 标准。

#### 场景:函数return控制

- **当** 检查任意 GPIO 驱动函数
- **那么** 每个函数至多有两条 return 语句：一条成功返回，一条通过 goto 跳转后的错误返回
