# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR},
                    ${RAPIDJSON_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND sigmf_sources
    global.cc
    capture.cc
    annotation.cc
    sigmf.cc
    sigmf_reader.cc
    sigmf_writer.cc
    sigmf_sink_impl.cc
    burst_tagger_impl.cc )

set(sigmf_sources "${sigmf_sources}" PARENT_SCOPE)
if(NOT sigmf_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT sigmf_sources)

add_library(gnuradio-sigmf SHARED ${sigmf_sources})
target_link_libraries(gnuradio-sigmf ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-sigmf PROPERTIES DEFINE_SYMBOL "gnuradio_sigmf_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-sigmf PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-sigmf RUNTIME_COMPONENT "sigmf_runtime" DEVEL_COMPONENT "sigmf_devel")


########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

