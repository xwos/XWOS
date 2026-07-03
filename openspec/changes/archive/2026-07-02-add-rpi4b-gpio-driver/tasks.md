## 1. 创建 GPIO 驱动主体文件

- [x] 1.1 创建 `board/xwac/xwds/soc.c`，包含文件头和 `#include` 依赖
- [x] 1.2 定义 GPIO 配置结构体 `struct rpi4bxwds_gpio_cfg`（function + pud）和驱动数据结构体 `struct rpi4bxwds_soc_driver_data`（splk）
- [x] 1.3 声明全局实例：`rpi4bxwds_gpio_pin_state` 数组、`rpi4bxwds_soc_drvdata`、EIRQ 空表、`rpi4bxwds_soc` 设备对象、`rpi4bxwds_soc_drv` 驱动函数表

## 2. 实现寄存器访问辅助函数

- [x] 2.1 实现 `rpi4bxwds_gpfsel_reg_get/set` — 通过 switch(pin/10) 分发到 `gpfsel0~5`，使用 `u32` 做 RMW 操作
- [x] 2.2 实现 `rpi4bxwds_pupd_reg_get/set` — 通过 switch(pin/16) 分发到 `pup_pdn_cntrl_reg0~3`，使用 `u32` 做 RMW 操作

## 3. 实现 GPIO 操作函数

- [x] 3.1 实现 `rpi4bxwds_soc_drv_gpio_req` 和 `rpi4bxwds_soc_drv_gpio_rls` — 直通函数，忽略 port/pinmask 返回 XWOK
- [x] 3.2 实现 `rpi4bxwds_soc_drv_gpio_cfg` — 自旋锁保护下，对 pinmask 中的每个 PIN 迭代调用 fsel 和 pupd 配置
- [x] 3.3 实现 `rpi4bxwds_soc_drv_gpio_set` 和 `rpi4bxwds_soc_drv_gpio_reset` — 自旋锁保护下，低32位写 gpset0/gpclr0，高位写 gpset1/gpclr1
- [x] 3.4 实现 `rpi4bxwds_soc_drv_gpio_toggle` — 自旋锁保护下，读 gplev，反写 set/clr
- [x] 3.5 实现 `rpi4bxwds_soc_drv_gpio_output` — 自旋锁保护下，读 gplev，计算差分，写 set/clr
- [x] 3.6 实现 `rpi4bxwds_soc_drv_gpio_input` — 读 gplev0/1，按 pinmask 掩码返回

## 4. 实现设备生命周期函数

- [x] 4.1 实现 `rpi4bxwds_soc_drv_probe` — 初始化自旋锁
- [x] 4.2 实现 `rpi4bxwds_soc_drv_remove` — 空操作
- [x] 4.3 实现 `rpi4bxwds_soc_drv_start` 和 `rpi4bxwds_soc_drv_stop` — 空操作
- [x] 4.4 实现 `rpi4bxwds_soc_init` — construct → probe → start，仿照 UART init 的 goto 错误处理模式
- [x] 4.5 实现 `rpi4bxwds_soc_fini` — stop → remove → destruct

## 5. 集成到板级代码

- [x] 5.1 修改 `device.h` — 声明 `rpi4bxwds_soc`、`rpi4bxwds_soc_init()`、`rpi4bxwds_soc_fini()`
- [x] 5.2 修改 `board/init.c` — 在 `xwos_postinit()` 的 CPU0 分支中添加 `rpi4bxwds_soc_init()` 调用

## 6. 编译验证

- [x] 6.1 在 `xwbd/RPi4B` 目录执行 `source env.sh && xwm` 编译整个工程，确认无编译错误
- [x] 6.2 执行 `make mc` 进行 MISRA-C:2012 合规性检查，处理所有违规
