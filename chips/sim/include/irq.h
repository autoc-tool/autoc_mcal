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

#ifndef __CHIPS_SIM_INCLUDE_IRQ_H
#define __CHIPS_SIM_INCLUDE_IRQ_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <nuttx/config.h>

#define TIMER_CORE0_IRQ_BASE  30
#define TIMER_CORE1_IRQ_BASE  30
#define TIMER_CORE2_IRQ_BASE  30
#define TIMER_CORE3_IRQ_BASE  30
#define TIMER_CORE4_IRQ_BASE  30
#define TIMER_CORE5_IRQ_BASE  30
#define INT_SRC_GPSR00  0
#define INT_SRC_GPSR01  0
#define INT_SRC_GPSR02  0
#define INT_SRC_GPSR03  0
#define INT_SRC_GPSR04  0
#define INT_SRC_GPSR05  0

#ifndef __ASSEMBLY__

#if defined(CONFIG_RT_FRAMEWORK ) && (CONFIG_RT_FRAMEWORK == 1)
void up_clear_irq(int irq);
void *up_get_contexthdl();
uint64_t up_get_timestamp(void);
uint32_t up_get_timestamp_freq(void);
void up_ipicall_attach(void *handler);
bool up_irq_is_enabled(int irq);
void up_init_irq(int irq, int irq_prio);
void up_set_contexthdl(void * const ctxhdl);
void up_switch_task_context(void *tcb);
int  up_trigger_ipicall(unsigned int cpu);
void up_trigger_ipicalls(unsigned int cpu_mask);
uint32_t up_timer_freq(void);
int up_timer_value(clock_t *ticks);
#ifdef CONFIG_RT_FRAMEWORK_SYSCALL
bool up_is_privilege_mode(void);
uint8_t up_get_mpu_set(void);
#endif /* CONFIG_RT_FRAMEWORK_SYSCALL */
#endif /* CONFIG_RT_FRAMEWORK */

#ifdef __cplusplus
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* __CHIPS_RT_FRAMEWORK_INCLUDE_IRQ_H */
