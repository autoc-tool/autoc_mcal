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

#ifndef VEEP_CFG_H
#define VEEP_CFG_H

#include "Eep.h"

#define vEEP_DEV_ERROR_DETECT STD_ON

#define vEEP_MODULE_ID EEP_MODULE_ID
#define vEEP_INSTANCE_ID ((uint8)0x00)

#define vEEP_INIT_SERVICE_ID EEP_INIT_SERVICE_ID
#define vEEP_SET_MODE_SERVICE_ID EEP_SET_MODE_SERVICE_ID
#define vEEP_READ_SERVICE_ID EEP_READ_SERVICE_ID
#define vEEP_WRITE_SERVICE_ID EEP_WRITE_SERVICE_ID
#define vEEP_ERASE_SERVICE_ID EEP_ERASE_SERVICE_ID
#define vEEP_COMPARE_SERVICE_ID EEP_COMPARE_SERVICE_ID
#define vEEP_CANCEL_SERVICE_ID EEP_CANCEL_SERVICE_ID
#define vEEP_GET_STATUS_SERVICE_ID EEP_GET_STATUS_SERVICE_ID
#define vEEP_GET_JOBRESULT_SERVICE_ID EEP_GET_JOBRESULT_SERVICE_ID
#define vEEP_MAINFUNCTION_SERVICE_ID EEP_MAINFUNCTION_SERVICE_ID
#define vEEP_GET_VERSION_SERVICE_ID EEP_GET_VERSION_SERVICE_ID
#define vEEP_SERVICE_ID_MAX EEP_SERVICE_ID_MAX

#define vEEP_E_INIT_FAILED EEP_E_INIT_FAILED
#define vEEP_E_PARAM_ADDRESS EEP_E_PARAM_ADDRESS
#define vEEP_E_PARAM_DATA EEP_E_PARAM_DATA
#define vEEP_E_PARAM_LENGTH EEP_E_PARAM_LENGTH
#define vEEP_E_PARAM_POINTER EEP_E_PARAM_POINTER
#define vEEP_E_UNINIT EEP_E_UNINIT
#define vEEP_E_BUSY EEP_E_BUSY
#define vEEP_E_TIMEOUT EEP_E_TIMEOUT

#endif
