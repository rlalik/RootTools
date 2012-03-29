cmake_minimum_required(VERSION 2.8)
project(RootTools)

# set(CMAKE_C_COMPILER /usr/bin/x86_64-pc-linux-gnu-gcc CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_C_COMPILE_OBJECT "<CMAKE_C_COMPILER> <DEFINES>  -DQT_NO_DEBUG -DNDEBUG <FLAGS> -o <OBJECT> -c <SOURCE>" CACHE STRING "C compile command" FORCE)
# set(CMAKE_CXX_COMPILER /usr/bin/x86_64-pc-linux-gnu-g++ CACHE FILEPATH "C++ compiler" FORCE)
set(CMAKE_CXX_COMPILE_OBJECT "<CMAKE_CXX_COMPILER> <DEFINES>  -DQT_NO_DEBUG -DNDEBUG <FLAGS> -o <OBJECT> -c <SOURCE>" CACHE STRING "C++ compile command" FORCE)

# set(CMAKE_C_FLAGS "-march=core2 -O2 -pipe") # -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common
# set(CMAKE_CXX_FLAGS "-march=core2 -O2 -pipe") # -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -DQT_NO_EXCEPTIONS -fno-check-new -fno-common -Woverloaded-virtual -fno-threadsafe-statics -fvisibility=hidden -fvisibility-inlines-hidden
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags -Wl,-O1 -Wl,--as-needed")
# set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--enable-new-dtags -Wl,--fatal-warnings -lc -Wl,-O1 -Wl,--as-needed")
# set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags -Wl,--fatal-warnings -lc -Wl,-O1 -Wl,--as-needed")
# set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--enable-new-dtags -Wl,--fatal-warnings -Wl,--no-undefined -lc -Wl,-O1 -Wl,--as-needed")
# set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags -Wl,--fatal-warnings -Wl,--no-undefined -lc -Wl,-O1 -Wl,--as-needed")

# The version number.
set (library_VERSION_MAJOR 0)
set (library_VERSION_MINOR 1)
set (library_VERSION_PATCH 1)

set (VERSION ${library_VERSION_MAJOR}.${library_VERSION_MINOR}.${library_VERSION_PATCH})

set(PRONAME RootTools)
##### find programs
### ROOT
find_program(root_config root-config)

if (NOT root_config)
	message(FATAL_ERROR "Unable to find 'root-config' program")
endif (NOT root_config)

find_program(root_libmap rlibmap)

if (NOT root_libmap)
	message(FATAL_ERROR "Unable to find 'rootlibmap' program")
endif (NOT root_libmap)

find_program(root_cint rootcint)

if (NOT root_cint)
	message(FATAL_ERROR "Unable to find 'rootcint' program")
endif (NOT root_cint)

##### fetch variables

execute_process (COMMAND ${root_config} --incdir OUTPUT_VARIABLE root_INCLUDES ERROR_VARIABLE root_INCLUDES_ERROR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND ${root_config} --libdir OUTPUT_VARIABLE root_LIBS ERROR_VARIABLE root_LIBS_ERROR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND ${root_config} --libs OUTPUT_VARIABLE root_LDFLAGS ERROR_VARIABLE root_LDFLAGS_ERROR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND ${root_config} --version OUTPUT_VARIABLE root_VERSION ERROR_VARIABLE root_VERSION_ERROR OUTPUT_STRIP_TRAILING_WHITESPACE)

string(REGEX REPLACE "/" "." root_VERSION ${root_VERSION})

message(STATUS "SONAME=${root_VERSION}")

include_directories(${root_INCLUDES})
link_directories(${root_LIBS})

set (root_cint_executable ${root_cint})
set (root_cint_cppflags -I${root_INCLUDES} -I${go4_INCLUDES}) # -I${roclib_INCLUDES}
set (rlibmap_executable ${root_libmap})
set (rlibmap_cppflags -I${root_INCLUDES} -I${go4_INCLUDES}) # -I${roclib_INCLUDES}

set(library_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/RootTools.cxx) # ${CMAKE_CURRENT_SOURCE_DIR}/
set(library_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/RootTools.h) #

set(libraryDict_TRG ${PRONAME}Dict.cxx ${PRONAME}Dict.h)
set(libraryDict_DEPS LinkDef.h ${library_HDRS})

set(libraryRM_TRG lib${PRONAME}.rootmap)
set(libraryRM_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)

add_custom_command (OUTPUT ${libraryDict_TRG}
		    COMMAND ${root_cint_executable} -f ${PRONAME}Dict.cxx -c -p ${root_cint_cppflags} ${library_HDRS} ${libraryRM_DEPS}
		    DEPENDS ${libraryDict_DEPS} ${library_SRCS})

add_library(${PRONAME} SHARED ${library_SRCS} ${libraryDict_TRG})

target_link_libraries(${PRONAME} ${root_LDFLAGS})
set_target_properties(${PRONAME} PROPERTIES
	SOVERSION ${root_VERSION}
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
