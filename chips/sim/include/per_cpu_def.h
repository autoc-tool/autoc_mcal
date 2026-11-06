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

#ifndef __CHIPS_SIM_INCLUDE_PERF_CPU_DEF_H
#define __CHIPS_SIM_INCLUDE_PERF_CPU_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif

#define __percpu_data
#define __percpu_bss
#define this_cpu_var(s) (*(__typeof__(s) *)((unsigned long)&(s) + g_cpu_data_size * up_cpu_index()))

#ifdef __cplusplus
}
#endif

#endif /* __CHIPS_SIM_INCLUDE_PERF_CPU_DEF_H */
