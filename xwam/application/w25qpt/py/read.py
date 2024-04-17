#! /usr/bin/python3
# -*- coding:utf-8 -*-
# @file
# @brief W25Qxx编程器：读取工具
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

def read(port: str, baudrate: int, address: int, size: int) -> bytes:
    msg = bytearray([0xAA, 0xAA, 0xAA, 0xAA,
                     0x08, 0x00, 0x40, 0x04,
                     0x00, 0x00, 0x00, 0x00, # 地址
                     0x00, 0x00, 0x00, 0x00, # 读取的大小，最大值256
                     0x00, 0x00, 0x00, 0x00, # CRC32
                     0x55, 0x55])
    flashdata = bytearray()
    with hwif.HwIf(port = port, baudrate = baudrate) as hwport:
        rest = size
        rdsz = 0
        while rest > 0:
            if (rest > 256):
                rdsz = 256
            else:
                rdsz = rest
            msg[8 : 12] = address.to_bytes(4, byteorder='big', signed=False)
            msg[12 : 16] = rdsz.to_bytes(4, byteorder='big', signed=False)
            msgcrc32 = zlib.crc32(msg[4 : 16])
            msg[16 : 20] = msgcrc32.to_bytes(4, byteorder='big', signed=False)
            print(msg.hex())
            txsz = hwport.tx(msg)
            if (txsz == len(msg)):
                rxb = hwport.rx()
                if not rxb == None:
                    oprc = rxb[5]
                    if (oprc == 0):
                        flashdata += rxb[12 : rdsz + 12]
                        rest -= rdsz
                        address += rdsz
                    elif (oprc == 1):
                        print("读取Flash({0}, {1})... -EINTR".format(hex(address), rdsz))
                        break
                    elif (oprc == 2):
                        print("读取Flash({0}, {1})... -ETIMEDOUT".format(hex(address), rdsz))
                        break
                    else:
                        print("读取Flash({0}, {1})... -EIO".format(hex(address), rdsz))
                        break
    return flashdata


def parse_arg():
    parser = argparse.ArgumentParser(description="W25Qxx编程器：读取工具")
    parser.add_argument("-p", "--port",
                        action="store", dest="port", default="/dev/ttyUSB0",
                        help="串口端口")
    parser.add_argument("-b", "--baudrate",
                        action="store", dest="baudrate", type=int, default=1000000,
                        help="波特率")
    parser.add_argument("-o", "--output", action="store",
                        dest="output",default="",
                        help="保存到文件")
    parser.add_argument('address',
                        help="起始地址")
    parser.add_argument('size',
                        help="大小，<=256")
    return parser


def main():
    parser = parse_arg()
    args = parser.parse_args()
    address = int(args.address, 16)
    size = int(args.size, 10)
    if (args.output != ""):
        output = open(args.output, "w+b")
    else:
        output = None

    flashdata = read(args.port, args.baudrate, address, size)
    if (output):
        if (len(flashdata) > 0):
            output.write(flashdata)
    else:
        if (len(flashdata) > 0):
            print(flashdata.hex().upper())

if __name__ == "__main__":
    main()
