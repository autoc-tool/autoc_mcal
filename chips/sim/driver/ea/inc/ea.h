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

#ifndef EA_H__
#define EA_H__

#include "Std_Types.h"

void ea_set_mode(uint8 mode);
uint8 ea_read(uint16 block_number, uint16 block_offset, uint8 *data_buffer_ptr, uint16 length);
uint8 ea_write(uint16 block_number, const uint8 *data_buffer_ptr);
void ea_cancel(void);
uint8 ea_get_status(void);
uint8 ea_get_job_result(void);
uint8 ea_invalidate_block(uint16 block_number);
uint8 ea_erase_immediate_block(uint16 block_number);
void ea_main_function(void);
void eep_main_function(void);
uint8 ea_eep_init(void);
uint8 ea_write_block(uint16 block_number, uint8 *buffer_ptr);
uint8 ea_read_block(uint16 block_number, uint16 block_offset, uint8 *buffer_ptr, uint16 length);

#endif
