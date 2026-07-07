
# XWOS

[Switch to English](README.EN.md)

玄武操作系统（XWOS®）开发于2015年，是一款运行在嵌入式微控制器上的通用实时操作系统，
主要面向可靠、实时、安全的应用领域，包括汽车、物联网、工控等。

XWOS的源码遵循 **MISRA-C:2012** 标准，已经在汽车行业有越来越多的量产项目。

+ <https://xwos.tech/>
+ <http://xwos.tech/>
+ <https://xwos.org/>
+ <http://xwos.org/>

# 示例工程

XWOS提供了比较多的示例工程：

+ [树莓派4B](https://xwos.tech/Docs/UserManual/Board/RPi4B/)
+ STM32
  + [STM32F072C8XWOS](https://gitee.com/xwos/STM32F072C8XWOS.git)：STM32F072C8最小系统
  + [STM32F103C8XWOS](https://gitee.com/xwos/STM32F103C8XWOS.git)：STM32F103C8最小系统
  + [ATKSTM32F103ZXWOS](https://gitee.com/xwos/ATKSTM32F103ZXWOS.git)：正点原子F103核心板
  + [ATKSTM32F407ZXWOS](https://gitee.com/xwos/ATKSTM32F407ZXWOS.git)：正点原子F407核心板
  + [EmbedFireStm32H743XWOS](https://gitee.com/xwos/EmbedFireStm32H743XWOS.git)：野火STM32H743-Pro开发板
  + [AtkApolloH743XWOS](https://gitee.com/xwos/AtkApolloH743XWOS.git)：正点原子阿波罗STM32H743开发板
  + [FK429M1XWOS](https://gitee.com/xwos/FK429M1XWOS.git)：反客STM32F429-M1开发板
  + [WeActMiniStm32H750XWOS](https://gitee.com/xwos/WeActMiniStm32H750XWOS.git)：微行电子MiniStm32H750开发板
+ Flagchip
  + [Fc7300DemoBoard](https://xwos.tech/Docs/UserManual/Board/Flagchip/Fc7300DemoBoard/)：FC7300 Demo Board
+ GD32
  + [GD32F103CBXWOS](https://gitee.com/xwos/GD32F103CBXWOS.git)：GD32F103CB核心板
  + [GD32F303CCXWOS](https://gitee.com/xwos/GD32F303CCXWOS.git)：GD32F303CC核心板
+ S32K
  + [S32K1XWOS](https://gitee.com/xwos/S32K1XWOS.git)：S32K144官方评估板
  + [S32K3XWOS](https://gitee.com/xwos/S32K3XWOS.git)：S32K312官方评估板
+ GD32V
  + [GD32VF103XWOS](https://gitee.com/xwos/GD32VF103XWOS.git)：Longan nano开发板
+ APM32
  + [APM32F103CBXWOS](https://gitee.com/xwos/APM32F103CBXWOS.git)：APM32F103CB核心板
+ CH32
  + [CH32F103C8XWOS](https://gitee.com/xwos/CH32F103C8XWOS.git)：CH32F103CB核心板
+ Embed PowerPC
  + [MPC5607BXWOS](https://gitee.com/xwos/MPC5607BXWOS.git)：MPC5607B核心板


# 开源协议

+ **XWOS核心代码**
  + XWOS核心代码采用[ **MPL-2.0** ](http://mozilla.org/MPL/2.0/)发布；
  + 适用范围： `xwos` 、 `xwmd` 文件夹下的源码；
+ **XWOS的外围代码**
  + XWOS的外围代码采用[ **Apache-2.0** ](http://www.apache.org/licenses/LICENSE-2.0)发布；
  + 适用范围： `xwcd` 、 `xwbd` 、 `xwam` 、 `xwbs` 、 `xwxr` 文件夹下的源码。
+ **第三方软件**
  + 适用范围： `xwem` 文件夹下的源码以及 `xwbd` 中第三方提供的驱动源码；
  + **免责声明** ： XWOS项目中集成的第三方开源软件仅供参考，用户需自行检查这些软件的缺陷以及 **LICENSE** ，
    并 **自行承担相关的缺陷风险及法律风险** 。XWOS开发团队 **不承担任何法律责任** 。
