# ##############################################################################
# Copyright (c) 2025 Li Auto Inc. and its affiliates
# Licensed under the Apache License, Version 2.0(the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ##############################################################################
if(CONFIG_SIM_ETH_DRIVER)
    nuttx_add_library(sim_eth_adp STATIC)
    set(INC
        ${NUTTX_APPS_DIR}/components/include
        ${NUTTX_APPS_DIR}/components/include/autosar
        ${NUTTX_APPS_DIR}/${APPS_DIR}/apps
        ${NUTTX_APPS_DIR}/vendor/semidrive/drivers/platform/Base/include
        ${NUTTX_APPS_DIR}/vendor/semidrive/drivers/configs/boards/E3650_dev_kit/output/include
        ${NUTTX_APPS_DIR}/drivers/sim/eth/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
    )
    file(GLOB SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    )
    list(APPEND SRCS
        ${NUTTX_APPS_DIR}/vendor/semidrive/drivers/configs/boards/E3650_dev_kit/output/src/Eth_PBCfg.c
    )
    target_include_directories(sim_eth_adp PRIVATE ${INC})
    target_sources(sim_eth_adp PRIVATE ${SRCS})
endif()
