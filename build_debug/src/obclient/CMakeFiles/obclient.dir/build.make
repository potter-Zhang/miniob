# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/someone/桌面/myd/miniob合并/sdk_miniob

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/someone/桌面/myd/miniob合并/sdk_miniob/build

# Include any dependencies generated for this target.
include src/obclient/CMakeFiles/obclient.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/obclient/CMakeFiles/obclient.dir/compiler_depend.make

# Include the progress variables for this target.
include src/obclient/CMakeFiles/obclient.dir/progress.make

# Include the compile flags for this target's objects.
include src/obclient/CMakeFiles/obclient.dir/flags.make

src/obclient/CMakeFiles/obclient.dir/client.cpp.o: src/obclient/CMakeFiles/obclient.dir/flags.make
src/obclient/CMakeFiles/obclient.dir/client.cpp.o: ../src/obclient/client.cpp
src/obclient/CMakeFiles/obclient.dir/client.cpp.o: src/obclient/CMakeFiles/obclient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/someone/桌面/myd/miniob合并/sdk_miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/obclient/CMakeFiles/obclient.dir/client.cpp.o"
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/src/obclient && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/obclient/CMakeFiles/obclient.dir/client.cpp.o -MF CMakeFiles/obclient.dir/client.cpp.o.d -o CMakeFiles/obclient.dir/client.cpp.o -c /home/someone/桌面/myd/miniob合并/sdk_miniob/src/obclient/client.cpp

src/obclient/CMakeFiles/obclient.dir/client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/obclient.dir/client.cpp.i"
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/src/obclient && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/someone/桌面/myd/miniob合并/sdk_miniob/src/obclient/client.cpp > CMakeFiles/obclient.dir/client.cpp.i

src/obclient/CMakeFiles/obclient.dir/client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/obclient.dir/client.cpp.s"
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/src/obclient && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/someone/桌面/myd/miniob合并/sdk_miniob/src/obclient/client.cpp -o CMakeFiles/obclient.dir/client.cpp.s

# Object files for target obclient
obclient_OBJECTS = \
"CMakeFiles/obclient.dir/client.cpp.o"

# External object files for target obclient
obclient_EXTERNAL_OBJECTS =

bin/obclient: src/obclient/CMakeFiles/obclient.dir/client.cpp.o
bin/obclient: src/obclient/CMakeFiles/obclient.dir/build.make
bin/obclient: lib/libcommon.a
bin/obclient: src/obclient/CMakeFiles/obclient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/someone/桌面/myd/miniob合并/sdk_miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/obclient"
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/src/obclient && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/obclient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/obclient/CMakeFiles/obclient.dir/build: bin/obclient
.PHONY : src/obclient/CMakeFiles/obclient.dir/build

src/obclient/CMakeFiles/obclient.dir/clean:
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/src/obclient && $(CMAKE_COMMAND) -P CMakeFiles/obclient.dir/cmake_clean.cmake
.PHONY : src/obclient/CMakeFiles/obclient.dir/clean

src/obclient/CMakeFiles/obclient.dir/depend:
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/someone/桌面/myd/miniob合并/sdk_miniob /home/someone/桌面/myd/miniob合并/sdk_miniob/src/obclient /home/someone/桌面/myd/miniob合并/sdk_miniob/build /home/someone/桌面/myd/miniob合并/sdk_miniob/build/src/obclient /home/someone/桌面/myd/miniob合并/sdk_miniob/build/src/obclient/CMakeFiles/obclient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/obclient/CMakeFiles/obclient.dir/depend

