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

#ifndef VETH_ADAPT_H
#define VETH_ADAPT_H

#include "Eth_PBCfg.h"

#define vETH_GETDROPCOUNT_API STD_OFF
#define vETH_GETETHERSTATS_API STD_OFF
#define vETH_GLOBALTIMESUPPORT ETH_GET_GLOBALTIME_API
#define vETH_ICMP_CHECKSUMOFFLOAD_ENABLE STD_OFF
#define vETH_IPV4_CHECKSUMOFFLOAD_ENABLE STD_OFF
#define vETH_TCP_CHECKSUMOFFLOAD_ENABLE STD_OFF
#define vETH_UDP_CHECKSUMOFFLOAD_ENABLE STD_OFF
#define vETH_DEV_ERROR_DETECT ETH_DEV_ERROR_DETECT
#define vETH_VERSION_INFO_API ETH_GET_VERSION_INFO_API
#define vETH_UPDATE_PHY_ADDR_FILTER_API ETH_UPDATE_FILTER_API
#define vETH_MAX_CONTROLLERS ETH_MAXCTRLS_SUPPORTED

#define vETH_MODULE_ID ETH_MODULE_ID
#define vETH_INSTANCE_ID ETH_DRIVER_INSTANCE

#define vETH_E_DET_NO_ERR ((uint8)0x00)
#define vETH_E_INV_CTRL_IDX ((uint8)0x01)
#define vETH_E_NOT_INITIALIZED ((uint8)0x02)
#define vETH_E_PARAM_POINTER ((uint8)0x03)
#define vETH_E_INV_PARAM ((uint8)0x04)
#define vETH_E_INV_MODE ((uint8)0x05)
#define vETH_E_INIT_FAILED ((uint8)0x20)
#define vETH_E_CORE_NOT_CONFIGURED ((uint8)0x64)

#define vETH_SID_INIT ((uint8)0x01)
#define vETH_SID_SETCONTROLLERMODE ((uint8)0x02)
#define vETH_SID_GETCONTROLLERMODE ((uint8)0x03)
#define vETH_SID_WRITEMII ((uint8)0x05)
#define vETH_SID_READMII ((uint8)0x06)
#define vETH_SID_GETPHYADDRESS ((uint8)0x08)
#define vETH_SID_PROVIDETXBUFFER ((uint8)0x09)
#define vETH_SID_TRANSMIT ((uint8)0x0A)
#define vETH_SID_RECEIVE ((uint8)0x0B)
#define vETH_SID_TXCONFIRMATION ((uint8)0x0C)
#define vETH_SID_GETVERSIONINFO ((uint8)0x0D)
#define vETH_SID_RXHANDLER ((uint8)0x10)
#define vETH_SID_TXHANDLER ((uint8)0x11)
#define vETH_SID_UPDATEFILTER ((uint8)0x12)
#define vETH_SID_SETPHYSADDRESS ((uint8)0x13)
#define vETH_SID_GETDROPCOUNT ((uint8)0x14)
#define vETH_SID_GETETHERSTATS ((uint8)0x15)
#define vETH_SID_GETCURRENTTIME ((uint8)0x16)
#define vETH_SID_ENABLEEGRESSTIMESTAMP ((uint8)0x17)
#define vETH_SID_GETEGRESSTIMESTAMP ((uint8)0x18)
#define vETH_SID_GETINGRESSTIMESTAMP ((uint8)0x19)
#define vETH_SID_SETCORRECTIONTIME ((uint8)0x1A)
#define vETH_SID_SETGLOBALTIME ((uint8)0x1B)

#define ISR_ETHTX_ID (140)
#define ISR_ETHRX_ID (140)
#define vETH_DEFAULT_TX_CHANNEL_ID (0x00)
#define vETH_DEFAULT_RX_CHANNEL_ID (0x00)
#define vETH_MAX_TXCHANNEL_NUM (0x01)
#define vETH_MAX_RXCHANNEL_NUM (0x01)

#endif
