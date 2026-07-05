## 为什么

当前 DMA 驱动的 `dma_cfg` 仅接收 CB 结构体指针写入 CONBLK_AD，不对 CS 寄存器中的优先级、panic 优先级、debug 暂停使能、写响应等待等字段做任何配置。这些字段控制 DMA 通道的 AXI 总线行为，对系统稳定性和调试至关重要，必须可通过配置接口设置。

## 变更内容

- 在 `soc.h` 中新增 `struct rpi4bxwds_dma_cfg`，使用 union 内嵌三种 CB 结构体（`soc_dma_cb` / `soc_dma_lite_cb` / `soc_dma4_cb`），并附加四个 CS 配置字段（`cs_priority`, `cs_panic_priority`, `cs_disdebug`, `cs_wait_for_outstanding_writes`）
- 修改 `soc.c` 中 `dma_cfg` 的实现：将 `cfg` 参数由裸 CB 指针改为 `struct rpi4bxwds_dma_cfg *`，写入 CONBLK_AD 的同时配置 CS 寄存器对应位域
- 配置结构体需 `__xwcc_aligned(32)` 对齐（CB 要求 256-bit 对齐）
- **BREAKING**: `dma_cfg` 的 `void *cfg` 参数语义变更——调用方必须传递 `struct rpi4bxwds_dma_cfg *` 而非裸 CB 指针

## 功能

### 新增功能

- `dma-cfg`: 在 DMA 通道配置中集成 CS 寄存器控制能力，支持设置 AXI 优先级（priority）、panic 优先级（panic_priority）、debug 暂停禁用（disdebug）、写响应等待（wait_for_outstanding_writes）

### 修改功能

- `dma-driver`: 修改 `dma_cfg` 的 BSP 实现，cfg 参数从裸 CB 指针改为包含 CB 和 CS 配置的结构体

## 影响

- **修改**: `board/xwac/xwds/soc.h` — 新增 `struct rpi4bxwds_dma_cfg`，新增 `#include <soc_dma.h>`
- **修改**: `board/xwac/xwds/soc.c` — 修改 `rpi4bxwds_soc_drv_dma_cfg` 实现，写入 CS 寄存器配置
- **依赖**: `xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h`（CB 结构体定义）
