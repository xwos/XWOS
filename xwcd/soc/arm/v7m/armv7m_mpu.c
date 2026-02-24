/**
 * @file
 * @brief 架构描述层：ARMv7M MPU
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#include <xwcd/soc/arm/v7m/armv7m_mpu.h>
#include <xwcd/soc/arm/v7m/armv7m_isa.h>
#include <xwcd/soc/arm/v7m/xwosimpl_soc_isa.h>
#include <xwos/lib/errno.h>

/**
 * @brief 获取MPU的TYPE寄存器值
 * @param[out] type: 存放TYPE寄存器值的指针
 * @return 错误码
 * @retval XWOK: 成功
 * @retval -EFAULT: 参数错误
 */
xwer_t armv7m_mpu_get_type(xwu32_t * type)
{
        xwer_t rc;

        rc = XWOK;
        if (NULL == type) {
                rc = -EFAULT;
                goto err_type_null;
        }
        *type = armv7m_scs.scb.mpu.type.u32;
        return rc;

err_type_null:
        return rc;
}

/**
 * @brief 配置MPU的区域
 * @param[in] cfg: MPU区域配置数组指针
 * @param[in] num: 配置的区域数量
 * @return 错误码
 * @retval XWOK: 成功
 * @retval -EFAULT: 参数错误
 * @retval -EINVAL: 配置参数无效
 * @retval -ERANGE: 区域编号超出范围
 */
xwer_t armv7m_mpu_set(struct armv7m_mpu_config * cfg, xwu8_t num)
{
        xwer_t rc;
        xwu32_t mputype;
        xwu8_t max_regions;
        xwu8_t i;
        xwu32_t rbar;
        xwu32_t rasr;
        xwu32_t size_field;
        xwu32_t size_value;

        rc = XWOK;

        /* 参数验证 */
        if ((NULL == cfg) || (0 == num)) {
                rc = -EFAULT;
                goto err_param;
        }

        /* 获取MPU支持的最大区域数 */
        mputype = armv7m_scs.scb.mpu.type.u32;
        max_regions = (xwu8_t)((mputype & ARMV7M_MPU_TYPE_DREGION_MSK) >>
                               ARMV7M_MPU_TYPE_DREGION_POS);

        /* 配置每个区域 */
        for (i = 0; i < num; i++) {
                /* 检查区域编号是否有效 */
                if (cfg[i].rnr >= max_regions) {
                        rc = -ERANGE;
                        goto err_region_outofrange;
                }

                /* 提取SIZE字段的值 */
                size_field = cfg[i].size & ARMV7M_MPU_RASR_SIZE_MSK;
                size_value = (size_field >> ARMV7M_MPU_RASR_SIZE_POS);

                /* 检查SIZE值是否有效（1-31） */
                if ((size_value < 1) || (size_value > 31)) {
                        rc = -EINVAL;
                        goto err_invalid_size;
                }

                /* 检查基地址是否按大小对齐 */
                /* 区域大小 = 2^(SIZE+1)字节 */
                if (cfg[i].base_addr & ((1U << (size_value + 1)) - 1)) {
                        rc = -EALIGN;
                        goto err_addr_not_aligned;
                }

                /* 构建RBAR寄存器值 */
                rbar = (cfg[i].base_addr & ARMV7M_MPU_RBAR_ADDR_MSK) |
                       ((cfg[i].rnr & 0xFU) << ARMV7M_MPU_RBAR_REGION_POS) |
                       ARMV7M_MPU_RBAR_VALID_MSK;

                /* 构建RASR寄存器值 */
                rasr = cfg[i].size | cfg[i].attr |
                       ((cfg[i].srd & 0xFFU) << ARMV7M_MPU_RASR_SRD_POS);
                if (cfg[i].en) {
                        rasr |= ARMV7M_MPU_RASR_ENABLE_MSK;
                }

                /* 配置区域 */
                armv7m_dsb();
                armv7m_scs.scb.mpu.rnr.u32 = cfg[i].rnr;
                armv7m_scs.scb.mpu.rbar.u32 = rbar;
                armv7m_scs.scb.mpu.rasr.u32 = rasr;
                armv7m_dsb();
                armv7m_isb();
        }

        return rc;

err_addr_not_aligned:
err_invalid_size:
err_region_outofrange:
err_param:
        return rc;
}

/**
 * @brief 重置MPU的所有区域（禁用所有区域）
 */
void armv7m_mpu_reset(void)
{
        xwu32_t mputype;
        xwu8_t max_regions;
        xwu8_t i;

        /* 获取MPU支持的最大区域数 */
        mputype = armv7m_scs.scb.mpu.type.u32;
        max_regions = (xwu8_t)((mputype & ARMV7M_MPU_TYPE_DREGION_MSK) >>
                               ARMV7M_MPU_TYPE_DREGION_POS);

        /* 禁用所有区域 */
        for (i = 0; i < max_regions; i++) {
                armv7m_dsb();
                armv7m_scs.scb.mpu.rnr.u32 = i;
                armv7m_scs.scb.mpu.rasr.u32 = 0;
                armv7m_dsb();
                armv7m_isb();
        }
}

/**
 * @brief 禁用MPU
 */
void armv7m_mpu_disable(void)
{
        xwu32_t ctrl;

        /* 读取当前CTRL寄存器值 */
        ctrl = armv7m_scs.scb.mpu.ctrl.u32;
        /* 清除ENABLE位 */
        ctrl &= ~ARMV7M_MPU_CTRL_ENABLE_MSK;
        /* 写入CTRL寄存器 */
        armv7m_dsb();
        armv7m_scs.scb.mpu.ctrl.u32 = ctrl;
        armv7m_dsb();
        armv7m_isb();
}

/**
 * @brief 启用MPU
 */
void armv7m_mpu_enable(void)
{
        xwu32_t ctrl;

        /* 读取当前CTRL寄存器值 */
        ctrl = armv7m_scs.scb.mpu.ctrl.u32;
        /* 设置ENABLE位 */
        ctrl |= ARMV7M_MPU_CTRL_ENABLE_MSK;
        /* 写入CTRL寄存器 */
        armv7m_dsb();
        armv7m_scs.scb.mpu.ctrl.u32 = ctrl;
        armv7m_dsb();
        armv7m_isb();
}