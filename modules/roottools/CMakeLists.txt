if(DEFINED RootTools_SHARED_LIBS)
  set(BUILD_SHARED_LIBS "${RootTools_SHARED_LIBS}")
endif()

set(PH RootTools.h ProgressBar.h)

add_library(RootTools RootTools.cxx ProgressBar.cxx langaus.C ${PH})
add_library(RT::RootTools ALIAS RootTools)

set_target_properties(RootTools PROPERTIES PUBLIC_HEADER "${PH}")

target_include_directories(
  RootTools PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                   "$<INSTALL_INTERFACE:include>/RootTools")

target_link_libraries(RootTools PUBLIC ${ROOT_LIBS_OF_INTREST})

install(
  TARGETS RootTools
  EXPORT RT_Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RootTools)
