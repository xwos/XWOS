#! /bin/sh
# @file
# @brief 编译xwmo的shell辅助函数
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

function xwmc()
{
  local cpath=$(pwd)
  local rpath=
  local prefix=
  if [[ ${cpath} =~ ${XuanWuOS_XWMD_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_XWMD_DIR}}
    prefix=XWMDCFG
  elif [[ ${cpath} =~ ${XuanWuOS_XWCD_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_XWCD_DIR}}
    prefix=XWCDCFG
  elif [[ ${cpath} =~ ${XuanWuOS_XWEM_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_XWEM_DIR}}
    prefix=XWEMCFG
  elif [[ ${cpath} =~ ${XuanWuOS_XWAM_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_XWAM_DIR}}
    prefix=XWAMCFG
  elif [[ ${cpath} =~ ${XuanWuOS_BM_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_BM_DIR}}
    prefix=BMCFG
  elif [[ ${cpath} =~ ${XuanWuOS_OEM_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_OEM_DIR}}
    prefix=OEMCFG
  fi
  local cfg=${rpath//_/__}
  local cfg=${cfg//./_}
  local cfg=${cfg//-/_}
  local cfg=${prefix}${cfg//\//_}
  echo ${cfg}
}

function xwmn()
{
  local cpath=$(pwd)
  local rpath=
  local prefix=
  if [[ ${cpath} =~ ${XuanWuOS_XWMD_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_XWMD_DIR}}
    prefix=xwmd
  elif [[ ${cpath} =~ ${XuanWuOS_XWCD_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_XWCD_DIR}}
    prefix=xwcd
  elif [[ ${cpath} =~ ${XuanWuOS_XWEM_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_XWEM_DIR}}
    prefix=xwem
  elif [[ ${cpath} =~ ${XuanWuOS_XWAM_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_XWAM_DIR}}
    prefix=xwam
  elif [[ ${cpath} =~ ${XuanWuOS_BM_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_BM_DIR}}
    prefix=bm
  elif [[ ${cpath} =~ ${XuanWuOS_OEM_DIR} ]] ; then
    rpath=${cpath#*${XuanWuOS_OEM_DIR}}
    prefix=oem
  fi
  local name=${rpath//_/__}
  local name=${rpath//./_}
  local name=${rpath//-/_}
  local name=${prefix}${name//\//_}.a
  echo ${name}
}

function xwm()
{
  local argv=$(getopt -o B -n "${0}" -- "$@")
  eval set -- "${argv}"
  local opt_b=false
  local opt_target=
  while true ; do
    case "$1" in
      -B)
        opt_b=true
        shift 1
        ;;
      --)
        opt_target=$2
        break
        ;;
    esac
  done

  if [[ ${opt_b} = true ]] ; then
    make -C ${XuanWuOS_BRD_DIR} clean
  fi
  make -C ${XuanWuOS_BRD_DIR} ${opt_target}
}

function xwmm()
{
  local argv=$(getopt -o B -n "${0}" -- "$@")
  eval set -- "${argv}"
  local opt_b=false
  local opt_target=
  while true ; do
    case "$1" in
      -B)
        opt_b=true
        shift 1
        ;;
      --)
        opt_target=$2
        break
        ;;
    esac
  done

  local cpath=$(pwd)
  local rpath=${cpath#*${XuanWuOS_PATH}/}
  if [[ ${opt_b} = true ]] ; then
    make -C ${XuanWuOS_PATH} -f ${rpath}/xwmo.mk XuanWuOS_BRD_DIR=${XuanWuOS_BRD_DIR} clean
  fi
  make -C ${XuanWuOS_PATH} -f ${rpath}/xwmo.mk XuanWuOS_BRD_DIR=${XuanWuOS_BRD_DIR} ${opt_target}
}

function xwmmm()
{
  local argv=$(getopt -o B -n "${0}" -- "$@")
  eval set -- "${argv}"
  local opt_b=false
  local opt_mo=
  while true ; do
    case "$1" in
      -B)
        opt_b=true
        shift 1
        ;;
      --)
        opt_mo=${2}
        break
        ;;
    esac
  done

  local rpath=${opt_mo}
  if [[ ${opt_b} = true ]] ; then
    make -C ${XuanWuOS_PATH} -f ${rpath}/xwmo.mk XuanWuOS_BRD_DIR=${XuanWuOS_BRD_DIR} clean
  fi
  make -C ${XuanWuOS_PATH} -f ${rpath}/xwmo.mk XuanWuOS_BRD_DIR=${XuanWuOS_BRD_DIR}
}

function xwcroot()
{
  cd ${XuanWuOS_PATH}
}

function xwcbd()
{
  cd ${XuanWuOS_PATH}/${XuanWuOS_BRD_DIR}
}

echo -e "\e[1m\e[47;34m初始化完成 Y(^_^)Y \e[0m"
echo -e "\e[1m\e[44;32m[常用命令索引]\e[0m"
echo -e "\e[1m\e[49;34mxwcbd            切换到电路板工程目录\e[0m"
echo -e "\e[1m\e[49;34mxwcroot          切换到根目录\e[0m"
echo -e "\e[1m\e[49;34mxwm              编译整个工程\e[0m"
echo -e "\e[1m\e[49;34mxwmm             编译当前路径下的玄武模块\e[0m"
echo -e "\e[1m\e[49;34mxwmmm PATH       编译指定路径下的玄武模块\e[0m"
