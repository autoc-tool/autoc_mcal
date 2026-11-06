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

if(CONFIG_MODULE_MEMIF)
	nuttx_add_library(sim_ea_adp STATIC)
	set(SIM_EEPADP_DIR "${NUTTX_CHIP_ABS_DIR}/driver/eep")
	set(MCAL_EA_DIR "${NUTTX_APPS_DIR}/vendor/semidrive/drivers/platform/Ea")
	set(MCAL_EEP_DIR "${NUTTX_APPS_DIR}/vendor/semidrive/drivers/platform/Eep")
	set(MCAL_TYPEDEF_DIR "${NUTTX_APPS_DIR}/vendor/semidrive/drivers/platform/Base/include")
	set(MCAL_CFG_DIR "${NUTTX_APPS_DIR}/vendor/semidrive/drivers/configs/boards/E3650_dev_kit/output")

	set(INC
		# 类型定义头文件
		${MCAL_TYPEDEF_DIR}
		# EEP cfg头文件
		${MCAL_CFG_DIR}/include
		# veep driver头文件
		${NUTTX_APPS_DIR}/drivers/sim/eep/inc
		# veep posix头文件
		${NUTTX_APPS_DIR}/drivers/sim/eep/eep_posix
		# 工程适配头文件
		${CMAKE_CURRENT_SOURCE_DIR}/inc
		${SIM_EEPADP_DIR}/inc
		# EEP driver头文件
		${MCAL_EEP_DIR}/include
		# EA driver头文件
		${MCAL_EA_DIR}/include
	)
	file(GLOB SRCS
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
		"${MCAL_CFG_DIR}/src/Ea_Cfg.c"
		"${MCAL_EA_DIR}/src/Ea.c"
		"${MCAL_EA_DIR}/src/Ea_Ops.c"
	)

	target_include_directories(sim_ea_adp PRIVATE ${INC})
	target_sources(sim_ea_adp PRIVATE ${SRCS})
endif()