## 新增需求

### 需求:DMA通道申请

系统必须支持通过 `xwds_dma_req` 申请 DMA 通道。申请时执行通道复位（CS.RESET=1），
标记通道为已占用。通道 15 被 VPU 独占，禁止 ARM 侧申请。

#### 场景:成功申请通道

- **当** 调用 `xwds_dma_req(soc, XWDS_DMA_CH(3))` 且通道 3 空闲
- **那么** 系统返回 `XWOK`，通道 3 被复位并被标记为已占用

#### 场景:申请超出范围的通道

- **当** 调用 `xwds_dma_req(soc, XWDS_DMA_CH(15))`
- **那么** 系统返回 `-ERANGE`

#### 场景:重复申请同一通道

- **当** 通道 3 已被申请，再次调用 `xwds_dma_req(soc, XWDS_DMA_CH(3))`
- **那么** 系统返回 `-EBUSY`

#### 场景:申请 VPU 独占通道

- **当** 调用 `xwds_dma_req(soc, XWDS_DMA_CH(15))`
- **那么** 系统返回 `-ERANGE`

#### 场景:soc指针为空

- **当** 调用 `xwds_dma_req(NULL, XWDS_DMA_CH(0))`
- **那么** 系统返回 `-EFAULT`

---

### 需求:DMA通道释放

系统必须支持通过 `xwds_dma_rls` 释放 DMA 通道。释放时清除 CONBLK_AD 寄存器为 0，
标记通道为空闲。

#### 场景:成功释放已申请通道

- **当** 通道 3 处于已申请状态，调用 `xwds_dma_rls(soc, XWDS_DMA_CH(3))`
- **那么** 系统返回 `XWOK`，通道 3 的 CONBLK_AD 被清零，通道被标记为空闲

#### 场景:释放未申请的通道

- **当** 通道 3 未被申请，调用 `xwds_dma_rls(soc, XWDS_DMA_CH(3))`
- **那么** 系统返回 `-EPERM`

---

### 需求:DMA通道配置

系统必须支持通过 `xwds_dma_cfg` 配置 DMA 通道的控制块地址。
根据通道类型（普通 DMA / DMA Lite / DMA4）将 CB 地址写入对应寄存器格式。
普通 DMA 和 DMA Lite 写入完整 32 位地址（低 5 位必须为 0），DMA4 写入 `addr >> 5` 格式。

#### 场景:配置普通DMA通道的CB地址

- **当** 调用 `xwds_dma_cfg(soc, XWDS_DMA_CH(3), &cb, callback, arg)` 且 cb 地址 32 字节对齐
- **那么** 系统将 cb 地址写入 soc_dma3.CONBLK_AD，回调函数和参数被存储到对应通道的回调表中，返回 `XWOK`

#### 场景:配置DMA4通道的CB地址

- **当** 调用 `xwds_dma_cfg(soc, XWDS_DMA_CH(11), &cb, callback, arg)` 且 cb 地址 32 字节对齐
- **那么** 系统将 `cb_addr >> 5` 写入 soc_dma11.CB，返回 `XWOK`

#### 场景:cfg指针为空

- **当** 调用 `xwds_dma_cfg(soc, XWDS_DMA_CH(3), NULL, callback, arg)`
- **那么** 系统返回 `-EFAULT`

#### 场景:CB地址未对齐

- **当** 调用 `xwds_dma_cfg(soc, XWDS_DMA_CH(3), &unaligned_cb, callback, arg)` 且 cb 地址不以 32 字节对齐
- **那么** 系统返回 `-EINVAL`

#### 场景:通道未申请

- **当** 通道 3 未被申请，调用 `xwds_dma_cfg(soc, XWDS_DMA_CH(3), &cb, callback, arg)`
- **那么** 系统返回 `-EPERM`

---

### 需求:DMA通道使能与禁用

`xwds_dma_enable` 和 `xwds_dma_disable` 在 BCM2711 上无对应寄存器操作，
直接返回 `XWOK`。

#### 场景:使能通道

- **当** 调用 `xwds_dma_enable(soc, XWDS_DMA_CH(3))`
- **那么** 系统返回 `XWOK`

#### 场景:禁用通道

- **当** 调用 `xwds_dma_disable(soc, XWDS_DMA_CH(3))`
- **那么** 系统返回 `XWOK`

---

### 需求:DMA传输启动

系统必须支持通过 `xwds_dma_start` 启动 DMA 传输。驱动将 CS 寄存器的 ACTIVE 位置 1。

#### 场景:启动已配置的通道

- **当** 通道 3 已完成 cfg 配置，调用 `xwds_dma_start(soc, XWDS_DMA_CH(3))`
- **那么** 系统将 CS.ACTIVE 置 1，DMA 开始传输，返回 `XWOK`

#### 场景:启动未配置的通道

- **当** 通道 3 未完成 cfg 配置，调用 `xwds_dma_start(soc, XWDS_DMA_CH(3))`
- **那么** 系统返回 `-EPERM`

---

### 需求:DMA传输停止

系统必须支持通过 `xwds_dma_stop` 暂停 DMA 传输。驱动将 CS 寄存器的 ACTIVE 位清 0，
通道进入暂停状态，CB 地址保留，可通过 `dma_start` 恢复。

#### 场景:停止活跃通道

- **当** 通道 3 处于活跃传输状态，调用 `xwds_dma_stop(soc, XWDS_DMA_CH(3))`
- **那么** 系统将 CS.ACTIVE 清 0，DMA 暂停，返回 `XWOK`

#### 场景:停止已空闲通道

- **当** 通道 3 当前空闲，调用 `xwds_dma_stop(soc, XWDS_DMA_CH(3))`
- **那么** 系统返回 `XWOK`（幂等操作）

---

### 需求:DMA传输完成中断

DMA 传输完成时，系统必须通过中断机制调用对应通道的回调函数。
ISR 检测 CS.INT 标志，清除后调用 `chcbs[ch]` 回调。

#### 场景:普通通道传输完成中断

- **当** 通道 3 完成一次 CB 传输且 CB 中 INTEN=1，硬件置 CS.INT=1
- **那么** ISR 检测到后清除 CS.INT，调用 `chcbs[3](soc, 3, rc, arg)`

#### 场景:共享IRQ通道中断（DMA7_8）

- **当** 通道 7 和通道 8 共享 `SOC_VC_IRQ_DMA7_8`，通道 7 先完成
- **那么** ISR 分别检查 CS7.INT 和 CS8.INT，仅调用已完成通道的回调

#### 场景:DMA4通道传输完成中断

- **当** 通道 11（DMA4）完成一次 CB 传输且 CB 中 INTEN=1
- **那么** ISR 检测 CS11.INT 后清除，调用 `chcbs[11](soc, 11, rc, arg)`

---

### 需求:SMP并发安全

系统必须保证在多核并发访问 DMA 寄存器时操作的原子性。
使用与 GPIO/EIRQ 驱动相同的自旋锁 `struct xwos_splk`。

#### 场景:多核并发访问保护

- **当** 两个 CPU 核同时调用 DMA 操作函数
- **那么** 自旋锁保证同一时刻只有一个核访问 DMA 寄存器

---

### 需求:MISRA-C:2012合规

DMA 驱动代码必须遵循 MISRA-C:2012 标准。

#### 场景:函数return控制

- **当** 检查任意 DMA 驱动函数
- **那么** 每个函数至多有两条 return 语句：一条成功返回，一条通过 goto 跳转后的错误返回
