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

#include "ea.h"
#include "Ea.h"
#include "Eep.h"

#define EEP_MAX_EXE_TIMES       (10000u)
#define EEP_PREINIT_DELAY_US    (10000u)
#define EEP_REG_OFFSET          (0x800u)
#define EEP_REG_WIDTH           (1u)

static MemIf_JobResultType ea_eep_wait_mainfunction(void)
{
	MemIf_JobResultType job_result = MEMIF_JOB_FAILED;
	static uint16 max_times = EEP_MAX_EXE_TIMES;

	while (max_times > 0u) {
		Ea_MainFunction();
		Eep_MainFunction();

		job_result = Ea_GetJobResult();

		if (MEMIF_JOB_OK == job_result) {
			return E_OK;
		} else if (MEMIF_JOB_PENDING == job_result) {
		} else {
			return job_result;
		}
		max_times--;
	}
	return job_result;
}

void ea_set_mode(uint8 mode)
{
	Ea_SetMode(mode);
}

uint8 ea_read(uint16 block_number, uint16 block_offset, uint8 *data_buffer_ptr, uint16 length)
{
	return Ea_Read(block_number, block_offset, data_buffer_ptr, length);
}

uint8 ea_write(uint16 block_number, const uint8 *data_buffer_ptr)
{
	return Ea_Write(block_number, data_buffer_ptr);
}

void ea_cancel(void)
{
	Ea_Cancel();
}

uint8 ea_get_status(void)
{
	return Ea_GetStatus();
}

uint8 ea_get_job_result(void)
{
	return Ea_GetJobResult();
}

uint8 ea_invalidate_block(uint16 block_number)
{
	return Ea_InvalidateBlock(block_number);
}

uint8 ea_erase_immediate_block(uint16 block_number)
{
	return Ea_EraseImmediateBlock(block_number);
}

void ea_main_function(void)
{
	Ea_MainFunction();
}

void eep_main_function(void)
{
	Eep_MainFunction();
}

uint8 ea_eep_init(void)
{
	uint8 ret = 0;
	static uint16 max_times = EEP_MAX_EXE_TIMES;

	if (Eep_GetStatus() == MEMIF_UNINIT) {
		Eep_Init(&Eep_ConfigData);
	}

	if (MEMIF_UNINIT == Eep_GetStatus()) {
		return E_NOT_OK;
	}

	MemIf_JobResultType job_result = MEMIF_JOB_FAILED;
	MemIf_StatusType mode_status = MEMIF_UNINIT;

	Ea_Init(NULL);

	while (max_times > 0) {
		Ea_MainFunction();
		Eep_MainFunction();

		mode_status = Ea_GetStatus();

		if (MEMIF_IDLE == mode_status) {
			ret = 0;
			break;
		} else if (MEMIF_BUSY_INTERNAL == mode_status) {
		} else {
			ret = E_NOT_OK;
			break;
		}
		max_times--;
	}

	job_result = Ea_GetJobResult();

	if (MEMIF_JOB_OK != job_result) {
		ret = E_NOT_OK;
	}

	return ret;
}

uint8 ea_write_block(uint16 block_number, uint8 *buffer_ptr)
{
	uint8 ret = 0;

	ret = Ea_Write(block_number, buffer_ptr);

	if (E_OK != ret) {
		return ret;
	}
	MemIf_JobResultType job_result = ea_eep_wait_mainfunction();
	return job_result;
}

uint8 ea_read_block(uint16 block_number, uint16 block_offset, uint8 *buffer_ptr, uint16 length)
{
	uint8 ret = E_NOT_OK;

	ret = Ea_Read(block_number, block_offset, buffer_ptr, length);

	if (E_OK != ret) {
		return ret;
	}
	MemIf_JobResultType job_result = ea_eep_wait_mainfunction();
	return job_result;
}
