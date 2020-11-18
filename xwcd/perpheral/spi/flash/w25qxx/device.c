/**
 * @file
 * @brief SPI Flash W25Qxx Device
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

#include <xwcd/perpheral/spi/flash/w25qxx/device.h>

void xwds_w25qxx_construct(struct xwds_w25qxx * w25qxx)
{
        xwds_spip_construct(&w25qxx->spip);
}

void xwds_w25qxx_destruct(struct xwds_w25qxx * w25qxx)
{
        xwds_spip_destruct(&w25qxx->spip);
}
