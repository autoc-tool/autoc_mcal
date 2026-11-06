# autoc_mcal

AutoC PC MCAL as a **VCOS components** package (sibling of `communication`), not a vendor/chip tree.

Layout mirrors [vcos_components_communication](https://gitee.com/haloos/vcos_components_communication):

- Root `CMakeLists.txt` / `Kconfig` with `nuttx_add_subdirectory()` + `nuttx_generate_kconfig`
- Module dirs (`Can/`) with `src/`, `inc/`, `config_plug_in/`, `CMakeLists.txt`, `module_cfg.json`

Manifest path: `vcos/components/autoc_mcal` (repo `autoc-tool/autoc_mcal`).

Board/chip host glue for PC (`boards/`, `chips/`, `configurator_plug_in/`) is folded here so one repo owns PC MCAL; only `Can/` is a built module subdirectory.
