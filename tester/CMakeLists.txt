#* Author: Wady <iswady@163.com>
#* Date:   Wed May 15 12:00:00 2019

set(TESTER_SOURCE_FILES_C
    tester.c
)

set(TESTER_LIBS
    agiletimer
)

set(TESTER_INCLUDE_DIRS
)

if(TOOLBOX_FOUND)
    list(APPEND TESTER_LIBS ${TOOLBOX_LIBRARIES})
    list(APPEND TESTER_INCLUDE_DIRS ${TOOLBOX_INCLUDE_DIRS})
endif()

utils_apply_compile_flags(TESTER_SOURCE_FILES_C STRICT_OPTIONS_CPP STRICT_OPTIONS_C)

add_executable(tester_agiletimer ${TESTER_SOURCE_FILES_C})
target_link_libraries(tester_agiletimer ${TESTER_LIBS})
target_include_directories(tester_agiletimer PRIVATE ${TESTER_INCLUDE_DIRS})

install(TARGETS tester_agiletimer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
