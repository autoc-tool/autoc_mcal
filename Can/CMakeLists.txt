# ##############################################################################
# components/autoc_mcal/Can/CMakeLists.txt
# ##############################################################################

if(CONFIG_MODULE_CAN)
    nuttx_add_library(pc_can_driver STATIC)

    set(CSRCS)
    file(GLOB CAN_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    )
    list(APPEND CSRCS
        ${CAN_SOURCES}
        ${GENDATA_DIR}/Can/Can_PBcfg.c
    )

    set(INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/host
        ${NUTTX_APPS_DIR}/components/include
        ${NUTTX_APPS_DIR}/components/include/autosar
        ${NUTTX_APPS_DIR}/components/rt_framework/inc
        ${NUTTX_APPS_DIR}/components/communication/net_dev/canif/inc
        ${GENDATA_DIR}/Can
        ${GENDATA_DIR}/CanIf
        ${GENDATA_DIR}/Memmap
    )

    target_sources(pc_can_driver PRIVATE ${CSRCS})
    target_include_directories(pc_can_driver PRIVATE ${INCLUDE_DIRS})
    target_include_directories(pc_can_driver PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

    if(WIN32)
        target_link_libraries(pc_can_driver PRIVATE ws2_32)
    endif()

    # Host socket wrappers (not renamed by nuttx.rel symbol remap).
    if(NOT WIN32 AND TARGET nuttx)
        target_sources(nuttx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/host/can_rpc_nx_host.c)
        target_include_directories(nuttx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/host)
    endif()
endif()
