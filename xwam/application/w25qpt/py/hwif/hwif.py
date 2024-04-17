#! /usr/bin/python3
# -*- coding:utf-8 -*-
# @file
# @brief W25Qxx编程器：硬件接口层
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

import serial
import zlib

class HwIf:
    SOF = 0xAA
    SOF_SIZE = 4
    EOF = 0x55
    EOF_SIZE = 2
    HEAD_SIZE = 4
    ADDRESS_SIZE = 4
    SDU_SIZE_MAX = 256
    CHKSUM_SIZE = 4

    def __init__(self, port: str = "/dev/ttyUSB0", baudrate: int = 1000000):
        self.port = port
        self.baudrate = baudrate
        self.uart = serial.Serial(port=self.port, baudrate=self.baudrate,
                                  parity=serial.PARITY_NONE,
                                  stopbits=serial.STOPBITS_ONE)
    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if exc_type:
            print(f"Exception: {exc_type} {exc_value}")
        self.close()

    def is_opened(self):
        return self.uart.is_open

    def open(self):
        self.uart.open()

    def close(self):
        self.uart.close()

    def tx(self, data: bytearray):
        if (not self.uart.is_open):
            return 0
        else:
            return self.uart.write(data)

    def __chk_sdusize_parity(self, head: bytearray):
        parity = (((head[3] >> 0) & 3) ^
                  ((head[3] >> 2) & 3) ^
                  ((head[3] >> 4) & 3) ^
                  ((head[3] >> 6) & 3) ^
                  ((head[2] >> 0) & 3) ^
                  ((head[2] >> 2) & 3) ^
                  ((head[2] >> 4) & 3) ^
                  ((head[2] >> 6) & 3))
        if parity == 0:
            return True
        else:
            return False

    def __decode_sdusize(self, head: bytearray):
        sdusize = (head[2] & 0x3F) << 8
        sdusize |= head[3]
        return sdusize

    def __chk_crc32(self, msg: bytearray, sdusize: int):
        msgcrc32 = zlib.crc32(msg[self.SOF_SIZE: self.SOF_SIZE + self.HEAD_SIZE + self.ADDRESS_SIZE + sdusize])
        if ((((msgcrc32 >> 24) & 0xFF) == msg[self.SOF_SIZE + self.HEAD_SIZE + self.ADDRESS_SIZE + sdusize + 0]) and
            (((msgcrc32 >> 16) & 0xFF) == msg[self.SOF_SIZE + self.HEAD_SIZE + self.ADDRESS_SIZE + sdusize + 1]) and
            (((msgcrc32 >> 8) & 0xFF) == msg[self.SOF_SIZE + self.HEAD_SIZE + self.ADDRESS_SIZE + sdusize + 2]) and
            (((msgcrc32 >> 0) & 0xFF) == msg[self.SOF_SIZE + self.HEAD_SIZE + self.ADDRESS_SIZE + sdusize + 3])):
            return True
        else:
            return False

    def __rx_once(self):
        sof = bytearray([self.SOF, self.SOF, self.SOF, self.SOF])
        head = bytearray([0x0, 0x0, 0x0, 0x0])
        body = bytearray()
        eof = bytearray([self.EOF, self.EOF])

        # Rx SOF
        sofcnt = 0
        while True:
            data = self.uart.read()
            if data[0] == self.SOF:
                sofcnt += 1
            else:
                break
        if sofcnt < self.SOF_SIZE:
            return None

        # Rx HEAD
        head[0] = data[0]
        headcnt = 1
        while headcnt < self.HEAD_SIZE:
            data = self.uart.read()
            head[headcnt] = data[0]
            headcnt += 1
        if not self.__chk_sdusize_parity(head):
            return None
        sdusize = self.__decode_sdusize(head)
        if sdusize > self.SDU_SIZE_MAX:
            return None

        # Rx Body
        bodysize = sdusize + self.ADDRESS_SIZE + self.CHKSUM_SIZE
        bodycnt = 0
        while bodycnt < bodysize:
            data = self.uart.read()
            body += data
            bodycnt += 1

        # Rx EOF
        eofcnt = 0
        while eofcnt < self.EOF_SIZE:
            data = self.uart.read()
            if data[0] == self.EOF:
                eofcnt += 1
            else:
                return None

        # Check CRC32
        msg = sof + head + body + eof
        if self.__chk_crc32(msg, sdusize):
            return msg
        else:
            return None

    def rx(self):
        if (not self.uart.is_open):
            return None
        msg = None
        while msg == None:
            msg = self.__rx_once()
        return msg
