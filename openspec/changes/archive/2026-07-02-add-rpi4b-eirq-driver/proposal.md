## 为什么

RPi4B/BCM2711 已在 `cfg/xwcd.h` 中启用 `XWCDCFG_ds_SOC_EIRQ=1`，但板级 SOC 驱动中 `eirq_req` 和 `eirq_rls` 函数指针为 NULL，`.eirq` 成员（ISR 表、参数表、中断数量）全为零。GPIO 外部中断功能完全不可用，无法响应 GPIO 引脚上的边沿或电平事件。

## 变更内容

- 实现 `rpi4bxwds_soc_drv_eirq_req()` — 配置 BCM2711 GPIO 异步检测寄存器并路由中断
- 实现 `rpi4bxwds_soc_drv_eirq_rls()` — 清除 GPIO 检测配置，释放中断资源
- 实现 4 个 GPIO bank 中断服务程序（Bank 0~3），读 gpeds 状态分发到用户 ISR
- 填充 `rpi4bxwds_soc.eirq` 成员，建立 58 路外部中断的 ISR 表和参数表
- 在 probe 阶段静态注册 4 个 bank ISR 到 GIC（参考 miniuart 模式）
- 全部使用异步检测寄存器（gparen/gpafen/gphen/gplen），无需外设时钟

## 功能 (Capabilities)

### 新增功能
- `rpi4b-gpio-eirq`: 支持 BCM2711 的 58 根 GPIO 引脚（0~57）的外部中断，包括上升沿、下降沿、高电平、低电平四种触发模式，通过 XWDS 标准 EIRQ API 提供。

### 修改功能
<!-- 无现有功能需求变更 -->

## 影响

- 仅修改 `xwbd/RPi4B/board/xwac/xwds/soc.c`，在现有 SOC 驱动中增加 ~200 行 EIRQ 实现
- 依赖现有的 `arch_gic2.h`（GIC ISR 注册接口）和 `soc_gpio.h`（GPIO 寄存器定义），无需新增依赖
