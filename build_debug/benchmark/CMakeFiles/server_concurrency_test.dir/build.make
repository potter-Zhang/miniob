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
CMAKE_SOURCE_DIR = /home/harry/Desktop/miniob

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/harry/Desktop/miniob/build

# Include any dependencies generated for this target.
include benchmark/CMakeFiles/server_concurrency_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include benchmark/CMakeFiles/server_concurrency_test.dir/compiler_depend.make

# Include the progress variables for this target.
include benchmark/CMakeFiles/server_concurrency_test.dir/progress.make

# Include the compile flags for this target's objects.
include benchmark/CMakeFiles/server_concurrency_test.dir/flags.make

benchmark/CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o: benchmark/CMakeFiles/server_concurrency_test.dir/flags.make
benchmark/CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o: ../benchmark/server_concurrency_test.cpp
benchmark/CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o: benchmark/CMakeFiles/server_concurrency_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/harry/Desktop/miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object benchmark/CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o"
	cd /home/harry/Desktop/miniob/build/benchmark && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT benchmark/CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o -MF CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o.d -o CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o -c /home/harry/Desktop/miniob/benchmark/server_concurrency_test.cpp

benchmark/CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.i"
	cd /home/harry/Desktop/miniob/build/benchmark && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/Desktop/miniob/benchmark/server_concurrency_test.cpp > CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.i

benchmark/CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.s"
	cd /home/harry/Desktop/miniob/build/benchmark && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/Desktop/miniob/benchmark/server_concurrency_test.cpp -o CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.s

# Object files for target server_concurrency_test
server_concurrency_test_OBJECTS = \
"CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o"

# External object files for target server_concurrency_test
server_concurrency_test_EXTERNAL_OBJECTS =

bin/server_concurrency_test: benchmark/CMakeFiles/server_concurrency_test.dir/server_concurrency_test.cpp.o
bin/server_concurrency_test: benchmark/CMakeFiles/server_concurrency_test.dir/build.make
bin/server_concurrency_test: lib/libcommon.a
bin/server_concurrency_test: lib/libobserver.a
bin/server_concurrency_test: lib/libcommon.a
bin/server_concurrency_test: /usr/local/lib/libevent_pthreads.a
bin/server_concurrency_test: /usr/local/lib/libevent_core.a
bin/server_concurrency_test: benchmark/CMakeFiles/server_concurrency_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/harry/Desktop/miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/server_concurrency_test"
	cd /home/harry/Desktop/miniob/build/benchmark && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server_concurrency_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
benchmark/CMakeFiles/server_concurrency_test.dir/build: bin/server_concurrency_test
.PHONY : benchmark/CMakeFiles/server_concurrency_test.dir/build

benchmark/CMakeFiles/server_concurrency_test.dir/clean:
	cd /home/harry/Desktop/miniob/build/benchmark && $(CMAKE_COMMAND) -P CMakeFiles/server_concurrency_test.dir/cmake_clean.cmake
.PHONY : benchmark/CMakeFiles/server_concurrency_test.dir/clean

benchmark/CMakeFiles/server_concurrency_test.dir/depend:
	cd /home/harry/Desktop/miniob/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/harry/Desktop/miniob /home/harry/Desktop/miniob/benchmark /home/harry/Desktop/miniob/build /home/harry/Desktop/miniob/build/benchmark /home/harry/Desktop/miniob/build/benchmark/CMakeFiles/server_concurrency_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : benchmark/CMakeFiles/server_concurrency_test.dir/depend

