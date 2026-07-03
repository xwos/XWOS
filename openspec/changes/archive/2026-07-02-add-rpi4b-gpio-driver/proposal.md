## 为什么

树莓派4B (BCM2711) 板级支持包目前缺少 GPIO 驱动。配置文件中 `XWCDCFG_ds_SOC_GPIO=1` 已启用，但对应的 BSP 实现层（`board/xwac/xwds/soc.c`）尚未创建，导致上层设备驱动（如 SPI、I2C）无法通过标准 SOC 框架申请和控制 GPIO。

## 变更内容

- 新建 `board/xwac/xwds/soc.c`，实现 BCM2711 芯片的 `struct xwds_soc_driver` 驱动函数表
- 在 `device.h` 中声明 SOC 设备实例和 init/fini 函数
- 在 `board/init.c` 的 `xwos_postinit()` 中集成 SOC 设备初始化
- GPIO 操作：req, rls, cfg（功能选择+上下拉）, set, reset, toggle, output, input
- SMP 自旋锁保护（`struct xwos_splk`）
- 遵循 MISRA-C:2012 的 return/goto 约束

## 功能

### 新增功能

- `gpio-driver`: 为 BCM2711 芯片提供 SOC 层面的 GPIO 驱动，支持 pin 0~57 的功能选择、电平控制（set/reset/toggle/output）和输入读取，Port 参数忽略（芯片为单平面 GPIO 控制器）

### 修改功能

_无_

## 影响

- **新建**: `board/xwac/xwds/soc.c` — GPIO 驱动主体文件
- **修改**: `board/xwac/xwds/device.h` — 添加 `rpi4bxwds_soc` 实例声明和 `rpi4bxwds_soc_init/fini` 函数声明
- **修改**: `board/init.c` — 在 `xwos_postinit()` 中添加 SOC 设备初始化调用
- **依赖**: `xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h`（寄存器定义）、`xwcd/ds/soc/chip.h`（驱动框架）、`xwos/osal/lock/spinlock.h`（自旋锁）
