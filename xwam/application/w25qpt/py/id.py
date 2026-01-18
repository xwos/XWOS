#! /usr/bin/python3
# -*- coding:utf-8 -*-
# @file
# @brief W25Qxx编程器：ID读取工具
# @author
# + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
from hwif import hwif

msg_read_uid = bytearray([0xAA, 0xAA, 0xAA, 0xAA,
                          0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00,
                          0x65, 0x22, 0xDF, 0x69,
                          0x55, 0x55])

msg_read_mid = bytearray([0xAA, 0xAA, 0xAA, 0xAA,
                          0x01, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00,
                          0xA9, 0x88, 0xDF, 0xF7,
                          0x55, 0x55])

msg_read_jid = bytearray([0xAA, 0xAA, 0xAA, 0xAA,
                          0x02, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00,
                          0x27, 0x07, 0xD8, 0x14,
                          0x55, 0x55])


def read_uid(port: str, baudrate: int):
    with hwif.HwIf(port = port, baudrate = baudrate) as hwport:
        txsz = hwport.tx(msg_read_uid)
        if txsz == len(msg_read_uid):
            rxb = hwport.rx()
            if not rxb == None:
                oprc = rxb[5]
                if (oprc == 0):
                    print(rxb[12:20].hex().upper())
                elif (oprc == 1):
                    print("读取UID... -EINTR")
                elif (oprc == 2):
                    print("读取UID... -ETIMEDOUT")
                else:
                    print("读取UID... -EIO")


def read_mid(port: str, baudrate: int):
    with hwif.HwIf(port = port, baudrate = baudrate) as hwport:
        txsz = hwport.tx(msg_read_mid)
        if txsz == len(msg_read_mid):
            rxb = hwport.rx()
            if not rxb == None:
                oprc = rxb[5]
                if (oprc == 0):
                    print(rxb[12:14].hex().upper())
                elif (oprc == 1):
                    print("读取MID... -EINTR")
                elif (oprc == 2):
                    print("读取MID... -ETIMEDOUT")
                else:
                    print("读取MID... -EIO")


def read_jid(port: str, baudrate: int):
    with hwif.HwIf(port = port, baudrate = baudrate) as hwport:
        txsz = hwport.tx(msg_read_jid)
        if txsz == len(msg_read_jid):
            rxb = hwport.rx()
            if not rxb == None:
                oprc = rxb[5]
                if (oprc == 0):
                    print(rxb[13:16].hex().upper())
                elif (oprc == 1):
                    print("读取JID... -EINTR")
                elif (oprc == 2):
                    print("读取JID... -ETIMEDOUT")
                else:
                    print("读取JID... -EIO")


def parse_arg():
    parser = argparse.ArgumentParser(description="W25Qxx编程器：ID读取工具")
    parser.add_argument("-p", "--port",
                        action="store", dest="port", default="/dev/ttyUSB0",
                        help="串口端口")
    parser.add_argument("-b", "--baudrate",
                        action="store", dest="baudrate", type=int, default=1000000,
                        help="波特率")
    parser.add_argument('id',
                        metavar='ID',
                        help="ID类型：uid,mid,jid")
    return parser


def main():
    parser = parse_arg()
    args = parser.parse_args()
    if args.id == "uid":
        read_uid(args.port, args.baudrate)
    elif args.id == "mid":
        read_mid(args.port, args.baudrate)
    elif args.id == "jid":
        read_jid(args.port, args.baudrate)

if __name__ == "__main__":
    main()
