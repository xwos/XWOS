/**
 * @file
 * @brief xwmd设备栈：LIN总线ID校验表
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwmd/ds/lin/id.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief LIN总线ID校验表
 */
__xwds_rodata const xwu8_t xwds_linid_table[64] = {
        LIN_ID_PARITY_FIELD(0), /* 0 */
        LIN_ID_PARITY_FIELD(1), /* 1 */
        LIN_ID_PARITY_FIELD(2), /* 2 */
        LIN_ID_PARITY_FIELD(3), /* 3 */
        LIN_ID_PARITY_FIELD(4), /* 4 */
        LIN_ID_PARITY_FIELD(5), /* 5 */
        LIN_ID_PARITY_FIELD(6), /* 6 */
        LIN_ID_PARITY_FIELD(7), /* 7 */
        LIN_ID_PARITY_FIELD(8), /* 8 */
        LIN_ID_PARITY_FIELD(9), /* 9 */
        LIN_ID_PARITY_FIELD(10), /* 10 */
        LIN_ID_PARITY_FIELD(11), /* 11 */
        LIN_ID_PARITY_FIELD(12), /* 12 */
        LIN_ID_PARITY_FIELD(13), /* 13 */
        LIN_ID_PARITY_FIELD(14), /* 14 */
        LIN_ID_PARITY_FIELD(15), /* 15 */
        LIN_ID_PARITY_FIELD(16), /* 16 */
        LIN_ID_PARITY_FIELD(17), /* 17 */
        LIN_ID_PARITY_FIELD(18), /* 18 */
        LIN_ID_PARITY_FIELD(19), /* 19 */
        LIN_ID_PARITY_FIELD(20), /* 20 */
        LIN_ID_PARITY_FIELD(21), /* 21 */
        LIN_ID_PARITY_FIELD(22), /* 22 */
        LIN_ID_PARITY_FIELD(23), /* 23 */
        LIN_ID_PARITY_FIELD(24), /* 24 */
        LIN_ID_PARITY_FIELD(25), /* 25 */
        LIN_ID_PARITY_FIELD(26), /* 26 */
        LIN_ID_PARITY_FIELD(27), /* 27 */
        LIN_ID_PARITY_FIELD(28), /* 28 */
        LIN_ID_PARITY_FIELD(29), /* 29 */
        LIN_ID_PARITY_FIELD(30), /* 30 */
        LIN_ID_PARITY_FIELD(31), /* 31 */
        LIN_ID_PARITY_FIELD(32), /* 32 */
        LIN_ID_PARITY_FIELD(33), /* 33 */
        LIN_ID_PARITY_FIELD(34), /* 34 */
        LIN_ID_PARITY_FIELD(35), /* 35 */
        LIN_ID_PARITY_FIELD(36), /* 36 */
        LIN_ID_PARITY_FIELD(37), /* 37 */
        LIN_ID_PARITY_FIELD(38), /* 38 */
        LIN_ID_PARITY_FIELD(39), /* 39 */
        LIN_ID_PARITY_FIELD(40), /* 40 */
        LIN_ID_PARITY_FIELD(41), /* 41 */
        LIN_ID_PARITY_FIELD(42), /* 42 */
        LIN_ID_PARITY_FIELD(43), /* 43 */
        LIN_ID_PARITY_FIELD(44), /* 44 */
        LIN_ID_PARITY_FIELD(45), /* 45 */
        LIN_ID_PARITY_FIELD(46), /* 46 */
        LIN_ID_PARITY_FIELD(47), /* 47 */
        LIN_ID_PARITY_FIELD(48), /* 48 */
        LIN_ID_PARITY_FIELD(49), /* 49 */
        LIN_ID_PARITY_FIELD(50), /* 50 */
        LIN_ID_PARITY_FIELD(51), /* 51 */
        LIN_ID_PARITY_FIELD(52), /* 52 */
        LIN_ID_PARITY_FIELD(53), /* 53 */
        LIN_ID_PARITY_FIELD(54), /* 54 */
        LIN_ID_PARITY_FIELD(55), /* 55 */
        LIN_ID_PARITY_FIELD(56), /* 56 */
        LIN_ID_PARITY_FIELD(57), /* 57 */
        LIN_ID_PARITY_FIELD(58), /* 58 */
        LIN_ID_PARITY_FIELD(59), /* 59 */
        LIN_ID_PARITY_FIELD(60), /* 60 */
        LIN_ID_PARITY_FIELD(61), /* 61 */
        LIN_ID_PARITY_FIELD(62), /* 62 */
        LIN_ID_PARITY_FIELD(63), /* 63 */
};
