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

 #include <stddef.h>
#include <nuttx/config.h>
#include <nuttx/board.h>
#include <nuttx/arch.h>
#include "arch/irq.h"
#include "arch/chip/irq.h"
#include "rtfw_counter.h"

typedef void (*ipicall_func_t)(void);

extern void platform_init(void);
extern void StartOS(void);

static void *archctxhandle[CONFIG_BMP_NCPUS];
static void *ipicall_callback[CONFIG_BMP_NCPUS];
volatile uint32_t gactive_nsh_core;

/* ipicall irq vectornumber table */
static uint32_t per_cpu_ipicall_irq[] = {
	INT_SRC_GPSR00,
	INT_SRC_GPSR01,
	INT_SRC_GPSR02,
	INT_SRC_GPSR03,
	INT_SRC_GPSR04,
	INT_SRC_GPSR05,
};

/* timer irq vectornumber table */
static uint32_t per_cpu_timer_irq[] = {
	TIMER_CORE0_IRQ_BASE,
	TIMER_CORE1_IRQ_BASE,
	TIMER_CORE2_IRQ_BASE,
	TIMER_CORE3_IRQ_BASE,
	TIMER_CORE4_IRQ_BASE,
	TIMER_CORE5_IRQ_BASE,
};

void up_set_contexthdl(void *const ctxhdl)
{
	irqstate_t irq_state = up_irq_save();

	archctxhandle[up_cpu_index()] = ctxhdl;
	up_irq_restore(irq_state);
}

void *up_get_contexthdl(void)
{
	return archctxhandle[up_cpu_index()];
}

void up_uart_rx_handler(void)
{
	/* 打桩，sim使用周期轮询查询uart状态实现输入输出 */
}

void up_ipicall_isr_handler(void *arg)
{
	(void)arg;
	int cpu = up_cpu_index();

	if (ipicall_callback[cpu] != NULL)
		((ipicall_func_t)(ipicall_callback[cpu]))();
}

void up_ipicall_attach(void *handler)
{
	ipicall_callback[up_cpu_index()] = handler;
}

int up_trigger_ipicall(unsigned int cpu)
{
	if (cpu < CONFIG_BMP_NCPUS) {
		up_trigger_irq(per_cpu_ipicall_irq[cpu], 1 << cpu);
		return 0;
	}

	return -1;
}

void up_trigger_ipicalls(unsigned int cpu_mask)
{
	int cpu = CONFIG_BMP_NCPUS - 1;

	for (; cpu >= 0; cpu--) {
		if (cpu_mask & (0x01 << cpu)) {
			up_trigger_irq(per_cpu_ipicall_irq[cpu], 1 << cpu);
		}
	}
}

void rt_fwk_tick_handler(void *arg)
{
	nxsched_process_timer();
	os_counter_check();
}

uint32_t up_get_systick_irqbase(void)
{
	return per_cpu_timer_irq[up_cpu_index()];
}

static void up_ostimer_start(void)
{
	up_enable_irq(up_get_systick_irqbase());
}

#ifdef CONFIG_BOARD_EARLY_INITIALIZE
void board_early_initialize(void)
{
#if defined(CONFIG_RT_FRAMEWORK) && (CONFIG_RT_FRAMEWORK == 1)
	platform_init();
	StartOS();
	up_ostimer_start();
#endif
}
#endif /* CONFIG_BOARD_EARLY_INITIALIZE */
