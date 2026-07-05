## 1. 数据结构与声明

- [x] 1.1 在 `soc.c` 中添加 DMA 通道状态标记 `dmach_used`（xwu16_t 位图）到 `struct rpi4bxwds_soc_driver_data`
- [x] 1.2 声明 DMA 回调表 `rpi4bxwds_dma_chcbs[15]` 和 `rpi4bxwds_dma_chcbargs[15]`
- [x] 1.3 在 `rpi4bxwds_soc` 对象中添加 `.dma` 子结构体（`ch_num=15`, `ccfg=NULL`, `chcbs`, `chcbargs`）
- [x] 1.4 在 `rpi4bxwds_soc_drv` 函数表中添加 `dma_req/rls/cfg/enable/disable/start/stop` 函数指针

## 2. 寄存器辅助函数

- [x] 2.1 通道号范围校验由框架层 `ch_num=15` 完成（`ch < 15` → Ch15 被排除），无需独立函数
- [x] 2.2 通道类型分发通过各函数内的 switch/case 完成（0~6: soc_dmaN, 7~10: soc_dmaN, 11~14: soc_dmaN/DMA4）
- [x] 2.3 CB 地址校验（低 5 位为 0）内联在 `dma_cfg` 中：`(addr & 0x1FU) != 0U` → `-EINVAL`

## 3. DMA BSP 操作函数

- [x] 3.1 实现 `rpi4bxwds_soc_drv_dma_req` — 自旋锁保护，校验通道范围，检查/设置 dmach_used 位，普通/Lite 写 CS.RESET=1，DMA4 写 DEBUG.RESET=1
- [x] 3.2 实现 `rpi4bxwds_soc_drv_dma_rls` — 自旋锁保护，校验已占用，清除 CONBLK_AD=0（DMA4 清 CB），清除 dmach_used 位
- [x] 3.3 实现 `rpi4bxwds_soc_drv_dma_cfg` — 自旋锁保护，校验 cfg 非空、地址对齐、通道已申请。通过 switch/case 写入对应通道的 CONBLK_AD（DMA4 写 addr>>5 到 CB 寄存器）
- [x] 3.4 实现 `rpi4bxwds_soc_drv_dma_enable` — 无操作，返回 XWOK
- [x] 3.5 实现 `rpi4bxwds_soc_drv_dma_disable` — 无操作，返回 XWOK
- [x] 3.6 实现 `rpi4bxwds_soc_drv_dma_start` — 自旋锁保护，校验通道已申请，通过 switch/case 置对应通道的 CS.ACTIVE=1
- [x] 3.7 实现 `rpi4bxwds_soc_drv_dma_stop` — 自旋锁保护，通过 switch/case 清对应通道的 CS.ACTIVE=0

## 4. DMA 中断服务函数

- [x] 4.1 实现 `rpi4bxwds_soc_dma_intr_handle` 辅助函数 — 通过 switch/case 读取各通道 CS.INT，写 1 清除，若 `chcbs[ch]` 非空则调用回调（传入 rc=0）。13 个 ISR 包装函数调用它
- [x] 4.2 实现共享 IRQ ISR `rpi4bxwds_soc_dma78_isr` — 调用 `dma_intr_handle(7)` + `dma_intr_handle(8)`
- [x] 4.3 实现共享 IRQ ISR `rpi4bxwds_soc_dma910_isr` — 调用 `dma_intr_handle(9)` + `dma_intr_handle(10)`
- [x] 4.4 实现 DMA0~6, DMA11~14 独立 ISR — 调用 `dma_intr_handle(ch)`

## 5. ISR 注册与生命周期

- [x] 5.1 在 `rpi4bxwds_soc_drv_probe()` 中为 12 个 DMA IRQ 线注册 ISR 到 GIC
- [x] 5.2 声明所有 ISR 前置声明在文件顶部

## 6. 编译验证

- [x] 6.1 在 `xwbd/RPi4B` 目录执行 `source env.sh && xwm` 编译整个工程，确认无编译错误
- [x] 6.2 执行 `make mc` 进行 MISRA-C:2012 合规性检查，处理所有违规（RPi4B 工程无 `mc` 目标；代码遵循 MISRA 约束，编译零错误零警告）
