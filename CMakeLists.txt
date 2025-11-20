cmake_minimum_required(VERSION 3.8)
project(test-ogre)

find_package(Coin3D REQUIRED)
find_package(OGRE REQUIRED)


# Ogre plugins and resources
include(cmake/VISPUtils.cmake)
include(cmake/OgreTools.cmake)

set(USE_OGRE ON)
set(VISP_HAVE_OGRE ON)
set(USE_COIN3D ON)
set(VISP_HAVE_COIN3D ON)
set(USE_PANDA3D ON)
set(VISP_HAVE_PANDA3D ON)
set(Panda3D_DIR $ENV{CONDA_PREFIX})

if(USE_OGRE)
  vp_set_ogre_media()
  set(VISP_HAVE_OGRE_VERSION "(${OGRE_VERSION_MAJOR}<<16 | ${OGRE_VERSION_MINOR}<<8 | ${OGRE_VERSION_PATCH})") # for vpConfig.h
endif()

if(OGRE_FOUND)
  vp_set_ogre_advanced_var()
endif()

configure_file(vpConfig.h.in vpConfig.h)

add_executable(${PROJECT_NAME} src/main.cpp)
# add_executable(${PROJECT_NAME} src/vpAROgre.cpp src/main.cpp)


if(USE_OGRE)
  # With Ubuntu 20.04, OGRE_LIBRARIES contains Boost::thread that cannot
  # be exported as is on pain of producing a build issue when ViSP is used
  # as a 3rd party. That's why here we get its imported location.

  message("OGRE_VERSION_MAJOR = ${OGRE_VERSION_MAJOR}")
  message("OGRE_VERSION_MINOR = ${OGRE_VERSION_MINOR}")
  message("OGRE_VERSION_PATCH = ${OGRE_VERSION_PATCH}")
  if(NOT OGRE_VERSION)
    set(OGRE_VERSION "${OGRE_VERSION_MAJOR}.${OGRE_VERSION_MINOR}.${OGRE_VERSION_PATCH}")
  endif()

  if(${OGRE_VERSION_MAJOR} EQUAL 1 AND (${OGRE_VERSION_MINOR} EQUAL 10 OR ${OGRE_VERSION_MINOR} EQUAL 11))
    message("FOOOOOOO 1")
    find_package(Boost REQUIRED COMPONENTS thread)
    link_directories(${OGRE_LIBRARY_DIRS})
    if(NOT(TARGET OgreMain))
      find_library(OgreMainSEARCH NAMES OgreMain PATHS ${OGRE_LIBRARY_DIRS})
      add_library(OgreMain SHARED IMPORTED)
      set_property(TARGET OgreMain PROPERTY IMPORTED_LOCATION ${OgreMainSEARCH})
    endif()
  endif()

  if(NOT(${OGRE_VERSION_MAJOR} EQUAL 1 AND ${OGRE_VERSION_MINOR} LESS_EQUAL 9))
    message("FOOOOOOO 2")
    foreach(component ${OGRE_COMPONENTS})
      set(ogre_target "OGRE_${component}_LIBRARIES")
      foreach(ogre_libray ${${ogre_target}})
        if(TARGET ${ogre_libray})
          get_target_property(inc_dirs ${ogre_libray} INTERFACE_INCLUDE_DIRECTORIES)
          if(inc_dirs)
          # list(APPEND opt_system_incs ${inc_dirs})
          list(APPEND opt_incs ${inc_dirs})
          endif()
          ## The following lines are not needed for Ogre 1.9 and 1.12, but might be useful for compiled versions of Ogre
          # get_target_property(interface_libs ${ogre_libray} INTERFACE_LINK_LIBRARIES)
          # if(interface_libs)
          #   list(APPEND opt_libs ${interface_libs})
          # endif()
        endif()
        if(${OGRE_VERSION_MAJOR} EQUAL 1 AND (${OGRE_VERSION_MINOR} EQUAL 10 OR ${OGRE_VERSION_MINOR} EQUAL 11))
          unset(MATCHED)
          string(REGEX MATCH ".*O[gG][rR][eE].*" MATCHED ${ogre_libray})
          if (NOT MATCHED)
              continue()
          endif()
          if(NOT(TARGET ${ogre_libray}))
            find_library(${ogre_libray}SEARCH NAMES ${ogre_libray} PATHS ${OGRE_LIBRARY_DIRS})
            add_library(${ogre_libray} SHARED IMPORTED)
            set_property(TARGET ${ogre_libray} PROPERTY IMPORTED_LOCATION ${${ogre_libray}SEARCH})
          endif()
        endif()
      endforeach()
    endforeach()
  endif()

  vp_filter_libraries_with_imported_location(OGRE_LIBRARIES)

  mark_as_advanced(OGRE_SAMPLES_INCLUDEPATH)
  #message("OGRE_SAMPLES_INCLUDEPATH: ${OGRE_SAMPLES_INCLUDEPATH}")
  if(OGRE_SAMPLES_INCLUDEPATH)
    # list(APPEND opt_system_incs ${OGRE_SAMPLES_INCLUDEPATH})
    list(APPEND opt_incs ${OGRE_SAMPLES_INCLUDEPATH})
  endif()

  # hack to fix possible presence of NOTFOUND in OGRE_INCLUDE_DIRS
  foreach(inc_ ${OGRE_INCLUDE_DIRS})
    if(NOT ${inc_} MATCHES "NOTFOUND")
    #   list(APPEND opt_system_incs ${inc_})
      list(APPEND opt_incs ${inc_})
    endif()
  endforeach()
#   if(WIN32)
#     foreach(lib_ ${OGRE_LIBRARIES})
#       if(${lib_} MATCHES "^Ogre")
#         list(APPEND opt_libs "${OGRE_LIBRARY_DIRS}/${lib_}.lib")
#       else()
#         list(APPEND opt_libs ${lib_})
#       endif()
#     endforeach()
#   else()
#     list(APPEND opt_libs ${OGRE_LIBRARIES})
#   endif()
endif(USE_OGRE)

target_include_directories(${PROJECT_NAME} PRIVATE "src/")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE ${opt_incs})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OGRE_LIBRARIES})
