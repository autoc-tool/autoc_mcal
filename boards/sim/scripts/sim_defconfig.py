#!/usr/bin/env python
# This Python file uses the following encoding: utf-8

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

import os
import sys
import argparse
import shutil
import subprocess
import fnmatch
from pathlib import Path

_CURRENT_PATH = os.path.dirname(os.path.abspath(__file__))
ROOT_PATH = os.path.join(_CURRENT_PATH, "../../../../../")

sys.path.append(os.path.join(ROOT_PATH, 'kernel', 'nuttx', 'tools'))
from process_includes import process_file

class DefConfigParser:
	def __init__(self, filepath, destpath, codegen_dir):
		self.codegen_dir = codegen_dir
		self.filepath = filepath
		self.destfilepath = destpath
		self.config = {}
		self.read_config()

	def read_config(self):
		"""Read and parse the defconfig file."""
		self.preprocess_config()
		with open(self.destfilepath, 'r') as file:
			for line in file:
				line = line.strip()
				if line and not line.startswith('#'):
					key, sep, value = line.partition('=')
					if sep:  # Only add lines with '=' separator
						self.config[key.strip()] = value.strip()

	def preprocess_config(self):
		"""Preprocess codegen defconfig macros."""
		dest_file = Path(self.destfilepath)
		process_file(dest_file, self.filepath, [self.codegen_dir])

	def get(self, key, default=None):
		"""Get the value of a configuration."""
		return self.config.get(key, default)

	def set(self, key, value):
		"""Set the value of a configuration."""
		self.config[key] = value

	def delete(self, key):
		"""Delete a configuration item."""
		if key in self.config:
			del self.config[key]

	def save(self, filepath=None):
		"""Save the current configuration to a file."""
		filepath = filepath or self.destfilepath
		with open(filepath, 'w') as file:
			for key, value in sorted(self.config.items()):
				file.write(f"{key}={value}\n")


def main():

	"""
	because sim not suppot hrt in rtframework,
	so if user use hrt,we need do some prebuild change"
	"""
	parser = argparse.ArgumentParser(description="defconfig process")
	# 使用默认路径，后期使用cfg路径可以通过cmake框架传参传入
	parser.add_argument("-s", type=str, help="defcfg dir")
	parser.add_argument("-d", type=str, help="defcfg dir")
	parser.add_argument('-i', type=str, help='project dir')
	args = parser.parse_args()

	source_defconfig_patch = os.path.join(args.s, "defconfig")
	dest_defconfig_patch = os.path.join(args.d, "defconfig")
	codegen_inc_defconfig_path = os.path.join(os.path.dirname(args.i), 'gen_data')

	defconfig = DefConfigParser(source_defconfig_patch, dest_defconfig_patch, codegen_inc_defconfig_path)

	# Get a configuration value

	# Set a new configuration value
	defconfig.set('CONFIG_ARCH', '"sim"')
	defconfig.set('CONFIG_ARCH_CHIP', '"sim"')

	custm_board = args.d
	custm_board = os.path.dirname(custm_board)
	custm_board = os.path.dirname(custm_board)
	custm_board = f'\"{custm_board}\"'
	chip_custm = custm_board.replace("boards","chips")
	defconfig.set('CONFIG_ARCH_BOARD_CUSTOM_DIR', custm_board)
	defconfig.set('CONFIG_ARCH_CHIP_CUSTOM_DIR', chip_custm)
	defconfig.set('CONFIG_ARCH_SIM', 'y')
	defconfig.set('CONFIG_SIM_IRQ_MANAGE_DISPATCH_BY_IRQID', 'y')
	defconfig.set('CONFIG_BOARDCTL_POWEROFF', 'y')
	defconfig.set('CONFIG_RTC', 'y')
	defconfig.set('CONFIG_RTC_DRIVER', 'y')
	defconfig.set('CONFIG_RTC_ARCH', 'y')

	defconfig.set('CONFIG_NSH_SC', 'y')
	defconfig.set('CONFIG_RT_FRAMEWORK_TIMER', 'n')
	defconfig.set('CONFIG_RT_FRAMEWORK_IOC', 'n')
	defconfig.set('CONFIG_RT_FRAMEWORK_HOOK', 'n')
	defconfig.set('CONFIG_LIBC_EXECFUNCS', 'y')
	defconfig.set('CONFIG_DRIVERS_NOTEDDR', 'n')
	# Register the virtual serial port of the host machine
	defconfig.set('CONFIG_SIM_UART_NUMBER', '2')
	defconfig.set('CONFIG_SIM_UART0_NAME', '"/dev/vttyS0"')
	defconfig.set('CONFIG_SIM_UART1_NAME', '"/dev/vttyS1"')
	# Delete a configuration value
	defconfig.delete('CONFIG_ARCH_CHIP_ARM_CUSTOM')
	defconfig.delete('CONFIG_ARCH_ARM')
	defconfig.delete('CONFIG_RT_FRAMEWORK_SYSCALL')
	defconfig.delete('CONFIG_ARCH_CHIP_E3650_GICD_BASE')
	defconfig.delete('CONFIG_ARCH_CHIP_E3650_GICR_BASE')
	defconfig.delete('CONFIG_ARCH_CHIP_E3650_TCM_ENABLE')
	defconfig.delete('CONFIG_STACK_OVERFLOW_CHECK')
	defconfig.delete('CONFIG_ARCH_IRQ_NESTING_LIMIT')
	defconfig.delete('CONFIG_DISABLE_PTHREAD')

	# Turn on the virtual driver function
	defconfig.set('CONFIG_SIM_EEP_DRIVER', 'y')
	defconfig.set('CONFIG_SIM_M32', 'y')
	# Save changes back to the file
	defconfig.save(dest_defconfig_patch)


if __name__ == "__main__":
	main()