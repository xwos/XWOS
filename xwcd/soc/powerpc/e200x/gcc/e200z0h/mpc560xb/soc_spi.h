/**
 * @file
 * @brief SOC描述层：SPI
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

#ifndef __soc_spi_h__
#define __soc_spi_h__

#include <xwos/standard.h>
#include <soc.h>

/* baudrate scaler */
#define SOC_SPI_CLKDIV_PBR_2            0
#define SOC_SPI_CLKDIV_PBR_3            1
#define SOC_SPI_CLKDIV_PBR_5            2
#define SOC_SPI_CLKDIV_PBR_7            3

#define SOC_SPI_CLKDIV_BR_2             0
#define SOC_SPI_CLKDIV_BR_4             1
#define SOC_SPI_CLKDIV_BR_6             2
#define SOC_SPI_CLKDIV_BR_8             3
#define SOC_SPI_CLKDIV_BR_16            4
#define SOC_SPI_CLKDIV_BR_32            5
#define SOC_SPI_CLKDIV_BR_64            6
#define SOC_SPI_CLKDIV_BR_128           7
#define SOC_SPI_CLKDIV_BR_256           8
#define SOC_SPI_CLKDIV_BR_512           9
#define SOC_SPI_CLKDIV_BR_1024          10
#define SOC_SPI_CLKDIV_BR_2048          11
#define SOC_SPI_CLKDIV_BR_4096          12
#define SOC_SPI_CLKDIV_BR_8192          13
#define SOC_SPI_CLKDIV_BR_16384         14
#define SOC_SPI_CLKDIV_BR_32768         15

#define SOC_SPI_CLKDIV_BR_DOUBLE        1

/* CS to SCK delay scaler */
#define SOC_SPI_CLKDIV_PCSSCK_1         0
#define SOC_SPI_CLKDIV_PCSSCK_3         1
#define SOC_SPI_CLKDIV_PCSSCK_5         2
#define SOC_SPI_CLKDIV_PCSSCK_7         3

#define SOC_SPI_CLKDIV_CSSCK_2          0
#define SOC_SPI_CLKDIV_CSSCK_4          1
#define SOC_SPI_CLKDIV_CSSCK_8          2
#define SOC_SPI_CLKDIV_CSSCK_16         3
#define SOC_SPI_CLKDIV_CSSCK_32         4
#define SOC_SPI_CLKDIV_CSSCK_64         5
#define SOC_SPI_CLKDIV_CSSCK_128        6
#define SOC_SPI_CLKDIV_CSSCK_256        7
#define SOC_SPI_CLKDIV_CSSCK_512        8
#define SOC_SPI_CLKDIV_CSSCK_1024       9
#define SOC_SPI_CLKDIV_CSSCK_2048       10
#define SOC_SPI_CLKDIV_CSSCK_4096       11
#define SOC_SPI_CLKDIV_CSSCK_8192       12
#define SOC_SPI_CLKDIV_CSSCK_16384      13
#define SOC_SPI_CLKDIV_CSSCK_32768      14
#define SOC_SPI_CLKDIV_CSSCK_65536      15

/* After SCK delay scaler */
#define SOC_SPI_CLKDIV_PASC_1           0
#define SOC_SPI_CLKDIV_PASC_3           1
#define SOC_SPI_CLKDIV_PASC_5           2
#define SOC_SPI_CLKDIV_PASC_7           3

#define SOC_SPI_CLKDIV_ASC_2            0
#define SOC_SPI_CLKDIV_ASC_4            1
#define SOC_SPI_CLKDIV_ASC_8            2
#define SOC_SPI_CLKDIV_ASC_16           3
#define SOC_SPI_CLKDIV_ASC_32           4
#define SOC_SPI_CLKDIV_ASC_64           5
#define SOC_SPI_CLKDIV_ASC_128          6
#define SOC_SPI_CLKDIV_ASC_256          7
#define SOC_SPI_CLKDIV_ASC_512          8
#define SOC_SPI_CLKDIV_ASC_1024         9
#define SOC_SPI_CLKDIV_ASC_2048         10
#define SOC_SPI_CLKDIV_ASC_4096         11
#define SOC_SPI_CLKDIV_ASC_8192         12
#define SOC_SPI_CLKDIV_ASC_16384        13
#define SOC_SPI_CLKDIV_ASC_32768        14
#define SOC_SPI_CLKDIV_ASC_65536        15

/* delay after transfer scaler */
#define SOC_SPI_CLKDIV_PDT_1            0
#define SOC_SPI_CLKDIV_PDT_3            1
#define SOC_SPI_CLKDIV_PDT_5            2
#define SOC_SPI_CLKDIV_PDT_7            3

#define SOC_SPI_CLKDIV_DT_2             0
#define SOC_SPI_CLKDIV_DT_4             1
#define SOC_SPI_CLKDIV_DT_8             2
#define SOC_SPI_CLKDIV_DT_16            3
#define SOC_SPI_CLKDIV_DT_32            4
#define SOC_SPI_CLKDIV_DT_64            5
#define SOC_SPI_CLKDIV_DT_128           6
#define SOC_SPI_CLKDIV_DT_256           7
#define SOC_SPI_CLKDIV_DT_512           8
#define SOC_SPI_CLKDIV_DT_1024          9
#define SOC_SPI_CLKDIV_DT_2048          10
#define SOC_SPI_CLKDIV_DT_4096          11
#define SOC_SPI_CLKDIV_DT_8192          12
#define SOC_SPI_CLKDIV_DT_16384         13
#define SOC_SPI_CLKDIV_DT_32768         14
#define SOC_SPI_CLKDIV_DT_65536         15

/**
 * @brief mpc560xb spi private configurations
 */
struct soc_spi_private_cfg {
        union {
                struct {
                        xwu32_t pcssck:2; /**< PCS to SCK delay prescaler */
                        xwu32_t cssck:4; /**< PCS to SCK delay scaler */
                        xwu32_t pasc:2; /**< delay after transfer prescaler */
                        xwu32_t asc:4; /**< delay after transfer scaler */
                        xwu32_t pdt:2; /**< delay after transfer prescaler */
                        xwu32_t dt:4; /**< delay after transfer Scaler */
                        xwu32_t pbr:2; /**< baudrate prescaler */
                        xwu32_t br:4; /**< baudrate scaler */
                        xwu32_t dbr:1; /**< double baudrate */
                } bit;
                xwu32_t u32;
        } clkdiv; /**< clock divisor */
        bool continuous_sck; /**< continuous SCK */
};

#endif /* soc_spi.h */
