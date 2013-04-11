cmake_minimum_required(VERSION 2.6)
project(RootTools)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Modules" ${CMAKE_MODULE_PATH})
find_package(ROOT)

# # set(CMAKE_C_COMPILER /usr/bin/x86_64-pc-linux-gnu-gcc CACHE FILEPATH "C compiler" FORCE)
# set(CMAKE_C_COMPILE_OBJECT "<CMAKE_C_COMPILER> <DEFINES>  -DQT_NO_DEBUG -DNDEBUG <FLAGS> -o <OBJECT> -c <SOURCE>" CACHE STRING "C compile command" FORCE)
# # set(CMAKE_CXX_COMPILER /usr/bin/x86_64-pc-linux-gnu-g++ CACHE FILEPATH "C++ compiler" FORCE)
# set(CMAKE_CXX_COMPILE_OBJECT "<CMAKE_CXX_COMPILER> <DEFINES>  -DQT_NO_DEBUG -DNDEBUG <FLAGS> -o <OBJECT> -c <SOURCE>" CACHE STRING "C++ compile command" FORCE)

# The version number.
set (library_VERSION_MAJOR 0)
set (library_VERSION_MINOR 1)
set (library_VERSION_PATCH 1)

set (VERSION ${library_VERSION_MAJOR}.${library_VERSION_MINOR}.${library_VERSION_PATCH})

set(PRONAME RootTools)
##### find programs
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${root_LIBRARIES_DIR})

set(library_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/RootTools.cxx langaus.C) # ${CMAKE_CURRENT_SOURCE_DIR}/
set(library_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/RootTools.h) #

set(library_DICT ${CMAKE_CURRENT_SOURCE_DIR}/${PRONAME}Dict.cxx)
set(library_LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)

ROOT_GENERATE_DICTIONARY(
	"${library_HDRS}" "${library_LINKDEF}"
	"${library_DICT}" "${INCLUE_DIRECTORIES}"
)

add_library(${PRONAME} SHARED ${library_SRCS} ${library_DICT})

string(REGEX REPLACE "/" "." ROOT_VERSION ${ROOTVERSION})

target_link_libraries(${PRONAME} ${ROOT_LIBRARIES})
set_target_properties(${PRONAME} PROPERTIES
	SOVERSION ${ROOT_VERSION}
	PUBLIC_HEADER RootTools.h)

#set_target_properties (tsetup PROPERTIES LINK_FLAGS ${root_LDFLAGS})

install(TARGETS ${PRONAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include)

# messages
MESSAGE(STATUS "<<< Configuration >>>
Project:	${CMAKE_PROJECT_NAME}

Build type	${CMAKE_BUILD_TYPE}
Install path	${CMAKE_INSTALL_PREFIX}

Compiler:
C		${CMAKE_C_COMPILER}
C++		${CMAKE_CXX_COMPILER}

Linker:
Ld		${CMAKE_LINKER}

Compiler flags:
C		${CMAKE_C_FLAGS}
C++		${CMAKE_CXX_FLAGS}

Linker flags:
Executable      ${CMAKE_EXE_LINKER_FLAGS}
Module          ${CMAKE_MODULE_LINKER_FLAGS}
Shared          ${CMAKE_SHARED_LINKER_FLAGS}\n")
