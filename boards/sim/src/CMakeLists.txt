# ##############################################################################
#
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
#
# ##############################################################################

# CSRCS = dummy.c
set(SIM_COMMON_BOARD ${NUTTX_DIR}/boards/sim/sim/sim)
nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim.h
${CMAKE_CURRENT_LIST_DIR}/sim.h)

if(CONFIG_RT_FRAMEWORK)
  list(APPEND SRCS sim_rt_fwk.c)
endif()

if(CONFIG_BOARD_LATE_INITIALIZE)
  nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_boot.c
  ${CMAKE_CURRENT_LIST_DIR}/sim_boot.c)
  list(APPEND SRCS sim_boot.c)
endif()

if(CONFIG_BOARDCTL)
  nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_appinit.c
  ${CMAKE_CURRENT_LIST_DIR}/sim_appinit.c)
  list(APPEND SRCS sim_appinit.c)
endif()

if(CONFIG_BOARD_LATE_INITIALIZE)
  nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_bringup.c
  ${CMAKE_CURRENT_LIST_DIR}/sim_bringup.c)
  list(APPEND SRCS sim_bringup.c)
  if(CONFIG_LIBC_ZONEINFO_ROMFS)
    nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_zoneinfo.c
    ${CMAKE_CURRENT_LIST_DIR}/sim_zoneinfo.c)
    list(APPEND SRCS sim_zoneinfo.c)
  endif()
elseif(CONFIG_BOARDCTL)
  nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_bringup.c
  ${CMAKE_CURRENT_LIST_DIR}/sim_bringup.c)
  list(APPEND SRCS sim_bringup.c)
  if(CONFIG_LIBC_ZONEINFO_ROMFS)
    list(APPEND SRCS sim_zoneinfo.c)
  endif()
endif()

if(CONFIG_EXAMPLES_GPIO)
  if(CONFIG_GPIO_LOWER_HALF)
    nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_ioexpander.c
    ${CMAKE_CURRENT_LIST_DIR}/sim_ioexpander.c)
    list(APPEND SRCS sim_ioexpander.c)
  else()
    nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_gpio.c
    ${CMAKE_CURRENT_LIST_DIR}/sim_gpio.c)
    list(APPEND SRCS sim_gpio.c)
  endif()
endif()

if(CONFIG_ARCH_BUTTONS)
  nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_buttons.c
  ${CMAKE_CURRENT_LIST_DIR}/sim_buttons.c)
  list(APPEND SRCS sim_buttons.c)
endif()

if(CONFIG_MOTOR_FOC_DUMMY)
  nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_foc.c
  ${CMAKE_CURRENT_LIST_DIR}/sim_foc.c)
  list(APPEND SRCS sim_foc.c)
endif()

if(CONFIG_USBDEV_COMPOSITE)
  nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/sim_composite.c
  ${CMAKE_CURRENT_LIST_DIR}/sim_composite.c)
  list(APPEND SRCS sim_composite.c)
endif()

if(MSVC)
  list(APPEND SRCS sim_msvc_compat.c sim_fork_msvc_stub.c)
endif()

target_sources(board PRIVATE ${SRCS})

set_property(GLOBAL PROPERTY LD_SCRIPT "${NUTTX_BOARD_DIR}/scripts/gnu-elf.ld")

if(CONFIG_ETC_ROMFS)
  nuttx_create_symlink( ${SIM_COMMON_BOARD}/src/etc
  ${CMAKE_CURRENT_LIST_DIR}/etc)
  nuttx_add_romfs(
    NAME
    etc
    MOUNTPOINT
    etc
    RCSRCS
    etc/init.d/rcS
    etc/init.d/rc.sysinit
    RCRAWS
    etc/group
    etc/passwd
    PATH
    ${CMAKE_CURRENT_BINARY_DIR}/etc)

  target_link_libraries(board PRIVATE romfs_etc)
endif()

if(NOT MSVC)
  target_compile_options(board PRIVATE -Werror)
endif()
if(NOT MSVC)
  target_compile_options(nuttx PRIVATE -Werror)
endif()

set(AUTOSAROS_DIR "${NUTTX_APPS_DIR}/components/rt_framework")
        set(INCLUDE_DIRS  ${GENDATA_DIR}/Os
                          ${GENDATA_DIR}/Memmap
                          ${GENDATA_DIR}/Rte
                          ${AUTOSAROS_DIR}/inc
                          ${NUTTX_APPS_DIR}/components/include
                          ${NUTTX_APPS_DIR}/components/include/autosar)
target_include_directories(board PRIVATE ${INCLUDE_DIRS})
