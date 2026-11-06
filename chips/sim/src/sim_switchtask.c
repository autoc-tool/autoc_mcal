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

#include <nuttx/config.h>
#include <nuttx/sched.h>
#include <arch/irq.h>
#include <stddef.h>

/*  \brief      switch task context in switch context or irq return stage
 *  \param[in]  xcp  task context in switch context
 *  \return
 */

void up_switch_task_context(void *tcb)
{
	struct tcb_s *next_tcb = (struct tcb_s *)tcb;

	if (next_tcb != NULL) {
		up_set_contexthdl(next_tcb->contexthdl);
	}
}
