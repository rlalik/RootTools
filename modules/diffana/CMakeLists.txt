if(DEFINED RootTools_SHARED_LIBS)
  set(BUILD_SHARED_LIBS "${RootTools_SHARED_LIBS}")
endif()

find_package(SmartFactory)
find_package(FitterFactory)

set(PH DifferentialContext.h DifferentialFactory.h DistributionContext.h
       DistributionFactory.h ExtraDimensionMapper.h)

add_library(
  DiffAna
  DifferentialContext.cxx DifferentialFactory.cxx DistributionContext.cxx
  DistributionFactory.cxx ExtraDimensionMapper.cxx ${PH})
add_library(RT::DiffAna ALIAS DiffAna)

set_target_properties(DiffAna PROPERTIES PUBLIC_HEADER "${PH}")

target_include_directories(
  DiffAna PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                 "$<INSTALL_INTERFACE:include/RootTools>")

target_link_libraries(
  DiffAna
  PUBLIC RT::SmartFactory RT::FitterFactory
  PRIVATE RootTools jsoncpp_static)

install(
  TARGETS DiffAna
  EXPORT RT_Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RootTools)
