## 新增需求

### 需求:GPIO外部中断申请
系统必须支持通过 `xwds_eirq_req()` API 为 BCM2711 的任意 GPIO 引脚（0~57）申请外部中断。

#### 场景:上升沿触发
- **当** 调用 `xwds_eirq_req()` 且 `eiflag` 包含 `XWDS_SOC_EIF_TM_RISING`
- **那么** 系统配置对应引脚的异步上升沿检测寄存器（gparen），并在引脚检测到上升沿时触发 ISR

#### 场景:下降沿触发
- **当** 调用 `xwds_eirq_req()` 且 `eiflag` 包含 `XWDS_SOC_EIF_TM_FALLING`
- **那么** 系统配置对应引脚的异步下降沿检测寄存器（gpafen），并在引脚检测到下降沿时触发 ISR

#### 场景:边沿触发（上升和下降同时）
- **当** 调用 `xwds_eirq_req()` 且 `eiflag` 同时包含 `XWDS_SOC_EIF_TM_RISING` 和 `XWDS_SOC_EIF_TM_FALLING`
- **那么** 系统同时配置异步上升沿和下降沿检测，并在任意边沿时触发 ISR

#### 场景:高电平触发
- **当** 调用 `xwds_eirq_req()` 且 `eiflag` 包含 `XWDS_SOC_EIF_TM_HIGH`
- **那么** 系统配置对应引脚的高电平检测寄存器（gphen），并在引脚为高电平时触发 ISR

#### 场景:低电平触发
- **当** 调用 `xwds_eirq_req()` 且 `eiflag` 包含 `XWDS_SOC_EIF_TM_LOW`
- **那么** 系统配置对应引脚的低电平检测寄存器（gplen），并在引脚为低电平时触发 ISR

#### 场景:重复申请同一EIRQ ID
- **当** `eirq_req()` 被调用且该 `eiid` 已被占用
- **那么** 通用层 `xwds_eirq_req()` 应当返回 `-EBUSY`

#### 场景:EIRQ ID越界
- **当** `eiid` 超出 `soc->eirq.num`（当前为 58）
- **那么** 通用层 `xwds_eirq_req()` 应当返回 `-ERANGE`

### 需求:GPIO外部中断释放
系统必须支持通过 `xwds_eirq_rls()` API 释放已申请的外部中断。

#### 场景:正常释放
- **当** 调用 `xwds_eirq_rls()` 释放一个已申请的 EIRQ
- **那么** 系统清除对应引脚的异步检测寄存器，清除 gpeds 中待处理的事件状态，释放 ISR 表槽位

#### 场景:释放未申请的EIRQ ID
- **当** `eirq_rls()` 被调用且该 `eiid` 未申请
- **那么** 通用层 `xwds_eirq_rls()` 应当返回 `-EPERM`

### 需求:Bank级中断分发
系统必须为 BCM2711 GPIO 的 4 个中断 bank 分别注册 GIC 中断服务程序，并在 bank ISR 中完成 pins 级的事件分发。

#### 场景:单个引脚触发中断
- **当** GPIO Bank 0 中引脚 17 触发中断事件
- **那么** Bank 0 的 ISR 读取 gpeds0 寄存器，识别引脚 17 为事件源，清除 gpeds0 中对应位，调用 `soc->eirq.isrs[17]` 指向的用户 ISR

#### 场景:多个引脚同时触发中断
- **当** GPIO Bank 0 中引脚 5 和引脚 23 同时触发中断事件
- **那么** Bank 0 的 ISR 读取 gpeds0 寄存器，依次识别并调用引脚 5 和引脚 23 的用户 ISR，最后统一清除 gpeds0

### 需求:Boot时注册Bank ISR
系统必须在 SOC 驱动的 probe 阶段静态注册 4 个 bank ISR 到 GIC。

#### 场景:Probe时注册中断
- **当** `rpi4bxwds_soc_drv_probe()` 被调用
- **那么** 通过 `armv8a_gic_irq_set_isr()` 注册 GPIO0~3 四个 bank ISR，设置优先级和亲和性，通过 `armv8a_gic_irq_enable()` 使能

### 需求:驱动函数指针正确链接
SOC 驱动的 `eirq_req` 和 `eirq_rls` 函数指针不得为 NULL。

#### 场景:通用层调用驱动
- **当** `xwds_eirq_req()` 检查到 `drv->eirq_req` 不为 NULL
- **那么** 调用板级驱动函数完成硬件配置，返回 `XWOK`
