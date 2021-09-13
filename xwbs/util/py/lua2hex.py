#! /usr/bin/python3
# -*- coding:utf-8 -*-
# @file
# @brief 将Lua源文件转换为C数组
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

import sys
import os
import argparse
import re

def parse_arg():
    parser = argparse.ArgumentParser(description="Convert Lua source to C bytes array.")
    parser.add_argument("-o", "--output", action="store",
                        dest="output",default="out.c",
                        help="Set output path")
    parser.add_argument("-p", "--prefix", action="store",
                        dest="prefix",default="",
                        help="Set array name prefix")
    parser.add_argument(action="store",
                        dest="source",
                        help="Set Lua source path")
    return parser

def main():
    parser = parse_arg()
    args = parser.parse_args()

    output_name = os.path.basename(args.output)
    array_name = args.prefix + re.search(r"(.+?)\.", output_name).group(1)
    # print("output =", args.output, "array_name = ", array_name)

    source_name = os.path.basename(args.source)
    source_suffix = re.search(r"(.+)\.(.+)", source_name).group(2)
    # print("source =", args.source, "source_suffix =", source_suffix)
    if source_suffix != "lua":
        print("error: unknown file type:", source_suffix)
        parser.print_help()
        return

    # 打开文件
    source_file = open(args.source, "rb")
    output_file = open(args.output, "w")

    # 转换文件
    array_line = "const char " + array_name + "[] = {\n"
    output_file.write(array_line)

    source_1st_line = next(source_file)
    source_lines = source_file.readlines();
    if not re.match(r"#!", str(source_1st_line, encoding="utf-8")):
        source_lines.insert(0, source_1st_line)
    # print(source_lines)
    for line in source_lines:
        array_line = ["        "] # 空出8个空格
        for i in line:
            array_line.append("0x{0:02X}, ".format(i))
        array_line.append("\n")
        output_file.write(''.join(array_line))
    output_file.write("        0x00\n")
    output_file.write("};\n")

    # 关闭文件
    source_file.close()
    output_file.close()

if __name__ == "__main__":
    main()
