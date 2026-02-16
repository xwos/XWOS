/**
 * @file
 * @brief XWSH: Commands
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <stdio.h>
#include <string.h>
#include <xwos/osal/thd.h>
#include "xwmd/cli/xwsh/mi.h"
#include "xwmd/cli/xwsh/core.h"

xwer_t xwsh_cmd_help(xwsz_t argc, char ** argv)
{
        XWOS_UNUSED(argc);
        XWOS_UNUSED(argv);
        xwsh_show_all_cmds();
        return XWOK;
}

xwer_t xwsh_cmd_clear(xwsz_t argc, char ** argv)
{
        XWOS_UNUSED(argc);
        XWOS_UNUSED(argv);
        printf("\e[2J");
        printf("\e[0;0H");
        return XWOK;
}

xwer_t xwsh_cmd_rd(xwsz_t argc, char ** argv)
{
        xwptr_t addr;
        xwsz_t size;
        xwu8_t * p;
        xwsz_t i;
        xwsz_t j;
        xwer_t rc;

        size = 1U;
        if (argc < 2U) {
                printf("Usage: rd <addr hex> [size dec, default 1]\r\n");
                printf("Example: rd 0x90000000 16\r\n");
                rc = -EINVAL;
                goto err_invalid;
        }
        rc = sscanf(argv[1], "%lx", &addr);
        if (1 != rc) {
                printf("Invalid address format\n");
                rc = -EINVAL;
                goto err_invalid;
        }
        if (argc > 2U) {
                rc = sscanf(argv[2], "%d", &size);
                if (1 != rc) {
                        printf("Invalid size format\n");
                        rc = -EINVAL;
                        goto err_invalid;
                }
        }

        printf("origin address: %p\r\n", (void *)addr);
        p = (xwu8_t *)addr;
        for (i = 0; i < size; i += 16U) {
                printf("%08x: ", i);
                for (j = 0; j < 16U && (i + j) < size; j++) {
                        printf("%02x ", p[i + j]);
                        if (j == 7U) {
                                printf(" ");
                        }
                }
                while (j < 16U) {
                        printf("   ");
                        if (j == 7U) {
                                printf(" ");
                        }
                        j++;
                }
                printf(" |");
                for (j = 0; j < 16U && (i + j) < size; j++) {
                        if (p[i + j] >= 32U && p[i + j] <= 126U) {
                                printf("%c", p[i + j]);
                        } else {
                                printf(".");
                        }
                }
                printf("|\r\n");
        }
        return XWOK;

err_invalid:
        return rc;
}

xwer_t xwsh_cmd_wr(xwsz_t argc, char ** argv)
{
        xwptr_t addr;
        xwu32_t value;
        xwu8_t * p;
        xwsz_t size;
        xwer_t rc;

        size = 4U;
        if (argc < 3U) {
                printf("Usage: wr <addr hex> <value hex> [b|w|l]\n");
                printf("  b: byte (1 byte)\n");
                printf("  w: word (2 bytes)\n");
                printf("  l: long (4 bytes, default)\n");
                rc = -EINVAL;
                goto err_invalid;
        }
        if (sscanf(argv[1], "%lx", &addr) != 1) {
                printf("Invalid address format\n");
                rc = -EINVAL;
                goto err_invalid;
        }
        if (sscanf(argv[2], "%lx", &value) != 1) {
                printf("Invalid value format\n");
                rc = -EINVAL;
                goto err_invalid;
        }
        if (argc > 3U) {
                switch (argv[3][0]) {
                case 'b':
                        size = 1U;
                        break;
                case 'w':
                        size = 2U;
                        break;
                case 'l':
                        size = 4U;
                        break;
                default:
                        printf("Invalid size format, use b/w/l\n");
                        rc = -EINVAL;
                        goto err_invalid;
                }
        }
        p = (xwu8_t *)addr;
        switch (size) {
                case 1U:
                        *p = (xwu8_t)value;
                        printf("Wrote 0x%02lx to 0x%08lx\n", value, addr);
                        break;
                case 2U:
                        *(xwu16_t *)p = (xwu16_t)value;
                        printf("Wrote 0x%04lx to 0x%08lx\n", value, addr);
                        break;
                case 4U:
                        *(xwu32_t *)p = value;
                        printf("Wrote 0x%08lx to 0x%08lx\n", value, addr);
                        break;
        }
        return XWOK;

err_invalid:
        return rc;
}

const
struct xwsh_cmd xwsh_cmd_table[] = {
        { .name = "help", .func = xwsh_cmd_help, .desc = "show this help" },
        { .name = "clear", .func = xwsh_cmd_clear, .desc = "clear screen" },
        { .name = "rd", .func = xwsh_cmd_rd, .desc = "read memory" },
        { .name = "wr", .func = xwsh_cmd_wr, .desc = "write memory" },
};

const
xwsz_t xwsh_cmd_table_size = sizeof(xwsh_cmd_table) / sizeof(xwsh_cmd_table[0]);
