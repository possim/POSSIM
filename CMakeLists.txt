# Software License Agreement (BSD License)
# 
# Copyright (c) 2013, Julian de Hoog <julian@dehoog.ca>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * The name of the author may not be used to endorse or promote 
#    products derived from this software without specific prior 
#    written permission from the author.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE. 


cmake_minimum_required (VERSION 2.6)

project (POSSIM)

# Set versions, include in config that can be called from POSSIM

set (POSSIM_VERSION_MAJOR 1)
set (POSSIM_VERSION_MINOR 01)
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/POSSIMConfig.h.in
    ${CMAKE_SOURCE_DIR}/cmake/POSSIMConfig.h
)


######################################################################
# Include packages

# First, we need to find boost
find_package(Boost COMPONENTS system filesystem REQUIRED)

# If the above doesn't work on your system (say due to unconventional 
# install location), then uncomment the lines below, substituting in 
# your own boost location.
#
# set(BOOST_ROOT "/home/username/Boost/boost_1_53_0")
# set(BOOST_LIBRARYDIR "/home/username/Boost/boost_1_53_0/bin/lib")
# set(Boost_USE_STATIC_LIBS ON)
# set(Boost_DEBUG ON)
# find_package(Boost COMPONENTS system filesystem REQUIRED)

# Second, we need to make sure the system finds matlab.  Unfortunately
# there is no reliable findMatlab cmake file available (yet), so 
# enter the path to your own installation of matlab manually

# Linux example
# SET(MATLAB_ROOT "/usr/local/MATLAB/R2012a")
# SET(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/bin/glnx86")

# Windows example
# SET(MATLAB_ROOT "C:/Program Files/MATLAB/R2012a")
# SET(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/extern/lib/win32/microsoft")

# Mac example
SET(MATLAB_ROOT "/Applications/MATLAB_R2012a.app/")
SET(MATLAB_LIBRARIES_DIR "${MATLAB_ROOT}/bin/maci64")


######################################################################
# The rest of this file should work on all platforms

SET(MATLAB_INCLUDE_DIR   "${MATLAB_ROOT}/extern/include")
find_library(MATLAB_MEX_LIBRARY mex ${MATLAB_LIBRARIES_DIR})
find_library(MATLAB_MX_LIBRARY  mx  ${MATLAB_LIBRARIES_DIR})
find_library(MATLAB_ENG_LIBRARY eng ${MATLAB_LIBRARIES_DIR})
        
# Find POSSIM source and header files
FILE(GLOB_RECURSE POSSIM_SOURCES src/*.cpp)
FILE(GLOB_RECURSE POSSIM_HEADERS src/*.h)

include_directories(
        ${Boost_INCLUDE_DIRS}
    	${MATLAB_INCLUDE_DIR}
        ${PROJECT_BINARY_DIR}
)

link_directories( 
        ${MATLAB_LIBRARIES_DIR} 
        ${Boost_LIBRARY_DIRS} 
)

add_executable(possim 
        ${POSSIM_SOURCES} 
        ${POSSIM_HEADERS}
)

target_link_libraries(possim 
        ${MATLAB_MX_LIBRARY} 
        ${MATLAB_ENG_LIBRARY}
        ${Boost_LIBRARIES}
)

install (TARGETS possim DESTINATION bin)
