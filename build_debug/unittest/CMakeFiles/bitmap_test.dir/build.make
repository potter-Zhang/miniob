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
include unittest/CMakeFiles/bitmap_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include unittest/CMakeFiles/bitmap_test.dir/compiler_depend.make

# Include the progress variables for this target.
include unittest/CMakeFiles/bitmap_test.dir/progress.make

# Include the compile flags for this target's objects.
include unittest/CMakeFiles/bitmap_test.dir/flags.make

unittest/CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o: unittest/CMakeFiles/bitmap_test.dir/flags.make
unittest/CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o: ../unittest/bitmap_test.cpp
unittest/CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o: unittest/CMakeFiles/bitmap_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/someone/桌面/myd/miniob合并/sdk_miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object unittest/CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o"
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT unittest/CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o -MF CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o.d -o CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o -c /home/someone/桌面/myd/miniob合并/sdk_miniob/unittest/bitmap_test.cpp

unittest/CMakeFiles/bitmap_test.dir/bitmap_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bitmap_test.dir/bitmap_test.cpp.i"
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/someone/桌面/myd/miniob合并/sdk_miniob/unittest/bitmap_test.cpp > CMakeFiles/bitmap_test.dir/bitmap_test.cpp.i

unittest/CMakeFiles/bitmap_test.dir/bitmap_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bitmap_test.dir/bitmap_test.cpp.s"
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/someone/桌面/myd/miniob合并/sdk_miniob/unittest/bitmap_test.cpp -o CMakeFiles/bitmap_test.dir/bitmap_test.cpp.s

# Object files for target bitmap_test
bitmap_test_OBJECTS = \
"CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o"

# External object files for target bitmap_test
bitmap_test_EXTERNAL_OBJECTS =

bin/bitmap_test: unittest/CMakeFiles/bitmap_test.dir/bitmap_test.cpp.o
bin/bitmap_test: unittest/CMakeFiles/bitmap_test.dir/build.make
bin/bitmap_test: lib/libcommon.a
bin/bitmap_test: lib/libobserver.a
bin/bitmap_test: lib/libcommon.a
bin/bitmap_test: /usr/local/lib/libevent_pthreads.a
bin/bitmap_test: /usr/local/lib/libevent_core.a
bin/bitmap_test: unittest/CMakeFiles/bitmap_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/someone/桌面/myd/miniob合并/sdk_miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/bitmap_test"
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bitmap_test.dir/link.txt --verbose=$(VERBOSE)
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest && /usr/bin/cmake -D TEST_TARGET=bitmap_test -D TEST_EXECUTABLE=/home/someone/桌面/myd/miniob合并/sdk_miniob/build/bin/bitmap_test -D TEST_EXECUTOR= -D TEST_WORKING_DIR=/home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest -D TEST_EXTRA_ARGS= -D TEST_PROPERTIES= -D TEST_PREFIX= -D TEST_SUFFIX= -D TEST_FILTER= -D NO_PRETTY_TYPES=FALSE -D NO_PRETTY_VALUES=FALSE -D TEST_LIST=bitmap_test_TESTS -D CTEST_FILE=/home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest/bitmap_test[1]_tests.cmake -D TEST_DISCOVERY_TIMEOUT=5 -D TEST_XML_OUTPUT_DIR= -P /usr/share/cmake-3.22/Modules/GoogleTestAddTests.cmake

# Rule to build all files generated by this target.
unittest/CMakeFiles/bitmap_test.dir/build: bin/bitmap_test
.PHONY : unittest/CMakeFiles/bitmap_test.dir/build

unittest/CMakeFiles/bitmap_test.dir/clean:
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest && $(CMAKE_COMMAND) -P CMakeFiles/bitmap_test.dir/cmake_clean.cmake
.PHONY : unittest/CMakeFiles/bitmap_test.dir/clean

unittest/CMakeFiles/bitmap_test.dir/depend:
	cd /home/someone/桌面/myd/miniob合并/sdk_miniob/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/someone/桌面/myd/miniob合并/sdk_miniob /home/someone/桌面/myd/miniob合并/sdk_miniob/unittest /home/someone/桌面/myd/miniob合并/sdk_miniob/build /home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest /home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest/CMakeFiles/bitmap_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : unittest/CMakeFiles/bitmap_test.dir/depend

