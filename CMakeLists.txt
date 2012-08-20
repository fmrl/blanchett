# $legal:1594:
# 
# Copyright (c) 2011, Michael Lowell Roberts.  
# All rights reserved. 
# 
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are 
# met: 
# 
#   - Redistributions of source code must retain the above copyright 
#   notice, this list of conditions and the following disclaimer. 
# 
#   - Redistributions in binary form must reproduce the above copyright 
#   notice, this list of conditions and the following disclaimer in the 
#   documentation and/or other materials provided with the distribution.
#  
#   - Neither the name of the copyright holder nor the names of 
#   contributors may be used to endorse or promote products derived 
#   from this software without specific prior written permission. 
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER 
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
# 
# ,$

cmake_minimum_required(VERSION 2.6)
project(blanchett)

include(cmakes/cache.cmake)

# toolchain configuration
# -----------------------
# on linux, the intel compiler executable is called *icc*.
if(${CMAKE_C_COMPILER} MATCHES icc)
	set(COMPILER_IS_ICC YES)
endif()
if(CMAKE_COMPILER_IS_GNUCC)
	# -Wmissing-braces and -Wmissing-field-initializers appears
	# to interfere with C89-defined behavior.
	# "-rdynamic" tells the linker to put all symbols into the symbol
	# table.
	# [mlr][todo] make the following setting configurable.
	# "-Wfatal-errors tells the compiler to stop after one error.
	set(DEFAULT_CFLAGS "-rdynamic -Wall -Wextra -Wno-missing-braces -Wno-missing-field-initializers -Wfatal-errors")
elseif(COMPILER_IS_ICC)
	# nothing to do (yet).
elseif(MSVC)
	# C4127 pops up every time a "do {} while (0)" is used.
	set(DEFAULT_CFLAGS "/W4 /wd4127")
else()
	message(WARNING "i don't know how to configure CMAKE_C_FLAGS for this compiler.")
endif()
set_cache_builtin(STRING CMAKE_C_FLAGS ${DEFAULT_CFLAGS})
set_cache_builtin(STRING CMAKE_CXX_FLAGS ${DEFAULT_CFLAGS})

include(cmakes/config.cmake)
generate_config_header(blanchett)

# include directory search path
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include
	)

# dependencies
# ------------
# threads
find_package(Threads REQUIRED)
# boost
include(cmakes/boost.cmake)
# [mlr][todo] blanchett doesn't need regex but nyan does. is there a way
# that i can abstract this requirement into nyan.cmake and boost.cmake?
find_boost(1.48.0 REQUIRED COMPONENTS thread regex)
include_directories(${Boost_INCLUDE_DIRS})
# nyan
include(cmakes/nyan.cmake)

add_library(blanchett 
	include/blanchett/actors.hpp
	include/blanchett/actors/actor.hpp
	include/blanchett/actors/consumer.hpp
	include/blanchett/actors/pool.hpp
	include/blanchett/actors/job.hpp
	include/blanchett/actors/reactor.hpp
	include/blanchett/actors/simple_reactor.hpp
	include/blanchett/actors/thread.hpp
	include/blanchett/actors/stl_queue_adaptor.hpp
	include/blanchett/actors/quit.hpp
	src/lib/actors/job.cpp
	src/lib/actors/quit.cpp
	)
target_link_libraries(blanchett ${NYAN_LIBRARIES} ${Boost_LIBRARIES})

add_executable(test src/test/test.cpp)
target_link_libraries(test blanchett)

# $vim:23: vim:set sts=3 sw=3 et:,$