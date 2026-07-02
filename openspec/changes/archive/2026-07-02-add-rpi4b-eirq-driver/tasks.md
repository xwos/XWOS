## 1. 数据结构准备

- [x] 1.1 在 `soc.c` 中定义 58 元素的外部中断 ISR 表 `rpi4bxwds_eirq_isrs[]` 和参数表 `rpi4bxwds_eirq_isrargs[]`
- [x] 1.2 填充 `rpi4bxwds_soc` 的 `.eirq` 成员（`isrs`/`isrargs` 指针、`num=58`）

## 2. Bank ISR 实现

- [x] 2.1 实现 `rpi4bxwds_soc_eirq_bank0_isr()` — 读 gpeds0，遍历 0~31 引脚，查表调用用户 ISR，写回 gpeds0 清除状态
- [x] 2.2 实现 `rpi4bxwds_soc_eirq_bank1_isr()` — 读 gpeds1，遍历 32~57 引脚，查表调用用户 ISR，写回 gpeds1 清除状态
- [x] 2.3 实现 `rpi4bxwds_soc_eirq_bank2_isr()` 和 `rpi4bxwds_soc_eirq_bank3_isr()` — 保留，空处理（Bank 2/3 当前无 GPIO 映射）

## 3. EIRQ 申请/释放实现

- [x] 3.1 实现 `rpi4bxwds_soc_drv_eirq_req()` — 根据 `eiflag` 设置对应引脚的异步检测寄存器（`gparen`/`gpafen`/`gphen`/`gplen`），按 pin 号确定 bank 索引
- [x] 3.2 实现 `rpi4bxwds_soc_drv_eirq_rls()` — 清除对应引脚的所有检测使能位，清除 gpeds 中待处理事件

## 4. Probe 阶段注册 GIC ISR

- [x] 4.1 在 `rpi4bxwds_soc_drv_probe()` 中，参照 miniuart 模式，使用 `armv8a_gic_irq_set_isr()` 注册 4 个 bank ISR
- [x] 4.2 设置各 bank IRQ 的优先级、触发类型（`ARMV8A_IRQ_TRIGGER_TYPE_LEVEL`）和 CPU 亲和性

## 5. 驱动函数指针链接

- [x] 5.1 将 `rpi4bxwds_soc_drv` 的 `.eirq_req` 和 `.eirq_rls` 指向实现的函数

## 6. 编译验证

- [x] 6.1 在 RPi4B 板级目录编译工程 `xwm -B`，验证无编译错误
- [x] 6.2 运行 MISRA-C 检查 `make soc.mc`（如适用），验证无新增违规
