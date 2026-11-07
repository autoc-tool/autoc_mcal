# PC MCAL Studio system plugin pack

Point VCOS Studio **system_path** at this directory. Do not import `components/Can` as a user plugin.

## New project

1. Studio → New Project.
2. **Configurator PlugIn Path / system_path**: `vcos/components/autoc_mcal/configurator_plug_in`.
3. **Target**: `PC_Common` (compiler `vs20xx`).
4. Enable **Can**, **CanIf**, and the other BSW modules you need. CanIf depends on Can.

Studio loads **Can** from `vcos/components/autoc_mcal/Can` (`plug_in_path` `../Can`, module name stays `Can`, so CanIf refs work). Os, CanIf, PduR, Com, and the rest come from `vcos/components`. `PC_Common` publishes Os `max_interrupt_priority` 255. `PC_Common` does not register Infineon `Can_17_McmCan`, so there is no name clash.

## Layout

```text
components/autoc_mcal/configurator_plug_in
├── components_plug_in_cfg.json   # PC_Common + module list
├── components_version.py
├── compatibility.json
├── linkage_cfg.json
├── platform_cfg/PC/PC_Common/    # empty MCAL overlay; Can stays ours
└── system_secondary/             # Studio menus / BswM common data
```

Relative `plug_in_path` values are joined onto this directory. The pack lives at `vcos/components/autoc_mcal/configurator_plug_in`, so BSW paths use `../../../components` and Can uses `../Can`. Studio rejects a path that contains `/components/` more than once (`../components/Can` does, and pack load crashes).
