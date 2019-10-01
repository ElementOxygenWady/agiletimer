#* Author: Wady <iswady@163.com>
#* Date:   Wed May 15 12:00:00 2019

cmake_minimum_required(VERSION 3.5.0)
project(Agiletimer VERSION 1.0.0 LANGUAGES C CXX)

if(EXTRA_CMAKE)
    include("${EXTRA_CMAKE}")
    message(STATUS "Including extra cmake from ${EXTRA_CMAKE}")
endif()

# Used to generate config.h.
string(TOUPPER ${PROJECT_NAME} CONFIG_PROJECT_NAME)

# Basic Information.
MESSAGE(STATUS "CMake version: " ${CMAKE_VERSION})
MESSAGE(STATUS "CMake system name: " ${CMAKE_SYSTEM_NAME})

# Specifying a search path for CMake modules to be loaded by the the include() or find_package() commands before checking the default modules that come with CMake.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Macro to provide an option dependent on other options.
include(CMakeDependentOption)

# Provides a macro to check if a list of one or more header files can be included together in C.
include(CheckIncludeFiles)

# Check if a C function can be linked.
include(CheckFunctionExists)

# Check if a symbol exists as a function, variable, or macro.
include(CheckSymbolExists)

# Provides install directory variables as defined by the GNU Coding Standards.
# GNU Coding Standards Reference 'https://www.gnu.org/prep/standards/html_node/Directory-Variables.html'.
include(GNUInstallDirs)

# Check whether the CXX compiler supports a given flag.
include(CheckCXXCompilerFlag)

if(NOT CMAKE_INSTALL_RPATH AND CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR})
    message(STATUS "Setting install rpath to ${CMAKE_INSTALL_RPATH}")
endif()

include(cmake/CMakeUtils.cmake)

# Provides an option that the user can optionally select.
option(ENABLE_SHARED "Build shared library." ON)
option(ENABLE_STATIC "Build static library." OFF)
option(ENABLE_STRICT "Build with strict compile options." ON)
option(ENABLE_TESTS "Enable compilation of test programs." OFF)
option(ENABLE_DEBUG "Turn on or off debug level logs." ON)

# Strict options, reference from Belledonne Communications, Grenoble France.
set(STRICT_OPTIONS_CPP )
set(STRICT_OPTIONS_C )
set(STRICT_OPTIONS_CXX )
list(APPEND STRICT_OPTIONS_CPP
    "-Wall"
    "-Wcast-align"
    "-Wconversion"
    "-Werror=return-type"
    "-Wfloat-equal"
    "-Winit-self"
    "-Wno-error=deprecated-declarations"
    "-Wpointer-arith"
    "-Wuninitialized"
    "-Wunused"
)
list(APPEND STRICT_OPTIONS_CXX
    "-Wnon-virtual-dtor"
    "-Woverloaded-virtual"
)
CHECK_CXX_COMPILER_FLAG("-Wsuggest-override" SUGGEST_OVERRIDE)
if(SUGGEST_OVERRIDE)
    list(APPEND STRICT_OPTIONS_CXX "-Wsuggest-override" "-Wno-error=suggest-override")
endif()
list(APPEND STRICT_OPTIONS_C "-Wstrict-prototypes" "-Werror=strict-prototypes")
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    list(APPEND STRICT_OPTIONS_C "-fno-inline-small-functions")
endif()
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    list(APPEND STRICT_OPTIONS_CPP "-Qunused-arguments" "-Wno-array-bounds")
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND STRICT_OPTIONS_CXX "-x c++")
endif()
if(ENABLE_STRICT)
    list(APPEND STRICT_OPTIONS_C "-Werror" "-Wextra" "-Wno-unused-parameter" "-Wno-missing-field-initializers" "-fno-strict-aliasing")
    list(APPEND STRICT_OPTIONS_CPP "-Werror" "-Wextra" "-Wno-unused-parameter" "-Wno-missing-field-initializers" "-fno-strict-aliasing")
endif()
if(STRICT_OPTIONS_CPP)
    list(REMOVE_DUPLICATES STRICT_OPTIONS_CPP)
endif()
if(STRICT_OPTIONS_C)
    list(REMOVE_DUPLICATES STRICT_OPTIONS_C)
endif()

if(ENABLE_DEBUG)
    add_definitions(-DENABLE_DEBUG)
endif()

find_package(Toolbox)
if(TOOLBOX_FOUND)
    add_definitions(-DTOOLBOX_FOUND)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
# Source files can have properties that affect how they are built.
#  GENERATED - Is this source file generated as part of the build process.
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/config.h PROPERTIES GENERATED ON)
add_definitions(-DHAVE_CONFIG_H)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    include
)

add_subdirectory(include)
add_subdirectory(src)

if(ENABLE_TESTS)
    add_subdirectory(tester)
endif()

configure_file(cmake/${PROJECT_NAME}Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY)

set(CONFIG_PACKAGE_LOCATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake")
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION ${CONFIG_PACKAGE_LOCATION}
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION ${CONFIG_PACKAGE_LOCATION}
)
