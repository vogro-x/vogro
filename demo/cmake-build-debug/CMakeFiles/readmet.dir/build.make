# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/andrew/Downloads/clion-2018.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/andrew/Downloads/clion-2018.2.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/andrew/Code/vogro/demo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/andrew/Code/vogro/demo/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/readmet.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/readmet.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/readmet.dir/flags.make

CMakeFiles/readmet.dir/readmet.cpp.o: CMakeFiles/readmet.dir/flags.make
CMakeFiles/readmet.dir/readmet.cpp.o: ../readmet.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/andrew/Code/vogro/demo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/readmet.dir/readmet.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/readmet.dir/readmet.cpp.o -c /home/andrew/Code/vogro/demo/readmet.cpp

CMakeFiles/readmet.dir/readmet.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/readmet.dir/readmet.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/andrew/Code/vogro/demo/readmet.cpp > CMakeFiles/readmet.dir/readmet.cpp.i

CMakeFiles/readmet.dir/readmet.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/readmet.dir/readmet.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/andrew/Code/vogro/demo/readmet.cpp -o CMakeFiles/readmet.dir/readmet.cpp.s

# Object files for target readmet
readmet_OBJECTS = \
"CMakeFiles/readmet.dir/readmet.cpp.o"

# External object files for target readmet
readmet_EXTERNAL_OBJECTS =

readmet: CMakeFiles/readmet.dir/readmet.cpp.o
readmet: CMakeFiles/readmet.dir/build.make
readmet: CMakeFiles/readmet.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/andrew/Code/vogro/demo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable readmet"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/readmet.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/readmet.dir/build: readmet

.PHONY : CMakeFiles/readmet.dir/build

CMakeFiles/readmet.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/readmet.dir/cmake_clean.cmake
.PHONY : CMakeFiles/readmet.dir/clean

CMakeFiles/readmet.dir/depend:
	cd /home/andrew/Code/vogro/demo/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/andrew/Code/vogro/demo /home/andrew/Code/vogro/demo /home/andrew/Code/vogro/demo/cmake-build-debug /home/andrew/Code/vogro/demo/cmake-build-debug /home/andrew/Code/vogro/demo/cmake-build-debug/CMakeFiles/readmet.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/readmet.dir/depend
