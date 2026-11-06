# ##############################################################################
# vendor/autoc boards/sim Toolchain — GCC flags gated for MSVC clean configure
# ##############################################################################

set(PLATFORM_FLAGS)

if(MSVC)
  # MSVC rejects GCC-only warning switches such as -Wno-undef.
  add_compile_options(/D__TRICORE__)
  list(APPEND PLATFORM_FLAGS
       -I${CMAKE_CURRENT_LIST_DIR}/../../../chips/sim/global_inc)
  # Prefer /I form as well for cl
  include_directories("${CMAKE_CURRENT_LIST_DIR}/../../../chips/sim/global_inc")
else()
  add_compile_options(
       -Wno-undef
       -D__TRICORE__
       -Wno-error=pedantic
       -Wno-error=array-bounds
       )
  list(APPEND PLATFORM_FLAGS
       -I${CMAKE_CURRENT_LIST_DIR}/../../../chips/sim/global_inc)
  add_compile_options(${PLATFORM_FLAGS})
endif()