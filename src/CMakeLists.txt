#* Author: Wady <iswady@163.com>
#* Date:   Wed May 15 12:00:00 2019

set(AGILETIMER_SOURCE_FILES_C
    agiletimer.c
)

set(AGILETIMER_LIBS
)

set(AGILETIMER_INCLUDE_DIRS
)

if(TOOLBOX_FOUND)
    list(APPEND AGILETIMER_LIBS ${TOOLBOX_LIBRARIES})
    list(APPEND AGILETIMER_INCLUDE_DIRS ${TOOLBOX_INCLUDE_DIRS})
endif()

if(NOT ANDROID)
    find_package(Threads)
    if(CMAKE_USE_PTHREADS_INIT)
        add_definitions(-DTHREAD_FOUND)
    endif()
else()
    add_definitions(-DTHREAD_FOUND)
endif()

utils_apply_compile_flags(AGILETIMER_SOURCE_FILES_C STRICT_OPTIONS_CPP STRICT_OPTIONS_C)

if(ENABLE_STATIC)
    add_library(agiletimer-static STATIC ${AGILETIMER_SOURCE_FILES_C})
    target_link_libraries(agiletimer-static INTERFACE ${AGILETIMER_LIBS})
if(CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(agiletimer-static INTERFACE ${CMAKE_THREAD_LIBS_INIT})
endif()
    target_include_directories(agiletimer-static PUBLIC ${AGILETIMER_INCLUDE_DIRS})
    install(TARGETS agiletimer-static EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
endif()
if(ENABLE_SHARED)
    add_library(agiletimer SHARED ${AGILETIMER_SOURCE_FILES_C})
    target_link_libraries(agiletimer PRIVATE ${AGILETIMER_LIBS})
if(CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(agiletimer PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()
    target_include_directories(agiletimer PUBLIC ${AGILETIMER_INCLUDE_DIRS})
    install(TARGETS agiletimer EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
endif()
