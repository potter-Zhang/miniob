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
include src/observer/CMakeFiles/observer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/observer/CMakeFiles/observer.dir/compiler_depend.make

# Include the progress variables for this target.
include src/observer/CMakeFiles/observer.dir/progress.make

# Include the compile flags for this target's objects.
include src/observer/CMakeFiles/observer.dir/flags.make

src/observer/CMakeFiles/observer.dir/main.cpp.o: src/observer/CMakeFiles/observer.dir/flags.make
src/observer/CMakeFiles/observer.dir/main.cpp.o: ../src/observer/main.cpp
src/observer/CMakeFiles/observer.dir/main.cpp.o: src/observer/CMakeFiles/observer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/harry/Desktop/miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/observer/CMakeFiles/observer.dir/main.cpp.o"
	cd /home/harry/Desktop/miniob/build/src/observer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/observer/CMakeFiles/observer.dir/main.cpp.o -MF CMakeFiles/observer.dir/main.cpp.o.d -o CMakeFiles/observer.dir/main.cpp.o -c /home/harry/Desktop/miniob/src/observer/main.cpp

src/observer/CMakeFiles/observer.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/observer.dir/main.cpp.i"
	cd /home/harry/Desktop/miniob/build/src/observer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/Desktop/miniob/src/observer/main.cpp > CMakeFiles/observer.dir/main.cpp.i

src/observer/CMakeFiles/observer.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/observer.dir/main.cpp.s"
	cd /home/harry/Desktop/miniob/build/src/observer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/Desktop/miniob/src/observer/main.cpp -o CMakeFiles/observer.dir/main.cpp.s

# Object files for target observer
observer_OBJECTS = \
"CMakeFiles/observer.dir/main.cpp.o"

# External object files for target observer
observer_EXTERNAL_OBJECTS =

bin/observer: src/observer/CMakeFiles/observer.dir/main.cpp.o
bin/observer: src/observer/CMakeFiles/observer.dir/build.make
bin/observer: lib/libobserver.a
bin/observer: lib/libcommon.a
bin/observer: /usr/local/lib/libevent_pthreads.a
bin/observer: /usr/local/lib/libevent_core.a
bin/observer: src/observer/CMakeFiles/observer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/harry/Desktop/miniob/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/observer"
	cd /home/harry/Desktop/miniob/build/src/observer && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/observer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/observer/CMakeFiles/observer.dir/build: bin/observer
.PHONY : src/observer/CMakeFiles/observer.dir/build

src/observer/CMakeFiles/observer.dir/clean:
	cd /home/harry/Desktop/miniob/build/src/observer && $(CMAKE_COMMAND) -P CMakeFiles/observer.dir/cmake_clean.cmake
.PHONY : src/observer/CMakeFiles/observer.dir/clean

src/observer/CMakeFiles/observer.dir/depend:
	cd /home/harry/Desktop/miniob/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/harry/Desktop/miniob /home/harry/Desktop/miniob/src/observer /home/harry/Desktop/miniob/build /home/harry/Desktop/miniob/build/src/observer /home/harry/Desktop/miniob/build/src/observer/CMakeFiles/observer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/observer/CMakeFiles/observer.dir/depend

