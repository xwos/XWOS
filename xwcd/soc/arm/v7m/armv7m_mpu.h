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

#ifndef __xwcd_soc_arm_v7m_armv7m_mpu_h__
#define __xwcd_soc_arm_v7m_armv7m_mpu_h__

#include <xwos/standard.h>
#include <xwos/lib/errno.h>

/******** ******** MPU寄存器位域定义 ******** ********/

/******** MPU CTRL寄存器位域 ********/
#define ARMV7M_MPU_CTRL_PRIVDEFENA_POS 2
#define ARMV7M_MPU_CTRL_PRIVDEFENA_MSK (1U << ARMV7M_MPU_CTRL_PRIVDEFENA_POS)
#define ARMV7M_MPU_CTRL_HFNMIENA_POS 1
#define ARMV7M_MPU_CTRL_HFNMIENA_MSK (1U << ARMV7M_MPU_CTRL_HFNMIENA_POS)
#define ARMV7M_MPU_CTRL_ENABLE_POS 0
#define ARMV7M_MPU_CTRL_ENABLE_MSK (1U << ARMV7M_MPU_CTRL_ENABLE_POS)

/******** MPU TYPE寄存器位域 ********/
#define ARMV7M_MPU_TYPE_SEPARATE_POS 0
#define ARMV7M_MPU_TYPE_SEPARATE_MSK (1U << ARMV7M_MPU_TYPE_SEPARATE_POS)
#define ARMV7M_MPU_TYPE_DREGION_POS 8
#define ARMV7M_MPU_TYPE_DREGION_MSK (0xFFU << ARMV7M_MPU_TYPE_DREGION_POS)

/******** MPU RBAR寄存器位域 ********/
#define ARMV7M_MPU_RBAR_REGION_POS 0
#define ARMV7M_MPU_RBAR_REGION_MSK (0xFU << ARMV7M_MPU_RBAR_REGION_POS)
#define ARMV7M_MPU_RBAR_VALID_POS 4
#define ARMV7M_MPU_RBAR_VALID_MSK (1U << ARMV7M_MPU_RBAR_VALID_POS)
#define ARMV7M_MPU_RBAR_ADDR_POS 5
#define ARMV7M_MPU_RBAR_ADDR_MSK (0x7FFFFFFU << ARMV7M_MPU_RBAR_ADDR_POS)

/******** MPU RASR寄存器位域 ********/
#define ARMV7M_MPU_RASR_ENABLE_POS 0
#define ARMV7M_MPU_RASR_ENABLE_MSK (1U << ARMV7M_MPU_RASR_ENABLE_POS)
#define ARMV7M_MPU_RASR_SIZE_POS 1
#define ARMV7M_MPU_RASR_SIZE_MSK (0x1FU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_RASR_SRD_POS 8
#define ARMV7M_MPU_RASR_SRD_MSK (0xFFU << ARMV7M_MPU_RASR_SRD_POS)
#define ARMV7M_MPU_RASR_B_POS 16
#define ARMV7M_MPU_RASR_B_MSK (1U << ARMV7M_MPU_RASR_B_POS)
#define ARMV7M_MPU_RASR_C_POS 17
#define ARMV7M_MPU_RASR_C_MSK (1U << ARMV7M_MPU_RASR_C_POS)
#define ARMV7M_MPU_RASR_S_POS 18
#define ARMV7M_MPU_RASR_S_MSK (1U << ARMV7M_MPU_RASR_S_POS)
#define ARMV7M_MPU_RASR_TEX_POS 19
#define ARMV7M_MPU_RASR_TEX_MSK (0x7U << ARMV7M_MPU_RASR_TEX_POS)
#define ARMV7M_MPU_RASR_AP_POS 24
#define ARMV7M_MPU_RASR_AP_MSK (0x7U << ARMV7M_MPU_RASR_AP_POS)
#define ARMV7M_MPU_RASR_XN_POS 28
#define ARMV7M_MPU_RASR_XN_MSK (1U << ARMV7M_MPU_RASR_XN_POS)

/******** ******** 内存属性宏定义 ******** ********/

/******** 内存类型宏定义（TEX, C, B的组合）********/
/* 强序内存（Strongly Ordered）*/
#define ARMV7M_MPU_ATTR_SO ((0x0U << ARMV7M_MPU_RASR_TEX_POS))
/* 设备内存（Device）*/
#define ARMV7M_MPU_ATTR_DEVICE                                                 \
        ((0x0U << ARMV7M_MPU_RASR_TEX_POS) | (0x0U << ARMV7M_MPU_RASR_C_POS) | \
         (0x1U << ARMV7M_MPU_RASR_B_POS))
/* 普通内存，非缓存（Normal, non-cacheable）*/
#define ARMV7M_MPU_ATTR_NORMAL_NC                                              \
        ((0x0U << ARMV7M_MPU_RASR_TEX_POS) | (0x1U << ARMV7M_MPU_RASR_C_POS) | \
         (0x0U << ARMV7M_MPU_RASR_B_POS))
/* 普通内存，写穿透（Normal, write-through）*/
#define ARMV7M_MPU_ATTR_NORMAL_WT                                              \
        ((0x1U << ARMV7M_MPU_RASR_TEX_POS) | (0x1U << ARMV7M_MPU_RASR_C_POS) | \
         (0x0U << ARMV7M_MPU_RASR_B_POS))
/* 普通内存，写回（Normal, write-back）*/
#define ARMV7M_MPU_ATTR_NORMAL_WB                                              \
        ((0x2U << ARMV7M_MPU_RASR_TEX_POS) | (0x1U << ARMV7M_MPU_RASR_C_POS) | \
         (0x0U << ARMV7M_MPU_RASR_B_POS))
/* 普通内存，写穿透，读分配（Normal, write-through, read-allocate）*/
#define ARMV7M_MPU_ATTR_NORMAL_WTRA                                            \
        ((0x1U << ARMV7M_MPU_RASR_TEX_POS) | (0x1U << ARMV7M_MPU_RASR_C_POS) | \
         (0x1U << ARMV7M_MPU_RASR_B_POS))
/* 普通内存，写回，读分配（Normal, write-back, read-allocate）*/
#define ARMV7M_MPU_ATTR_NORMAL_WBRA                                            \
        ((0x2U << ARMV7M_MPU_RASR_TEX_POS) | (0x1U << ARMV7M_MPU_RASR_C_POS) | \
         (0x1U << ARMV7M_MPU_RASR_B_POS))
/* 普通内存，写回，读写分配（Normal, write-back, write-allocate）*/
#define ARMV7M_MPU_ATTR_NORMAL_WBWA                                            \
        ((0x3U << ARMV7M_MPU_RASR_TEX_POS) | (0x1U << ARMV7M_MPU_RASR_C_POS) | \
         (0x1U << ARMV7M_MPU_RASR_B_POS))

/******** 共享属性宏定义 ********/
#define ARMV7M_MPU_ATTR_SHAREABLE (1U << ARMV7M_MPU_RASR_S_POS)
#define ARMV7M_MPU_ATTR_NONSHAREABLE (0U << ARMV7M_MPU_RASR_S_POS)

/******** 访问权限宏定义（AP[2:0]）********/
/* 无访问权限（特权模式下无访问，用户模式下无访问）*/
#define ARMV7M_MPU_AP_NO_ACCESS (0x0U << ARMV7M_MPU_RASR_AP_POS)
/* 特权模式读写，用户模式无访问 */
#define ARMV7M_MPU_AP_PRIV_RW_USR_NONE (0x1U << ARMV7M_MPU_RASR_AP_POS)
/* 特权模式读写，用户模式只读 */
#define ARMV7M_MPU_AP_PRIV_RW_USR_RO (0x2U << ARMV7M_MPU_RASR_AP_POS)
/* 特权模式读写，用户模式读写 */
#define ARMV7M_MPU_AP_PRIV_RW_USR_RW (0x3U << ARMV7M_MPU_RASR_AP_POS)
/* 特权模式只读，用户模式无访问 */
#define ARMV7M_MPU_AP_PRIV_RO_USR_NONE (0x5U << ARMV7M_MPU_RASR_AP_POS)
/* 特权模式只读，用户模式只读 */
#define ARMV7M_MPU_AP_PRIV_RO_USR_RO (0x6U << ARMV7M_MPU_RASR_AP_POS)
/* 仅特权模式访问（与AP=0相同）*/
#define ARMV7M_MPU_AP_PRIV_ONLY (0x7U << ARMV7M_MPU_RASR_AP_POS)

/******** 执行权限宏定义 ********/
#define ARMV7M_MPU_XN_ENABLE (1U << ARMV7M_MPU_RASR_XN_POS)
#define ARMV7M_MPU_XN_DISABLE (0U << ARMV7M_MPU_RASR_XN_POS)

/******** 区域大小宏定义（大小 = 2^(SIZE+1)字节）********/
#define ARMV7M_MPU_SIZE_32B (0x04U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_64B (0x05U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_128B (0x06U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_256B (0x07U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_512B (0x08U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_1KB (0x09U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_2KB (0x0AU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_4KB (0x0BU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_8KB (0x0CU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_16KB (0x0DU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_32KB (0x0EU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_64KB (0x0FU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_128KB (0x10U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_256KB (0x11U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_512KB (0x12U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_1MB (0x13U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_2MB (0x14U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_4MB (0x15U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_8MB (0x16U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_16MB (0x17U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_32MB (0x18U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_64MB (0x19U << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_128MB (0x1AU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_256MB (0x1BU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_512MB (0x1CU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_1GB (0x1DU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_2GB (0x1EU << ARMV7M_MPU_RASR_SIZE_POS)
#define ARMV7M_MPU_SIZE_4GB (0x1FU << ARMV7M_MPU_RASR_SIZE_POS)

/******** ******** MPU配置结构体 ******** ********/

/**
 * @brief MPU区域配置结构体
 */
struct armv7m_mpu_config {
        xwu32_t base_addr; /**< 区域基地址（必须按区域大小对齐）*/
        xwu32_t size; /**< 区域大小（使用ARMV7M_MPU_SIZE_xxx宏）*/
        xwu32_t attr; /**< 区域属性（内存类型+共享+访问权限+执行权限的组合）*/
        xwu8_t en; /**< 区域使能（非0表示使能）*/
        xwu8_t rnr; /**< 区域编号（0-255）*/
        xwu8_t srd; /**< 子区域禁用位图（bit0对应子区域0，bit7对应子区域7）*/
};

/******** ******** MPU API声明 ******** ********/

/**
 * @brief 获取MPU的TYPE寄存器值
 * @param[out] type: 存放TYPE寄存器值的指针
 * @return 错误码
 * @retval XWOK: 成功
 * @retval -EFAULT: 参数错误
 */
xwer_t armv7m_mpu_get_type(xwu32_t * type);

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
xwer_t armv7m_mpu_set(struct armv7m_mpu_config * cfg, xwu8_t num);

/**
 * @brief 重置MPU的所有区域（禁用所有区域）
 */
void armv7m_mpu_reset(void);

/**
 * @brief 禁用MPU
 */
void armv7m_mpu_disable(void);

/**
 * @brief 启用MPU
 */
void armv7m_mpu_enable(void);

#endif /* xwcd/soc/arm/v7m/armv7m_mpu.h */