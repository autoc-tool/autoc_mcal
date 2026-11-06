# board `pc` 鈥?native PC host (NuttX arch/sim)

Native PC path for EcuBus MCAL. **Not** the official `-sim 1` transform.

- `configs/vcos/defconfig` already sets `CONFIG_ARCH=sim` and reuses
  `vendor/semidrive/{boards,chips}/sim` as CUSTOM board/chip (same as the
  hand-built minimal_system5 path). No `vendor/ecubus/chips/sim` needed.
- Studio / plugins stay `PC_Common` + `vs20xx`.
- Host compile via `vcos_build` uses **`-compiler gcc`** (host gcc for NuttX sim).

See `vcos/build/PC_BUILD.md`.

## MSVC clean build

Use `boards/pc/scripts/clean_build_msvc.bat` (clears output, cmake via vcos_build, sanitize ninja, ninja). Board/chip live under `vendor/autoc/boards/sim` and `vendor/autoc/chips/sim`.
