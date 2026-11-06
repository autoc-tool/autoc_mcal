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
#include "Eth_PBCfg.h"
#include "Eth_GeneralTypes.h"
#include "eth_general_types.h"
#include "veth.h"
#include "comstack_types.h"
#include "sim_port_adapt.h"
#include "veth_internal.h"

#ifndef DEFAULT_MAC_ADDR
#ifdef CONFIG_NODE_MAC_ADDRESS
#define DEFAULT_MAC_ADDR (CONFIG_NODE_MAC_ADDRESS)
#else
#define DEFAULT_MAC_ADDR (0x123456789ABCu)
#endif
#endif

static veth_config_type veth_config_set;
static Std_ReturnType eth_semi_cfg_to_veth_config(const Eth_ConfigType *const cfg_ptr,
	veth_config_type *veth_config_ptr);
static Std_ReturnType veth_get_chnl_cfg(const Eth_CntrlConfigType *src_ctrl_cfg,
	veth_core_ctrl_config_type *dest_ctrl_cfg);
static uint8 eth_controller_index_map[ETH_MAXCTRLS_SUPPORTED] = { 0x0U };
static uint8 eth_ctrl_id_to_coreid_map[ETH_MAXCTRLS_SUPPORTED] = { 0x0U };

static uint8 veth_tx_prio_chnl_map_ctrl[vETH_MAX_FRAME_PRIORITY] = {
	/* 0 */ 0x00U,
	/* 1 */ 0x00U,
	/* 2 */ 0x00U,
	/* 3 */ 0x00U,
	/* 4 */ 0x00U,
	/* 5 */ 0x00U,
	/* 6 */ 0x00U,
	/* 7 */ 0x00U
};

static uint8 veth_rx_prio_chnl_map_ctrl[vETH_MAX_FRAME_PRIORITY] = {
	/* 0 */ 0x00U,
	/* 1 */ 0x00U,
	/* 2 */ 0x00U,
	/* 3 */ 0x00U,
	/* 4 */ 0x00U,
	/* 5 */ 0x00U,
	/* 6 */ 0x00U,
	/* 7 */ 0x00U
};

static uint8 tx_prio_sorted_chnl[vETH_MAX_TXCHANNEL_NUM] = {0};
static uint8 rx_prio_sorted_chnl[vETH_MAX_RXCHANNEL_NUM] = {0};

static Std_ReturnType eth_semi_cfg_to_veth_config(const Eth_ConfigType *const cfg_ptr,
																		veth_config_type *veth_config_ptr)
{
	uint8 core_idx;
	uint8 ctrl_idx;
	const Eth_CoreConfigType *src_core_cfg = NULL;
	veth_core_config_type *dest_core_cfg  = NULL;
	const Eth_CntrlConfigType *src_ctrl_cfg = NULL;
	veth_core_ctrl_config_type *dest_ctrl_cfg = NULL;

	if ((NULL == cfg_ptr) || (NULL == veth_config_ptr)) {
		return E_NOT_OK;
	}

	for (core_idx = 0; core_idx < OS_CORE_CNT; core_idx++) {
		src_core_cfg = cfg_ptr->EthCoreConfig[core_idx];
		if (NULL == src_core_cfg) {
			veth_config_ptr->veth_core_add[core_idx] = NULL;
			continue;
		}
		dest_core_cfg = (veth_core_config_type *)malloc(sizeof(veth_core_config_type));
		if (NULL == dest_core_cfg) {
			return E_NOT_OK;
		}
		dest_core_cfg->veth_max_controllers = src_core_cfg->EthMaxControllers;
		dest_core_cfg->veth_core_ctrl_ptr = (veth_core_ctrl_config_type *)malloc(
			src_core_cfg->EthMaxControllers * sizeof(veth_core_ctrl_config_type));
		if (NULL == dest_core_cfg->veth_core_ctrl_ptr)
		{
			free(dest_core_cfg);
			return E_NOT_OK;
		}

		for (ctrl_idx = 0; ctrl_idx < src_core_cfg->EthMaxControllers; ctrl_idx++)
		{
			src_ctrl_cfg = &(src_core_cfg->EthCntrlConfigPtr[ctrl_idx]);
			dest_ctrl_cfg = &(dest_core_cfg->veth_core_ctrl_ptr[ctrl_idx]);
			dest_ctrl_cfg->tx_int_enable = src_ctrl_cfg->EnableTxInterrupt;
			dest_ctrl_cfg->rx_int_enable = src_ctrl_cfg->EnableRxInterrupt;
			dest_ctrl_cfg->eth_ctrl_idx = src_ctrl_cfg->EthCtrlIdx;
			dest_ctrl_cfg->tx_traffic_algorithm = src_ctrl_cfg->TxTrafficAlgorithms;
			dest_ctrl_cfg->rx_traffic_algorithm = vETH_RX_ALGORITHM_SP;
			dest_ctrl_cfg->eth_tx_prio_to_chnl_map_ptr = veth_tx_prio_chnl_map_ctrl;
			dest_ctrl_cfg->eth_rx_prio_to_chnl_map_ptr = veth_rx_prio_chnl_map_ctrl;
			dest_ctrl_cfg->tx_chnl_num = vETH_MAX_TXCHANNEL_NUM;
			dest_ctrl_cfg->rx_chnl_num = vETH_MAX_RXCHANNEL_NUM;
			dest_ctrl_cfg->rx_prio_sorted_chnl_ptr = rx_prio_sorted_chnl;
			dest_ctrl_cfg->tx_prio_sorted_chnl_ptr = tx_prio_sorted_chnl;
			dest_ctrl_cfg->eth_mac_address[0] = (DEFAULT_MAC_ADDR >> 40) & 0xFF;
			dest_ctrl_cfg->eth_mac_address[1] = (DEFAULT_MAC_ADDR >> 32) & 0xFF;
			dest_ctrl_cfg->eth_mac_address[2] = (DEFAULT_MAC_ADDR >> 24) & 0xFF;
			dest_ctrl_cfg->eth_mac_address[3] = (DEFAULT_MAC_ADDR >> 16) & 0xFF;
			dest_ctrl_cfg->eth_mac_address[4] = (DEFAULT_MAC_ADDR >> 8) & 0xFF;
			dest_ctrl_cfg->eth_mac_address[5] = (DEFAULT_MAC_ADDR) & 0xFF;
			eth_controller_index_map[src_ctrl_cfg->EthCtrlIdx] = ctrl_idx;
			eth_ctrl_id_to_coreid_map[src_ctrl_cfg->EthCtrlIdx] = core_idx;
			if (E_OK != veth_get_chnl_cfg(src_ctrl_cfg, dest_ctrl_cfg)) {
				free(dest_core_cfg->veth_core_ctrl_ptr);
				free(dest_core_cfg);
				return E_NOT_OK;
			}
		}
		veth_config_ptr->veth_core_add[core_idx] = dest_core_cfg;
		veth_config_ptr->veth_glb_to_lcl_ctrlid_map_ptr = eth_controller_index_map;
		veth_config_ptr->veth_ctrlid_to_coreid_map_ptr = eth_ctrl_id_to_coreid_map;
	}
	return E_OK;
}

static Std_ReturnType veth_get_chnl_cfg(const Eth_CntrlConfigType *src_ctrl_cfg,
															veth_core_ctrl_config_type *dest_ctrl_cfg)
{
	uint8 i;

	if (NULL == src_ctrl_cfg || NULL == dest_ctrl_cfg) {
		return E_NOT_OK;
	}

	dest_ctrl_cfg->tx_chnl_cfg = (veth_ctrl_chnl_config_type *)malloc(dest_ctrl_cfg->tx_chnl_num *
																			sizeof(veth_ctrl_chnl_config_type));
	if (NULL == dest_ctrl_cfg->tx_chnl_cfg) {
		return E_NOT_OK;
	}
	dest_ctrl_cfg->rx_chnl_cfg = (veth_ctrl_chnl_config_type *)malloc(dest_ctrl_cfg->rx_chnl_num *
																	sizeof(veth_ctrl_chnl_config_type));
	if (NULL == dest_ctrl_cfg->rx_chnl_cfg) {
		free(dest_ctrl_cfg->tx_chnl_cfg);
		return E_NOT_OK;
	}
	for (i = 0; i < dest_ctrl_cfg->tx_chnl_num; i++) {
		dest_ctrl_cfg->tx_chnl_cfg[i].chnl_buf_len_byte = src_ctrl_cfg->Mtu;
		dest_ctrl_cfg->tx_chnl_cfg[i].chnl_buf_total = CTRL0_TX_BUFF_COUNT;
		dest_ctrl_cfg->tx_chnl_cfg[i].chnl_idx = i;
		dest_ctrl_cfg->tx_chnl_cfg[i].irq_num = ISR_ETHTX_ID;
		dest_ctrl_cfg->tx_chnl_cfg[i].core_id = eth_ctrl_id_to_coreid_map[src_ctrl_cfg->EthCtrlIdx];
		if (i == 0) {
			dest_ctrl_cfg->tx_chnl_cfg[i].core_buf_start_idx = 0;
		} else {
			dest_ctrl_cfg->tx_chnl_cfg[i].core_buf_start_idx =
										dest_ctrl_cfg->tx_chnl_cfg[i-1].core_buf_start_idx +
										dest_ctrl_cfg->tx_chnl_cfg[i-1].chnl_buf_total;
		}
	}

	for (i = 0; i < dest_ctrl_cfg->rx_chnl_num; i++) {
		dest_ctrl_cfg->rx_chnl_cfg[i].chnl_buf_len_byte = src_ctrl_cfg->Mtu;
		dest_ctrl_cfg->rx_chnl_cfg[i].chnl_buf_total = CTRL0_RX_BUFF_COUNT;
		dest_ctrl_cfg->rx_chnl_cfg[i].chnl_idx = i;
		dest_ctrl_cfg->rx_chnl_cfg[i].irq_num = ISR_ETHRX_ID;
		dest_ctrl_cfg->rx_chnl_cfg[i].core_id = eth_ctrl_id_to_coreid_map[src_ctrl_cfg->EthCtrlIdx];
		if (i == 0) {
			dest_ctrl_cfg->rx_chnl_cfg[i].core_buf_start_idx = 0;
		} else {
			dest_ctrl_cfg->rx_chnl_cfg[i].core_buf_start_idx =
										dest_ctrl_cfg->rx_chnl_cfg[i-1].core_buf_start_idx +
										dest_ctrl_cfg->rx_chnl_cfg[i-1].chnl_buf_total;
		}
	}
	return E_OK;
}

void eth_gethmac_init(void)
{
	eth_semi_cfg_to_veth_config(&Eth_Control_Config, &veth_config_set);
	veth_init(&veth_config_set);
}

void Eth_GetPhysAddr(uint8 ctrl_idx, uint8 *phys_addr_ptr)
{
	veth_get_phys_addr(ctrl_idx, phys_addr_ptr);
}

BufReq_ReturnType Eth_ProvideTxBuffer(uint8 ctrl_idx, uint8 priority, Eth_BufIdxType *bufidx_ptr, Eth_DataType **buf_ptr,
					uint16 *lenbyte_ptr)
{
	return veth_provide_tx_buffer(ctrl_idx, priority, bufidx_ptr, buf_ptr, lenbyte_ptr);
}

Std_ReturnType Eth_Transmit(uint8 ctrl_idx, Eth_BufIdxType buf_idx, Eth_FrameType frame_type, boolean tx_confirmation,
				uint16 len_byte, const uint8 *phys_addr_ptr)
{
	return veth_transmit(ctrl_idx, buf_idx, frame_type, tx_confirmation, len_byte, phys_addr_ptr);
}

void eth0_handle(void)
{
	uint8 ctrl_idx = 0;
	uint8 channel_idx = 0;

	veth_tx_irq_hdlr(ctrl_idx, channel_idx);
	veth_rx_irq_hdlr(ctrl_idx, channel_idx);
}

void eth_enable_eth_mac(void)
{
}

#if defined(vETH_GLOBALTIMESUPPORT) && (vETH_GLOBALTIMESUPPORT == 1)
Std_ReturnType Eth_GetCurrentTime(uint8 ctrl_idx, Eth_TimeStampQualType *const time_qual_ptr,
	Eth_TimeStampType *const timestamp_ptr)
{
	Std_ReturnType ret;
	ret = veth_get_current_time(ctrl_idx, (eth_timestamp_qual_type *const)time_qual_ptr,
															(eth_timestamp_type *const)timestamp_ptr);
	return ret;
}
#endif
