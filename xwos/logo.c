/**
 * @file
 * @brief XWOS LOGO
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/version.h>
#include <xwos/lib/xwlog.h>

#define XWOS_LOGO_SEGSIZE  32U

const __xwos_rodata
char xwos_logo[] =
        "                      -=-:.      .:-=-                      \r\n"
        "                     :++++++=--=++++++:                     \r\n"
        "                    .+++-:-=++++=-:-+++.                    \r\n"
        "                   :+++-     ..     -+++:.                  \r\n"
        "             ..:=+++++=              =+++++=-..             \r\n"
        "             +++++=:.                  .:=+++++             \r\n"
        "             .+++-                        -+++.             \r\n"
        "              :+++:                      :+++:              \r\n"
        "               -+++.                    .+++-               \r\n"
        "              .+++-                      -+++.              \r\n"
        "              =++=                        =++=              \r\n"
        "             =+++=:.                    .:=+++=             \r\n"
        "             :-=+++++=:              :=+++++=-:             \r\n"
        "                 .:-+++:            :+++-:.                 \r\n"
        "                    :+++:.:-=++=-:.:+++:                    \r\n"
        "                     -+++++++==+++++++-                     \r\n"
        "                      =+=-:.    .:-=+=                      \r\n"
        ".---.   .--- .---    --=.   :--.    :--==-:.       .--==-:. \r\n"
        " -+++. .+++: .+++.  :+++-   +++.  :++++==+++=.   .=+++==++= \r\n"
        "  :+++.=++.   =++-  =++++  :++=  :+++:    -+++.  =+++.   .  \r\n"
        "   :+++++.    :++= .++-++- -++-  =++=      +++-  .++++=-:.  \r\n"
        "   :++++=     .+++ -+= =+= =++.  +++=      =++=    .-=++++=:\r\n"
        "  :++=:+++.    =++-=+: :++-++=   -+++.    :+++:        .=+++\r\n"
        " -++=  :+++:   :+++++   +++++:    -+++=--=+++:   -++=-:-+++-\r\n"
        ":+==.   -===.   ====-   -====.     .-=++++=-     .-=++++=-. \r\n";

const __xwos_rodata
xwsz_t xwos_logo_size = sizeof(xwos_logo);

__xwos_code
void xwos_show_logo(void)
{
        xwsz_t pos;
        xwsz_t seg;
        xwsz_t logosz;

        logosz = xwos_logo_size - 1U;
        pos = 0;
        seg = 0;
        while (pos < logosz) {
                seg = ((logosz - pos) > XWOS_LOGO_SEGSIZE ?
                       XWOS_LOGO_SEGSIZE : (logosz - pos));
                xwps(&xwos_logo[pos], seg);
                pos += seg;
        }
        xwpf("Welcome to the XWOS-V%s! Build time %s %s\r\n",
             XWOS_VERSION, __DATE__, __TIME__);
}
