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

#ifndef ETH_H
#define ETH_H

#include "Eth_PBCfg.h"

#define   ETH1  (0u)
#define   ETH2  (1u)

/* MTL algorithms identifiers */
#define MTL_TX_ALGORITHM_WRR    0x0u
#define MTL_TX_ALGORITHM_WFQ    0x1u
#define MTL_TX_ALGORITHM_DWRR   0x2u
#define MTL_TX_ALGORITHM_SP     0x3u
#define MTL_RX_ALGORITHM_SP     0x4u
#define MTL_RX_ALGORITHM_WSP    0x5u
#define MTL_TX_ALGORITHM_CC     0x6u
/* RX/TX Queue Mode */
#define MTL_QUEUE_MODE_AVB      0x1u
#define MTL_QUEUE_MODE_DCB      0x2u

enum {
	MTL_MEM_POLICY_DEFAULT = 0,
	MTL_MEM_POLICY_MANUAL
};

#endif
