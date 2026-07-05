## 新增需求

### 需求:DMA通道CS寄存器配置

`dma_cfg` 必须支持通过 `struct rpi4bxwds_dma_cfg` 配置 DMA 通道 CS 寄存器中的
以下字段：AXI 优先级（priority）、AXI panic 优先级（panic_priority）、
debug 暂停禁用（disdebug）、写响应等待（wait_for_outstanding_writes）。

配置结构体 `__xwcc_aligned(32)` 对齐，内嵌 union cb 包含三种通道类型的 CB 结构体。
`cfg` 指针即 CB 地址（CB 位于结构体 offset 0）。

#### 场景:配置普通DMA通道的优先级和debug控制

- **当** 通过 `rpi4bxwds_dma_cfg` 配置 Ch3（普通DMA），设 `cs_priority=8`,
  `cs_panic_priority=15`, `cs_disdebug=1`, `cs_wait_for_outstanding_writes=0`
- **那么** `dma_cfg` 将 CB 地址写入 `soc_dma3.cb.u32`，
  并将 CS 寄存器的 `priority=8`, `panic_priority=15`, `disdebug=1`,
  `wait_for_outstanding_writes=0` 写入 `soc_dma3.cs`

#### 场景:配置DMA Lite通道

- **当** 通过 `rpi4bxwds_dma_cfg` 配置 Ch8（DMA Lite），使用 `.cb.lite` 成员
- **那么** `dma_cfg` 将 CB 地址写入 `soc_dma8.cb.u32`，CS 配置以相同位域名称写入
  `soc_dma8.cs`

#### 场景:配置DMA4通道

- **当** 通过 `rpi4bxwds_dma_cfg` 配置 Ch11（DMA4），使用 `.cb.dma4` 成员
- **那么** `dma_cfg` 将 CB 地址写入 `soc_dma11.cb.u32`（`addr>>5` 格式），
  CS 配置以相同位域名称写入 `soc_dma11.cs`

#### 场景:CS默认值

- **当** 调用方将 `cs_priority`, `cs_panic_priority`, `cs_disdebug`,
  `cs_wait_for_outstanding_writes` 全部设为 0
- **那么** CS 寄存器对应位域均被设为 0，等同于硬件复位默认值
