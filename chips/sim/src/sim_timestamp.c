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

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <nuttx/clock.h>
#include <nuttx/arch.h>
#include <sys/types.h>

#define SIM_TIMER_FREQ TICK_PER_SEC

uint32_t up_timer_freq(void)
{
    return SIM_TIMER_FREQ;
}

int up_timer_value(FAR clock_t *ticks)
{
    up_timer_gettick(ticks);
    return OK;
}
