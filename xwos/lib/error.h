/**
 * @file
 * @brief XWOS通用库：错误码
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_error_h__
#define __xwos_lib_error_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/type.h>
#include <xwos/lib/errno.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define MAX_ERRNO       4095

#define IS_ERR_VALUE(x) (((xwptr_t)(x)) >= (xwptr_t)(-MAX_ERRNO))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 将错误码转换为指针
 * @param err: (I) 错误码
 * @return 值为错误码的指针
 */
static __xw_inline
void * __must_check err_ptr(xwer_t err)
{
        return (void *)err;
}

/**
 * @brief 将指针的值转换为错误码
 * @param ptr: (I) 指针
 * @return 错误码
 */
static __xw_inline
xwer_t __must_check ptr_err(__force const void * ptr)
{
        return (xwer_t)ptr;
}

/**
 * @brief 测试指针的值是否为错误码
 * @param ptr: (I) 指针
 * @retval true: 是
 * @retval false: 否
 */
static __xw_inline
bool __must_check is_err(__force const void * ptr)
{
        return IS_ERR_VALUE(ptr)? true : false;
}

/**
 * @brief 测试指针的值是否为错误码或空(指针是否为无效指针)
 * @param ptr: (I) 指针
 * @retval true: 是
 * @retval false: 否
 */
static __xw_inline
bool __must_check is_err_or_null(__force const void * ptr)
{
        return (!ptr || IS_ERR_VALUE(ptr))? true : false;
}

#endif /* xwos/lib/error.h */
