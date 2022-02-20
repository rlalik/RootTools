cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(
  RootTools
  VERSION 0.4.0
  LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(GNUInstallDirs)

# ==============================================================================
# fetch external content
include(FetchContent)
# ------------------------------------------------------------------------------
# cmake_tools
FetchContent_Declare(
  cmake_tools
  GIT_REPOSITORY https://github.com/rlalik/cmake_tools
  GIT_TAG master)

FetchContent_MakeAvailable(cmake_tools)
list(APPEND CMAKE_MODULE_PATH ${cmake_tools_SOURCE_DIR})
include(check_3rd_party_tool)
# ------------------------------------------------------------------------------
# cmake - scripts
check_3rd_party_tool(
  cmake-scripts 9999 https://github.com/StableCoder/cmake-scripts TAG
  774a07ece3945b4f72e3ddc11646ab7cc9a349e2 AUTO)

list(APPEND CMAKE_MODULE_PATH ${cmake-scripts_SOURCE_DIR})
include(c++-standards)
include(code-coverage)
include(sanitizers)
include(doxygen)
include(formatting)
include(tools)
# ------------------------------------------------------------------------------
# jsoncpp
set(BUILD_SHARED_LIBS OFF)
check_3rd_party_tool(
  jsoncpp 1.9.5 https://github.com/open-source-parsers/jsoncpp TAG 1.9.5 AUTO)
set(JSONCPP_WITH_TESTS OFF)
set_property(TARGET jsoncpp_static PROPERTY POSITION_INDEPENDENT_CODE ON)
# ------------------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# find ROOT
set(ROOT_DIR
    ""
    CACHE STRING "Path to ROOT installation")
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS} ${ROOT_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS} ${ROOT_DIR}/cmake)

find_package(ROOT REQUIRED COMPONENTS Core ASImage CONFIG)

if(ROOT_VERSION VERSION_LESS 6)
  message(STATUS "Add support for ROOT legacy version ${ROOT_VERSION}")
  include(${ROOT_USE_FILE})
  set(ROOT_LIBS_OF_INTREST Core ASImage)
else()
  set(ROOT_LIBS_OF_INTREST ROOT::Core ROOT::ASImage)
endif()

option(RootTools_SHARED_LIBS "Build RootTools shared libs" ON)

add_compile_options(
  -Wall
  -Wextra
  #-Wshadow require higher than c++11
  -Wnon-virtual-dtor
  -Wold-style-cast
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  #-Wmisleading-indentation
  #-Wduplicated-cond
  #-Wduplicated-branches
  -Wlogical-op
  #-Wnull-dereference
  -Wuseless-cast
  -Wdouble-promotion
  -Wformat=2)

add_subdirectory(modules)

# Export the package for use from the build - tree (this registers the build -
# tree with a global CMake - registry)
export(PACKAGE ${PROJECT_NAME})

# Install the export set for use with the install - tree

install(
  EXPORT RT_Targets
  FILE RootToolsTargets.cmake
  NAMESPACE RT::
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
  COMPONENT dev)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  Config.cmake.in ${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_BINDIR)

write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_CMAKEDIR})

option(ENABLE_TESTING "Build tests" ON)
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

# messages
message(
  STATUS
    "<<< Configuration >>>
Project:        ${PROJECT_NAME}

Architecture:   ${CMAKE_LIBRARY_ARCHITECTURE}
Build type      ${CMAKE_BUILD_TYPE}
Install path    ${CMAKE_INSTALL_PREFIX}

Compiler:
C               ${CMAKE_C_COMPILER}
C++             ${CMAKE_CXX_COMPILER}

Linker:
Ld              ${CMAKE_LINKER}

Compiler flags:
C               ${CMAKE_C_FLAGS}
C++             ${CMAKE_CXX_FLAGS}

Linker flags:
Executable      ${CMAKE_EXE_LINKER_FLAGS}
Module          ${CMAKE_MODULE_LINKER_FLAGS}
Shared          ${CMAKE_SHARED_LINKER_FLAGS}\n
")
