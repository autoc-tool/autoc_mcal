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
import json

default_app_path = "applications/tc397_demo"
_CURRENT_PATH = os.path.dirname(os.path.abspath(__file__))
ROOT_PATH = os.path.join(_CURRENT_PATH, "../../../../../")
BUILD_PATH = os.path.join(ROOT_PATH, "build")
SIM_SCRIPTS_PATH = os.path.join(BUILD_PATH, "sim_scripts")
APPS_PATH = os.path.join(ROOT_PATH, "apps")
VCOS_STUDIO_PATH = os.path.join(ROOT_PATH, "vcos_studio")
VCOS_STUDIO_CONFIGURATOR_PATH = os.path.join(VCOS_STUDIO_PATH, "configurator")
THIRDPARTY_LIBRARY_PATH = os.path.join(VCOS_STUDIO_CONFIGURATOR_PATH, "third_party")
sys.path.append(VCOS_STUDIO_CONFIGURATOR_PATH)
sys.path.append(THIRDPARTY_LIBRARY_PATH)
sys.path.append(SIM_SCRIPTS_PATH)
from basic_func_module.autosar_utils.utils import get_short_name,set_value,get_definition_ref,get_value
from basic_func_module.autosar_utils.autosar_util import AutosarUtil
from basic_func_module.file_handler.file_io_handler import FileIOHandler
from cmd_api.auto_cli_api import AutoCliApi
from cmd_adaptor.data_management import get_autosar_bsw_class
from cmd_adaptor.cmd_interaction.cmd_logger import logger
from sim_common_function import save_schedule_json,OS_ECUC_PATH,SCH_TABLE_PATH

def process_arxml(args):
    file_path = os.path.join(os.path.dirname(args.m), OS_ECUC_PATH)

    # 定义备份文件路径
    backup_path = _CURRENT_PATH + "os_ecuc.bak"

    # 复制文件为 .bak 文件
    try:
        shutil.copy2(file_path, backup_path)
    except FileNotFoundError as e:
        print(f"文件或目录不存在: {e}")
    except PermissionError as e:
        print(f"权限不足: {e}")
    except Exception as e:
        print(f"复制失败: {e}")

    # codegen
    # 执行另一个 Python 脚本
    def find_vcosproject(directory):
        for root, _, files in os.walk(directory):
            for filename in files:
                if fnmatch.fnmatch(filename, '*.vcosproject'):
                    return os.path.join(root, filename)
        return None

    project = args.m
    codegen_script  = os.path.join(VCOS_STUDIO_CONFIGURATOR_PATH, "cmd_api/auto_cli.py")

    result1 = AutoCliApi.load_project(project)
    if result1 is False:
        logger.info("load project failed")
        sys.exit(-1)
    dict_allclass = get_autosar_bsw_class()
    autosar_os_class = dict_allclass["Os"]
    container_value_s = autosar_os_class.find_nodes("ECUC-CONTAINER-VALUE")
    for container_value in container_value_s:
        name = get_short_name(container_value)
        refname = get_definition_ref(container_value)
        if refname == "/AUTOSAR/EcucDefs/Os/OsCounter":
            subcontainer = container_value.SUB_CONTAINERS
            # 如果存在 subcontainer 全删掉 软件定时器不配置这个属性
            if subcontainer :
                for i in range(len(subcontainer.ECUC_CONTAINER_VALUE)):
                    sub = subcontainer.ECUC_CONTAINER_VALUE[i]
                    # value_list = getattr(sub, subcontainer.get_tag().replace("-", "_"), None)
                    AutosarUtil.remove_container_value(subcontainer.ECUC_CONTAINER_VALUE, sub)
            # 更改定时器模式为软件模式 1tick0.001秒
            param = container_value.PARAMETER_VALUES
            for numerical in param.ECUC_NUMERICAL_PARAM_VALUE:
                if(get_definition_ref(numerical) == "/AUTOSAR/EcucDefs/Os/OsCounter/OsSecondsPerTick"):
                    set_value(numerical,"0.001")
            for text in param.ECUC_TEXTUAL_PARAM_VALUE:
                if(get_definition_ref(text) == "/AUTOSAR/EcucDefs/Os/OsCounter/OsCounterType"):
                    set_value(text,"SOFTWARE")
        if refname == "/AUTOSAR/EcucDefs/Os/OsIsr":
            # 寻找 systick handler name
            param = container_value.PARAMETER_VALUES
            for text in param.ECUC_TEXTUAL_PARAM_VALUE:
                if(get_definition_ref(text) == "/AUTOSAR/EcucDefs/Os/OsIsr/OsIsrEntryFunctionName"):
                    if("systick_handler" in get_value(text) or "os_counter_hrt_handler" in get_value(text)):
                        set_value(text,"rt_fwk_tick_handler")
    #change 调度表json
    file1_path = os.path.join(os.path.dirname(args.m) , SCH_TABLE_PATH)
    save_schedule_json(file1_path)
    AutoCliApi.generate_all_schedule_table()
    AutoCliApi.save_project()

def revert_arxml(args):
    file_path = os.path.join(os.path.dirname(args.m), OS_ECUC_PATH)
    # 定义备份文件路径
    backup_path = _CURRENT_PATH + "os_ecuc.bak"
    # 删除原始文件
    print(file_path)
    os.remove(file_path)
    print(backup_path)
    # 将 .bak 文件改回原始文件名
    os.rename(backup_path, file_path)


def main():

    """
    because sim not suppot hrt in rtframework,
    so if user use hrt,we need do some prebuild change"
    """
    parser = argparse.ArgumentParser(description="VCOS Builder")
    # 使用默认路径，后期使用cfg路径可以通过cmake框架传参传入
    parser.add_argument("-m", type=str, default=default_app_path, help="cfg dir")
    parser.add_argument("-r", action="store_true", default=False, help="revert arxml")
    args = parser.parse_args()
    LOG_APTH = os.path.join(_CURRENT_PATH)
    logger.init_default_handler(log_path=LOG_APTH)
    if args.r:
        revert_arxml(args)
    else:
        process_arxml(args)

if __name__ == "__main__":
    main()
