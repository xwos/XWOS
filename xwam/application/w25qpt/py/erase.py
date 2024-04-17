#! /usr/bin/python3
# -*- coding:utf-8 -*-
# @file
# @brief W25Qxx编程器：擦除工具
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > Licensed under the Apache License, Version 2.0 (the "License");
# > you may not use this file except in compliance with the License.
# > You may obtain a copy of the License at
# >
# >         http://www.apache.org/licenses/LICENSE-2.0
# >
# > Unless required by applicable law or agreed to in writing, software
# > distributed under the License is distributed on an "AS IS" BASIS,
# > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# > See the License for the specific language governing permissions and
# > limitations under the License.
#

import argparse
import zlib
from hwif import hwif

msg_erase_chip = bytearray([0xAA, 0xAA, 0xAA, 0xAA,
                            0x03, 0x00,
                            0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00,
                            0xEB, 0xAD, 0xD8, 0x8A,
                            0x55, 0x55])

msg_erase_sector = bytearray([0xAA, 0xAA, 0xAA, 0xAA,
                              0x04, 0x00,
                              0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, # 地址 TBD
                              0x00, 0x00, 0x00, 0x00, # CRC32 TBD
                              0x55, 0x55])

msg_erase_32kblock = bytearray([0xAA, 0xAA, 0xAA, 0xAA,
                                0x05, 0x00,
                                0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, # 地址 TBD
                                0x00, 0x00, 0x00, 0x00, # CRC32 TBD
                                0x55, 0x55])

msg_erase_64kblock = bytearray([0xAA, 0xAA, 0xAA, 0xAA,
                                0x06, 0x00,
                                0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, # 地址 TBD
                                0x00, 0x00, 0x00, 0x00, # CRC32 TBD
                                0x55, 0x55])


def erase_chip(port: str, baudrate: int):
    with hwif.HwIf(port = port, baudrate = baudrate) as hwport:
        txsz = hwport.tx(msg_erase_chip)
        if txsz == len(msg_erase_chip):
            rxb = hwport.rx()
            if not rxb == None:
                oprc = rxb[5]
                if (oprc == 0):
                    print("擦除Flash<chip>... OK")
                elif (oprc == 1):
                    print("擦除Flash<chip>... -EINTR")
                elif (oprc == 2):
                    print("擦除Flash<chip>... -ETIMEDOUT")
                else:
                    print("擦除Flash<chip>... -EIO")


def erase_4k(port: str, baudrate: int, address: int):
    with hwif.HwIf(port = port, baudrate = baudrate) as hwport:
        msg_erase_sector[8 : 12] = address.to_bytes(4, byteorder='big', signed=False)
        msgcrc32 = zlib.crc32(msg_erase_sector[4 : 12])
        msg_erase_sector[12 : 16] = msgcrc32.to_bytes(4, byteorder='big',signed=False)
        txsz = hwport.tx(msg_erase_sector)
        if txsz == len(msg_erase_sector):
            rxb = hwport.rx()
            if not rxb == None:
                oprc = rxb[5]
                if (oprc == 0):
                    print("擦除Flash<4k>({0})... OK".format(hex(address)))
                elif (oprc == 1):
                    print("擦除Flash<4k>({0})... -EINTR".format(hex(address)))
                elif (oprc == 2):
                    print("擦除Flash<4k>({0})... -ETIMEDOUT".format(hex(address)))
                else:
                    print("擦除Flash<4k>({0})... -EIO".format(hex(address)))


def erase_32k(port: str, baudrate: int, address: int):
    with hwif.HwIf(port = port, baudrate = baudrate) as hwport:
        msg_erase_32kblock[8 : 12] = address.to_bytes(4, byteorder='big', signed=False)
        msgcrc32 = zlib.crc32(msg_erase_32kblock[4 : 12])
        msg_erase_32kblock[12 : 16] = msgcrc32.to_bytes(4, byteorder='big',signed=False)
        # print(msg_erase_32kblock.hex())
        txsz = hwport.tx(msg_erase_32kblock)
        if txsz == len(msg_erase_32kblock):
            rxb = hwport.rx()
            if not rxb == None:
                oprc = rxb[5]
                if (oprc == 0):
                    print("擦除Flash<32k>({0})... OK".format(hex(address)))
                elif (oprc == 1):
                    print("擦除Flash<32k>({0})... -EINTR".format(hex(address)))
                elif (oprc == 2):
                    print("擦除Flash<32k>({0})... -ETIMEDOUT".format(hex(address)))
                else:
                    print("擦除Flash<32k>({0})... -EIO".format(hex(address)))


def erase_64k(port: str, baudrate: int, address: int):
    with hwif.HwIf(port = port, baudrate = baudrate) as hwport:
        msg_erase_64kblock[8 : 12] = address.to_bytes(4, byteorder='big', signed=False)
        msgcrc32 = zlib.crc32(msg_erase_64kblock[4 : 12])
        msg_erase_64kblock[12 : 16] = msgcrc32.to_bytes(4, byteorder='big',signed=False)
        # print(msg_erase_64kblock.hex())
        txsz = hwport.tx(msg_erase_64kblock)
        if txsz == len(msg_erase_64kblock):
            rxb = hwport.rx()
            if not rxb == None:
                oprc = rxb[5]
                if (oprc == 0):
                    print("擦除Flash<64k>({0})... OK".format(hex(address)))
                elif (oprc == 1):
                    print("擦除Flash<64k>({0})... -EINTR".format(hex(address)))
                elif (oprc == 2):
                    print("擦除Flash<64k>({0})... -ETIMEDOUT".format(hex(address)))
                else:
                    print("擦除Flash<64k>({0})... -EIO".format(hex(address)))


def parse_arg():
    parser = argparse.ArgumentParser(description="W25Qxx编程器：擦除工具")
    parser.add_argument("-p", "--port",
                        action="store", dest="port", default="/dev/ttyUSB0",
                        help="串口端口")
    parser.add_argument("-b", "--baudrate",
                        action="store", dest="baudrate", type=int, default=1000000,
                        help="波特率")
    parser.add_argument("-t", "--type",
                        action="store", dest="type", default="chip",
                        help="擦除类型：4k, 32k, 64k, chip")
    parser.add_argument('address',
                        help="起始地址")
    return parser


def main():
    parser = parse_arg()
    args = parser.parse_args()
    address = int(args.address, 16)
    if (args.type == "chip"):
        erase_chip(args.port, args.baudrate)
    elif (args.type == "4k"):
        erase_4k(args.port, args.baudrate, address)
    elif (args.type == "32k"):
        erase_32k(args.port, args.baudrate, address)
    elif (args.type == "64k"):
        erase_64k(args.port, args.baudrate, address)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
