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

set (POSSIM_VERSION_MAJOR 0)
set (POSSIM_VERSION_MINOR 1)
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/POSSIMConfig.h.in
    ${CMAKE_SOURCE_DIR}/cmake/POSSIMConfig.h
)



# Include packages

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(Boost_USE_STATIC_LIBS        ON)
#set(Boost_USE_MULTITHREADED      ON)
set(Boost_DEBUG      ON)
#set(Boost_USE_STATIC_RUNTIME    OFF)
#set(BOOST_ROOT "C:/Program Files/boost/boost_1_53_0")
#set(BOOST_LIBRARYDIR "C:/Program Files/boost/boost_1_53_0/libs;C:/Program Files/boost/boost_1_53_0/bin.v2/libs")
#set(Boost_LIB_PREFIX "lib")
find_package(Boost COMPONENTS system filesystem REQUIRED)
#find_package(Boost REQUIRED)

if(Boost_FOUND)
   MESSAGE(STATUS "FOUND BOOST")
endif(Boost_FOUND)

# find_package(Matlab)
# if(NOT MATLAB_FOUND)
#        MESSAGE(STATUS "Could not find MATLAB package, setting paths manually")
#        SET(MATLAB_ROOT /Applications/MATLAB_R2011b.app)
#        SET(MATLAB_LIBRARIES ${MATLAB_ROOT}/bin/maci64)
#        SET(MATLAB_INCLUDE_DIR ${MATLAB_ROOT}/extern/include)
#        SET(MATLAB_MEX_LIBRARY ${MATLAB_LIBRARIES}/libmex.dylib)
#        SET(MATLAB_MX_LIBRARY  ${MATLAB_LIBRARIES}/libmx.dylib)
#        SET(MATLAB_ENG_LIBRARY ${MATLAB_LIBRARIES}/libeng.dylib)
# endif(NOT MATLAB_FOUND)

SET(MATLAB_ROOT "C:/Program Files/MATLAB/R2013a")
SET(MATLAB_LIBRARIES ${MATLAB_ROOT}/bin/win32)
SET(MATLAB_INCLUDE_DIR ${MATLAB_ROOT}/extern/include)
SET(MATLAB_MEX_LIBRARY ${MATLAB_LIBRARIES}/libmex.dll)
SET(MATLAB_MX_LIBRARY  ${MATLAB_LIBRARIES}/libmx.dll)
SET(MATLAB_ENG_LIBRARY ${MATLAB_LIBRARIES}/libeng.dll)


# Ensure MATLAB is on the path

# set(ENV{LD_LIBRARY_PATH} ENV{LD_LIBRARY_PATH} ${MATLAB_LIBRARIES})

# Find source and header files

FILE(GLOB_RECURSE POSSIM_SOURCES src/*.cpp)
FILE(GLOB_RECURSE POSSIM_HEADERS src/*.h)



include_directories(
        ${Boost_INCLUDE_DIRS}
	${MATLAB_INCLUDE_DIR}
        ${PROJECT_BINARY_DIR}
)

link_directories( 
        ${MATLAB_LIBRARIES_PATHS} 
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

