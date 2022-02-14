add_subdirectory(roottools)

# option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

option(BUILD_DIFFANA "Build DiffAna module" OFF)
if(BUILD_DIFFANA)
  add_subdirectory(diffana)
endif()
