# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

message(STATUS "Extra includes: ${Qt5Widgets_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ")
set(extra_includes ${Qt5Widgets_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
get_directory_property( DirDefs COMPILE_DEFINITIONS )
message(STATUS "COMPILE_DEFINITIONS = ${DirDefs}" )
set(define_symbols ${DirDefs};__PIC__)
message(STATUS "OLd defines: $CACHE{OLD_DEFINES}" )
message(STATUS "Definitions = ${define_symbols}" )
string(COMPARE NOTEQUAL "$CACHE{OLD_DEFINES}" "${define_symbols}" FORCE_REBUILD )
set(OLD_DEFINES "${define_symbols}" CACHE STRING "Save old defines" FORCE)
message(STATUS "Bindings rebuild forced ${FORCE_REBUILD}")

list(APPEND osmosdr_python_files
    ranges_python.cc
    sink_python.cc
    source_python.cc
    time_spec_python.cc
    device_python.cc
    python_bindings.cc
)

GR_PYBIND_MAKE_OOT(osmosdr
   ../..
   osmosdr
   "${osmosdr_python_files}")

install(TARGETS osmosdr_python DESTINATION ${GR_PYTHON_DIR}/osmosdr COMPONENT pythonapi)
