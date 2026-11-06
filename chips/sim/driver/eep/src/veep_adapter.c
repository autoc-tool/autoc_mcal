/*
 * Copyright (c) 2025 Li Auto Inc. and its affiliates
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <syslog.h>

#include "Eep.h"
#include "veep.h"

#define EEP_CFG_WRITE_BLOCK_SIZE (4096u)
#define EEP_CFG_READ_BLOCK_SIZE (1024u)

static veep_init_config_type *veep_converted_config;

static Std_ReturnType eep_e3650_to_veep_config(const Eep_ConfigType *config_ptr, veep_init_config_type *veep_config);

static Std_ReturnType eep_e3650_to_veep_config(const Eep_ConfigType *config_ptr, veep_init_config_type *veep_config)
{
	Eep_InitConfigType init_config;

	if (config_ptr == NULL) {
		return E_NOT_OK;
	}
	init_config = config_ptr->initConfig;
	veep_config->default_mode = init_config.defaultMode;
	veep_config->fast_read_block_size =
		(init_config.fastReadBlockSize == 0) ? EEP_CFG_READ_BLOCK_SIZE : init_config.fastReadBlockSize;
	veep_config->fast_write_block_size =
		(init_config.fastWriteBlockSize == 0) ? EEP_CFG_WRITE_BLOCK_SIZE : init_config.fastWriteBlockSize;
	veep_config->normal_read_block_size =
		(init_config.normalReadBlockSize == 0) ? EEP_CFG_READ_BLOCK_SIZE : init_config.normalReadBlockSize;
	veep_config->normal_write_block_size =
		(init_config.normalWriteBlockSize == 0) ? EEP_CFG_WRITE_BLOCK_SIZE : init_config.normalWriteBlockSize;
	for (uint8 i = 0; i < EEP_CFG_INIT_BLOCK_NUM; i++) {
		veep_config->block_config[i].base_address = init_config.blockConfig[i].baseAddress;
		veep_config->block_config[i].size = init_config.blockConfig[i].size;
		veep_config->block_config[i].job_end_notification = init_config.blockConfig[i].jobEndNotification;
		veep_config->block_config[i].job_error_notification = init_config.blockConfig[i].jobErrorNotification;
		veep_config->block_config[i].priority = init_config.blockConfig[i].priority;
	}

	return E_OK;
}

void Eep_Init(const Eep_ConfigType *ConfigPtr)
{
	if (veep_converted_config == NULL) {
		veep_converted_config = malloc(sizeof(veep_init_config_type));
		if (veep_converted_config == NULL) {
			veep_print(LOG_DEBUG, "[vEep] Invalid init para\n");
			return;
		}
	}
	if (E_OK == eep_e3650_to_veep_config(ConfigPtr, veep_converted_config)) {
		veep_init(veep_converted_config);
	} else {
		veep_print(LOG_DEBUG, "[vEep] Invalid init para\n");
	}
}

void Eep_SetMode(MemIf_ModeType Mode)
{
	veep_set_mode(Mode);
}

Std_ReturnType Eep_Read(Eep_AddressType EepromAddress, uint8 *DataBufferPtr, Eep_LengthType Length)
{
	return veep_read(EepromAddress, DataBufferPtr, Length);
}

Std_ReturnType Eep_Write(Eep_AddressType EepromAddress, const uint8 *DataBufferPtr, Eep_LengthType Length)
{
	return veep_write(EepromAddress, DataBufferPtr, Length);
}

void Eep_Cancel(void)
{
	veep_cancel();
}

MemIf_StatusType Eep_GetStatus(void)
{
	return veep_get_status();
}

MemIf_JobResultType Eep_GetJobResult(void)
{
	return veep_get_job_result();
}

void Eep_MainFunction(void)
{
	veep_mainfunction();
}
