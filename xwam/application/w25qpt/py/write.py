#! /usr/bin/python3
# -*- coding:utf-8 -*-
# @file
# @brief W25Qxx编程器：写入工具
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
import hwif


def parse_arg():
    parser = argparse.ArgumentParser(description="W25Qxx编程器：写入工具")
    parser.add_argument("-p", "--port",
                        action="store", dest="port", default="/dev/ttyUSB0",
                        help="串口端口")
    parser.add_argument("-b", "--baudrate",
                        action="store", dest="baudrate", type=int, default=1000000,
                        help="波特率")
    parser.add_argument('data',
                        help="数据文件")
    parser.add_argument('address',
                        help="起始地址")
    return parser


def __fill_parity(size:int):
    sizeay = bytearray(size.to_bytes(2, byteorder='big', signed=False))
    parity = ((((sizeay[1] >> 0) & 3) ^
               ((sizeay[1] >> 2) & 3) ^
               ((sizeay[1] >> 4) & 3) ^
               ((sizeay[1] >> 6) & 3) ^
               ((sizeay[0] >> 0) & 3) ^
               ((sizeay[0] >> 2) & 3) ^
               ((sizeay[0] >> 4) & 3)) << 6) & 0xC0
    sizeay[0] &= 0x3F
    sizeay[0] += parity
    return sizeay


def main():
    # Argument
    parser = parse_arg()
    args = parser.parse_args()
    address = int(args.address, 16)
    datafile = open(args.data, "rb")
    if (datafile.closed):
        print("打开文件\"" + args.data + "\"失败！")
        return

    hwport = hwif.HwIf(port=args.port, baudrate=args.baudrate)
    if (not hwport.is_opened()):
        print("打开串口\"" + args.port + "\"失败！")
        return

    # Write
    flashdata = datafile.read(256)
    wrsz = len(flashdata)
    while wrsz > 0:
        msg_write = bytearray([0xAA, 0xAA, 0xAA, 0xAA, 0x07, 0x00])
        msg_write[6 : 8] = __fill_parity(wrsz)
        msg_write[8 : 12] = address.to_bytes(4, byteorder='big', signed=False)
        msg_write[12 : 12 + wrsz] = flashdata
        msgcrc32 = zlib.crc32(msg_write[4 : 12 + wrsz])
        msg_write[12 + wrsz : 12 + wrsz + 4] = msgcrc32.to_bytes(4, byteorder='big',signed=False)
        msg_write.append(0x55)
        msg_write.append(0x55)
        print(msg_write.hex())

        txsz = hwport.tx(msg_write)
        if (txsz == len(msg_write)):
            rxb = hwport.rx()
            if not rxb == None:
                oprc = rxb[5]
                if (oprc == 0):
                    print("{0} {1} {2}".format("写入Flash(" + hex(address) + ")", wrsz, "字节 ... OK"))
                    address += wrsz
                    flashdata = datafile.read(256)
                    wrsz = len(flashdata)
                    print("load {} data".format(wrsz))
                elif (oprc == 1):
                    print("{0} {1} {2}".format("写入Flash(" + hex(address) + ")", wrsz, "字节 ... -EINTR"))
                    break
                elif (oprc == 2):
                    print("{0} {1} {2}".format("写入Flash(" + hex(address) + ")", wrsz, "字节 ... -ETIMEDOUT"))
                    break
                else:
                    print("{0} {1} {2}".format("写入Flash(" + hex(address) + ")", wrsz, "字节 ... -EIO"))
                    break

if __name__ == "__main__":
    main()
