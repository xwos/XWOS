## 为什么

树莓派4B (BCM2711) 板级支持包目前缺少 DMA 驱动。配置文件中 `XWCDCFG_ds_SOC_DMA=1` 已启用，SOC 驱动函数表 (`struct xwds_soc_driver`) 中 GPIO 和 EIRQ 已实现，但 DMA 的 7 个函数指针均为空。上层设备驱动（如 UART DMA 模式、SPI DMA 传输）无法通过标准 SOC 框架使用 DMA 通道。

## 变更内容

- 在 `soc.c` 的驱动函数表 `rpi4bxwds_soc_drv` 中添加 7 个 DMA BSP 函数：
  `dma_req`, `dma_rls`, `dma_cfg`, `dma_enable`, `dma_disable`, `dma_start`, `dma_stop`
- 在 `rpi4bxwds_soc` 对象中添加 `.dma` 子结构体（通道数、回调表）
- 添加 12 个 DMA ISR，在 `probe()` 中注册到 GIC
- 跳过 Ch15（VPU 独占），实际可用通道 0~14，共 15 个
- 遵循 MISRA-C:2012 的 return/goto 约束

## 功能

### 新增功能

- `dma-driver`: 为 BCM2711 芯片提供 SOC 层面的 DMA 驱动，支持通道 0~14 的申请/释放/配置/使能/禁用/启动/停止。三种通道类型（普通 DMA 0~6、DMA Lite 7~10、DMA4 11~14）均通过统一的驱动函数表接口访问，DMA 传输完成通过中断回调通知上层。

### 修改功能

_无_

## 影响

- **修改**: `board/xwac/xwds/soc.c` — 添加 DMA 驱动函数实现和 ISR
- **修改**: `board/xwac/xwds/soc.h` — 如有需要添加 DMA 类型声明
- **依赖**: `xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h`（寄存器定义）、`xwcd/ds/soc/dma.h`（DMA API 框架）、`xwos/osal/lock/spinlock.h`（自旋锁）
- **配置**: `XWCDCFG_ds_SOC_DMA=1`（已启用）
