#
# Copyright 2008,2009 Free Software Foundation, Inc.
#
# This application is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This application is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

# The presence of this file turns this directory into a Python package

'''
This is the GNU Radio OsmoSDR module.
'''

from __future__ import unicode_literals
import os

# import pybind11 generated symbols into the iio namespace
try:
    from .osmosdr_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .osmosdr_python import *

# import any pure python here
#
