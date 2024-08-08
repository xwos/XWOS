#! /bin/bash
# @file
# @brief environment-setup shell script
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

if [[ ${BASH_SOURCE[0]} = ${0} ]] ;then
  echo -e "\033[33;1mThis script must be sourced by bash!\033[0m"
  exit 0
fi

cd $(dirname ${BASH_SOURCE[0]})
make cfg
source wkspc/env.rc
xwcroot
