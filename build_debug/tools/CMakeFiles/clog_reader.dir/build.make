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
CMAKE_SOURCE_DIR = /home/someone/桌面/myd/miniob-2023/sdk_miniob

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/someone/桌面/myd/miniob-2023/sdk_miniob/build

# Include any dependencies generated for this target.
include tools/CMakeFiles/clog_reader.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tools/CMakeFiles/clog_reader.dir/compiler_depend.make

# Include the progress variables for this target.
include tools/CMakeFiles/clog_reader.dir/progress.make

# Include the compile flags for this target's objects.
include tools/CMakeFiles/clog_reader.dir/flags.make

tools/CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o: tools/CMakeFiles/clog_reader.dir/flags.make
tools/CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o: ../tools/clog_reader_cmd.cpp
tools/CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o: tools/CMakeFiles/clog_reader.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tools/CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o"
	cd /home/someone/桌面/myd/miniob-2023/sdk_miniob/build/tools && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tools/CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o -MF CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o.d -o CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o -c /home/someone/桌面/myd/miniob-2023/sdk_miniob/tools/clog_reader_cmd.cpp

tools/CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.i"
	cd /home/someone/桌面/myd/miniob-2023/sdk_miniob/build/tools && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/someone/桌面/myd/miniob-2023/sdk_miniob/tools/clog_reader_cmd.cpp > CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.i

tools/CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.s"
	cd /home/someone/桌面/myd/miniob-2023/sdk_miniob/build/tools && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/someone/桌面/myd/miniob-2023/sdk_miniob/tools/clog_reader_cmd.cpp -o CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.s

# Object files for target clog_reader
clog_reader_OBJECTS = \
"CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o"

# External object files for target clog_reader
clog_reader_EXTERNAL_OBJECTS =

bin/clog_reader: tools/CMakeFiles/clog_reader.dir/clog_reader_cmd.cpp.o
bin/clog_reader: tools/CMakeFiles/clog_reader.dir/build.make
bin/clog_reader: lib/libobserver.a
bin/clog_reader: lib/libcommon.a
bin/clog_reader: /usr/local/lib/libevent_pthreads.a
bin/clog_reader: /usr/local/lib/libevent_core.a
bin/clog_reader: tools/CMakeFiles/clog_reader.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/clog_reader"
	cd /home/someone/桌面/myd/miniob-2023/sdk_miniob/build/tools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/clog_reader.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/CMakeFiles/clog_reader.dir/build: bin/clog_reader
.PHONY : tools/CMakeFiles/clog_reader.dir/build

tools/CMakeFiles/clog_reader.dir/clean:
	cd /home/someone/桌面/myd/miniob-2023/sdk_miniob/build/tools && $(CMAKE_COMMAND) -P CMakeFiles/clog_reader.dir/cmake_clean.cmake
.PHONY : tools/CMakeFiles/clog_reader.dir/clean

tools/CMakeFiles/clog_reader.dir/depend:
	cd /home/someone/桌面/myd/miniob-2023/sdk_miniob/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/someone/桌面/myd/miniob-2023/sdk_miniob /home/someone/桌面/myd/miniob-2023/sdk_miniob/tools /home/someone/桌面/myd/miniob-2023/sdk_miniob/build /home/someone/桌面/myd/miniob-2023/sdk_miniob/build/tools /home/someone/桌面/myd/miniob-2023/sdk_miniob/build/tools/CMakeFiles/clog_reader.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/CMakeFiles/clog_reader.dir/depend

