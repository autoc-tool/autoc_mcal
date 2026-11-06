/****************************************************************************
 * boards/sim/src/sim_msvc_compat.c — MSVC host stubs for sim IRQ/BMP helpers
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#ifdef _MSC_VER

uint32_t up_cpu_index(void)
{
  return 0u;
}

void up_trigger_irq(int irq, uint32_t cpuset)
{
  (void)irq;
  (void)cpuset;
}

void up_clear_irq(int irq)
{
  (void)irq;
}

bool up_irq_is_enabled(int irq)
{
  (void)irq;
  return false;
}

void up_irq_disable(int irq)
{
  (void)irq;
}

void up_irq_enable(int irq)
{
  (void)irq;
}

void up_init_irq(int irq, int irq_prio)
{
  (void)irq;
  (void)irq_prio;
}

void up_affinity_irq(int irq, uint32_t cpuset)
{
  (void)irq;
  (void)cpuset;
}

void sim_idle_wait(void)
{
}

void bmp_start(void)
{
}

#endif /* _MSC_VER */