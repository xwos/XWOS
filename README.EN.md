
# XWOS

[切换到简体中文](README.md)

XWOS® (XuanWu Operating System), developed since 2015, is a general-purpose
real-time operating system running on embedded microcontrollers. It targets
reliable, real-time, and safety-critical application domains, including
automotive, IoT, and industrial control.

XWOS source code complies with the **MISRA-C:2012** standard and has been
adopted in an increasing number of production automotive projects.

+ <https://xwos.tech/en>
+ <http://xwos.tech/en>
+ <https://xwos.org/en>
+ <http://xwos.org/en>

# Example Projects

XWOS provides a number of example projects:

+ [Raspberry Pi 4B](https://xwos.tech/en/Docs/UserManual/Board/RPi4B/)
+ STM32
  + [AtkM100zStm32H750XWOS](https://gitee.com/xwos/AtkM100zStm32H750XWOS.git)：ALIENTEK M100Z-M7 board
  + [WeActMiniStm32H750XWOS](https://gitee.com/xwos/WeActMiniStm32H750XWOS.git): WeAct MiniStm32H750 dev board
  + [EmbedFireStm32H743XWOS](https://gitee.com/xwos/EmbedFireStm32H743XWOS.git): EmbedFire STM32H743-Pro dev board
  + [AtkApolloH743XWOS](https://gitee.com/xwos/AtkApolloH743XWOS.git): ALIENTEK Apollo STM32H743 dev board
  + [STM32F072C8XWOS](https://gitee.com/xwos/STM32F072C8XWOS.git): STM32F072C8 minimal system
  + [STM32F103C8XWOS](https://gitee.com/xwos/STM32F103C8XWOS.git): STM32F103C8 minimal system
  + [ATKSTM32F103ZXWOS](https://gitee.com/xwos/ATKSTM32F103ZXWOS.git): ALIENTEK F103 core board
  + [ATKSTM32F407ZXWOS](https://gitee.com/xwos/ATKSTM32F407ZXWOS.git): ALIENTEK F407 core board
  + [FK429M1XWOS](https://gitee.com/xwos/FK429M1XWOS.git): Fanke STM32F429-M1 dev board
+ Flagchip
  + [Fc7300DemoBoard](https://xwos.tech/en/Docs/UserManual/Board/Flagchip/Fc7300DemoBoard/)：FC7300 Demo Board
+ GD32
  + [GD32F103CBXWOS](https://gitee.com/xwos/GD32F103CBXWOS.git): GD32F103CB core board
  + [GD32F303CCXWOS](https://gitee.com/xwos/GD32F303CCXWOS.git): GD32F303CC core board
+ S32K
  + [S32K1XWOS](https://gitee.com/xwos/S32K1XWOS.git): S32K144 official evaluation board
  + [S32K3XWOS](https://gitee.com/xwos/S32K3XWOS.git): S32K312 official evaluation board
+ GD32V
  + [GD32VF103XWOS](https://gitee.com/xwos/GD32VF103XWOS.git): Longan nano dev board
+ APM32
  + [APM32F103CBXWOS](https://gitee.com/xwos/APM32F103CBXWOS.git): APM32F103CB core board
+ CH32
  + [CH32F103C8XWOS](https://gitee.com/xwos/CH32F103C8XWOS.git): CH32F103CB core board
+ Embed PowerPC
  + [MPC5607BXWOS](https://gitee.com/xwos/MPC5607BXWOS.git): MPC5607B core board


# License

+ **XWOS Core Code**
  + XWOS core code is released under [**MPL-2.0**](http://mozilla.org/MPL/2.0/);
  + Scope: source code under the `xwos` and `xwmd` directories.
+ **XWOS Peripheral Code**
  + XWOS peripheral code is released under [**Apache-2.0**](http://www.apache.org/licenses/LICENSE-2.0);
  + Scope: source code under the `xwcd`, `xwbd`, `xwam`, `xwbs`, and `xwxr` directories.
+ **Third-Party Software**
  + Scope: source code under the `xwem` directory and third-party driver code under `xwbd`.
  + **Disclaimer**: Third-party open-source software integrated in the XWOS
    project is provided for reference only. Users must inspect these components
    for defects and **LICENSE** terms themselves, and **assume all associated
    defect and legal risks**. The XWOS development team **assumes no legal
    liability**.
