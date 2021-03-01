# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of gr-osmosdr
#
# gr-osmosdr is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# gr-osmosdr is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with gr-osmosdr; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.13)
include(GNUInstallDirs)
project(gr-osmosdr CXX C)
enable_testing()

#policy setup
cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0079 NEW)

#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# GNURadio setup
########################################################################
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)
# Find GNURadio (pmt and runtime are core, always included)
find_package(Gnuradio "3.9" REQUIRED COMPONENTS blocks fft filter)

# Set the version information here
set(VERSION_MAJOR 1)
set(VERSION_API   3)
set(VERSION_ABI   9)
set(VERSION_PATCH 0)
include(GrVersion) #setup version info

########################################################################
# Compiler specific setup
########################################################################

include(GrMiscUtils) #compiler flag check

include(TestBigEndian)
TEST_BIG_ENDIAN(GR_IS_BIG_ENDIAN)
if(GR_IS_BIG_ENDIAN)
    add_definitions(-DGR_IS_BIG_ENDIAN)
endif(GR_IS_BIG_ENDIAN)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(NOT WIN32)
        #http://gcc.gnu.org/wiki/Visibility
        GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN)
    endif(NOT WIN32)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wsign-compare HAVE_WARN_SIGN_COMPARE)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wall HAVE_WARN_ALL)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wno-uninitialized HAVE_WARN_NO_UNINITIALIZED)
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

if(MSVC)
    include_directories(${CMAKE_SOURCE_DIR}/cmake/msvc) #missing headers
    add_definitions(-D_USE_MATH_DEFINES) #enables math constants on all supported versions of MSVC
    add_definitions(-D_WIN32_WINNT=0x0502) #Minimum version: "Windows Server 2003 with SP1, Windows XP with SP2"
    add_definitions(-DNOMINMAX) #disables stupidity and enables std::min and std::max
    add_definitions( #stop all kinds of compatibility warnings
        -D_SCL_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_DEPRECATE
    )
    add_definitions(-DHAVE_CONFIG_H)
    add_compile_options(/MP) #build with multiple processors
    add_compile_options(/bigobj) #allow for larger object files
endif(MSVC)

if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-DMS_WIN64)
    endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
endif(WIN32)

# Record Compiler Info for record
STRING(TOUPPER ${CMAKE_BUILD_TYPE} GRCBTU)
SET(COMPILER_INFO "")
IF(MSVC)
    IF(MSVC_VERSION LESS "${MSVC_MIN_VERSION}")
        MESSAGE(FATAL_ERROR "MSVC Versions < minimum version ${MSVC_MIN_VERSION}")
    ENDIF()
    SET(cmake_c_compiler_version ${MSVC_VERSION})
    SET(cmake_cxx_compiler_version ${MSVC_VERSION})
ELSE()
    execute_process(COMMAND ${CMAKE_C_COMPILER} --version
            OUTPUT_VARIABLE cmake_c_compiler_version)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
            OUTPUT_VARIABLE cmake_cxx_compiler_version)
ENDIF(MSVC)
SET(COMPILER_INFO "${CMAKE_C_COMPILER}:::${CMAKE_C_FLAGS_${GRCBTU}} ${CMAKE_C_FLAGS}\n${CMAKE_CXX_COMPILER}:::${CMAKE_CXX_FLAGS_${GRCBTU}} ${CMAKE_CXX_FLAGS}\n" )

# Convert to a C string to compile and display properly
string(STRIP "${cmake_c_compiler_version}" cmake_c_compiler_version)
string(STRIP "${cmake_cxx_compiler_version}" cmake_cxx_compiler_version)
string(STRIP ${COMPILER_INFO} COMPILER_INFO)
MESSAGE(STATUS "Compiler Version: ${cmake_c_compiler_version}")
MESSAGE(STATUS "Compiler Flags: ${COMPILER_INFO}")
string(REPLACE "\n" " \\n" cmake_c_compiler_version ${cmake_c_compiler_version})
string(REPLACE "\n" " \\n" cmake_cxx_compiler_version ${cmake_cxx_compiler_version})
string(REPLACE "\n" " \\n" COMPILER_INFO ${COMPILER_INFO})

########################################################################
# Find boost
########################################################################
find_package(Boost "1.65" REQUIRED chrono thread system)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to compile osmosdr")
endif()
########################################################################
# Install directories
########################################################################
include(GrPlatform) #define LIB_SUFFIX

if(NOT CMAKE_MODULES_DIR)
    set(CMAKE_MODULES_DIR lib${LIB_SUFFIX}/cmake)
endif(NOT CMAKE_MODULES_DIR)
set(GR_INCLUDE_DIR      include)
set(GR_CMAKE_DIR        ${CMAKE_MODULES_DIR}/osmosdr)
set(GR_PKG_DATA_DIR     ${GR_DATA_DIR}/${CMAKE_PROJECT_NAME})
set(GR_PKG_DOC_DIR      ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME})
set(GR_PKG_CONF_DIR     ${GR_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d)
set(GR_PKG_LIBEXEC_DIR  ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME})
########################################################################
# Find build dependencies and setup options
########################################################################

include(GrComponent)

set(ENABLE_NONFREE FALSE CACHE BOOL "Enable or disable nonfree components.")


    # GNURadio components & OOTs
    ##############################


    # Note this is not supposed to be lique that for GR components
    # but ATM GR's handling of components is broken
message(STATUS "Searching for GNURadio-Blocks...")
find_package(gnuradio-blocks PATHS ${Gnuradio_DIR})
message(STATUS " Found GNURadio-Blocks: ${gnuradio-blocks_FOUND}")

message(STATUS "Searching for IQ Balance...")
find_package(iqbalance PATHS ${Gnuradio_DIR})
message (STATUS " Found IQ Balance: ${iqbalance_FOUND}")

message(STATUS "Searching for UHD Drivers...")
find_package(UHD)
message (STATUS " Found UHD Driver: ${UHD_FOUND}")

message(STATUS "Searching for UHD Block...")
find_package(gnuradio-uhd PATHS ${Gnuradio_DIR})
message (STATUS " Found UHD Block: ${gnuradio-uhd_FOUND}")

message(STATUS "Searching for Volk...")
find_package(Volk REQUIRED)
message (STATUS " Found Volk: ${Volk_FOUND}")

    # Hardware drivers
    ####################

find_package(LibRTLSDR)
if(ENABLE_NONFREE)
    find_package(LibSDRplay)
endif(ENABLE_NONFREE)
find_package(LibHackRF)
find_package(LibAIRSPY)
find_package(LibAIRSPYHF)
find_package(LibbladeRF)
find_package(GnuradioFuncube)
find_package(GnuradioIIO)
find_package(SoapySDR NO_MODULE)
find_package(LibFreeSRP)
find_package(Doxygen)

    # Python
    ##########

find_package(PythonLibs 3)
find_package(pybind11)

GR_REGISTER_COMPONENT("Python support" ENABLE_PYTHON
    PYTHONLIBS_FOUND
    pybind11_FOUND
)

########################################################################
# Install directories
########################################################################



########################################################################
# On Apple only, set install name and use rpath correctly, if not already set
########################################################################
if(APPLE)
    if(NOT CMAKE_INSTALL_NAME_DIR)
        set(CMAKE_INSTALL_NAME_DIR
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
            PATH "Library Install Name Destination Directory" FORCE)
    endif(NOT CMAKE_INSTALL_NAME_DIR)
    if(NOT CMAKE_INSTALL_RPATH)
        set(CMAKE_INSTALL_RPATH
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
            PATH "Library Install RPath" FORCE)
    endif(NOT CMAKE_INSTALL_RPATH)
    if(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH ON CACHE
            BOOL "Do Build Using Library Install RPath" FORCE)
    endif(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
endif(APPLE)

########################################################################
# Setup doxygen option
########################################################################
find_package(Doxygen)

if(DOXYGEN_FOUND)
    option(ENABLE_DOXYGEN "Build docs using Doxygen" ON)
else(DOXYGEN_FOUND)
    option(ENABLE_DOXYGEN "Build docs using Doxygen" OFF)
endif(DOXYGEN_FOUND)

########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/osmosdr)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(python)
    add_subdirectory(grc)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)
add_subdirectory(docs)

########################################################################
# Print Summary
########################################################################
GR_PRINT_COMPONENT_SUMMARY()
if(ENABLE_NONFREE)
    MESSAGE(STATUS
            "NONFREE components have been enabled. The resulting
   binaries cannot be distributed under GPL terms.
  ")
endif(ENABLE_NONFREE)

MESSAGE(STATUS "Building for version: ${VERSION} / ${LIBVER}")
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
