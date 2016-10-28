########################################
# BEGIN_COPYRIGHT
#
# This file is part of SciDB.
# Copyright (C) 2008-2011 SciDB, Inc.
#
# SciDB is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
# INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
# NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
# the GNU General Public License for the complete license terms.
#
# You should have received a copy of the GNU General Public License
# along with SciDB.  If not, see <http://www.gnu.org/licenses/>.
#
# END_COPYRIGHT
########################################

#
# Finding cat executable.
#
# Once done this will define:
#	CAT_EXECUTABLE	- Full path to cat binary 

include(FindPackageHandleStandardArgs)

find_program(CAT_EXECUTABLE cat ${CAT_BIN_DIR})

find_package_handle_standard_args(CAT DEFAULT_MSG CAT_EXECUTABLE)

mark_as_advanced(CAT_EXECUTABLE)
