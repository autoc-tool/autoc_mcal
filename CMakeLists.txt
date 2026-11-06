# ##############################################################################
# components/autoc_mcal/CMakeLists.txt
#
# AutoC PC MCAL components (sibling of communication). Layout mirrors
# vcos_components_communication: nuttx_add_subdirectory + menu Kconfig.
# ##############################################################################
nuttx_add_subdirectory()
nuttx_generate_kconfig(MENUDESC "autoc_mcal")
