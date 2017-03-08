project(fovris)

include(GNUInstallDirs)
include(CheckCXXCompilerFlag)

cmake_minimum_required(VERSION 2.8)

macro(remove_cxx_flag flag)
  string(REPLACE "${flag}" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REPLACE "${flag}" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endmacro()

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic"
    )

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -fno-rtti -O2 -Wno-unused-variable -Wno-unused-parameter -DNDEBUG"
    )

find_package(OpenMP)
if (OPENMP_FOUND)
    message( "OpenMP found. Will be used to support multithreading.")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(Boost
    COMPONENTS system)
if (!BOOST_FOUND)
    message( FATAL_ERROR "Boost library not found! Required to compile.")
else()
    message(${Boost_LIBRARIES})
    message(${Boost_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
endif()

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_LIBRARY_ARCHITECTURE}/fovris:\\$ORIGIN/:\\$ORIGIN/../lib/${CMAKE_LIBRARY_ARCHITECTURE}/fovris")

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#remove -rdynamic
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

remove_cxx_flag("-O3")

include_directories(
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/src"
    "${PROJECT_SOURCE_DIR}/thirdparty"
    "${PROJECT_SOURCE_DIR}"
    )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#3rd party
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lemon)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linenoise-ng EXCLUDE_FROM_ALL)

#set output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
foreach(CFGNAME ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CFGNAME} CFGNAME)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFGNAME} ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFGNAME} ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFGNAME} ${CMAKE_BINARY_DIR}/lib)
endforeach()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cli)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/server)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples/code)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_custom_target(functionaltest
    COMMAND "FOURQL_PATH=$<TARGET_FILE:fovris>" python3 "${PROJECT_SOURCE_DIR}/test/functional/run_tests.py"
    DEPENDS fovris
)

add_custom_target(unittest
    DEPENDS alltests
    COMMAND alltests --force-colour
)

add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_MODULE_PATH}/uninstall.cmake"
)

file(GLOB MANPAGES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  man/fovris.1)

install(
  FILES ${MANPAGES}
  DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

set(CPACK_PACKAGE_NAME "fovris")
set(CPACK_PACKAGE_VENDOR "")
set(CPACK_PACKAGE_VERSION 0)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "fovris")
set(CPACK_PACKAGE_FILE_NAME "fovris")
set(CPACK_TOPLEVEL_TAG "fovris")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
include(CPack)
