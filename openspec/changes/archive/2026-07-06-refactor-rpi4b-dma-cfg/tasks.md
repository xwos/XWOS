## 1. 新增 DMA 配置结构体

- [x] 1.1 在 `soc.h` 中添加 `#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h>`
- [x] 1.2 在 `soc.h` 中定义 `struct rpi4bxwds_dma_cfg`，含 `union cb`（.dma / .lite / .dma4）和四个 `xwu32_t` CS 字段，使用 `__xwcc_aligned(32)`

## 2. 修改 dma_cfg 实现

- [x] 2.1 在 `soc.c` 的 `rpi4bxwds_soc_drv_dma_cfg` 中将 `cfg` 从裸 CB 指针改为 `const struct rpi4bxwds_dma_cfg *` 强制转换
- [x] 2.2 在 switch/case 每个分支中添加 CS 寄存器位域写入（`cs.b.priority`, `cs.b.panic_priority`, `cs.b.disdebug`, `cs.b.wait_for_outstanding_writes`）
- [x] 2.3 移除旧的裸 CB 地址校验逻辑，CB 地址始终从 `cfg->cb.dma`（或 union 任一成员）获取

## 3. 编译验证

- [x] 3.1 在 `xwbd/RPi4B` 目录执行 `source env.sh && xwm` 编译整个工程，确认无编译错误
- [x] 3.2 确认编译无警告
