/****************************************************************************
 * boards/sim/src/sim_fork_msvc_stub.c — stub fork helpers for MSVC host
 ****************************************************************************/
#ifdef _MSC_VER
int sim_fork_stub(void)
{
  return -1;
}
#endif