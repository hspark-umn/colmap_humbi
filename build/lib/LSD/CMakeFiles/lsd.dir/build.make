# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hspark/colmap

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hspark/colmap/build

# Include any dependencies generated for this target.
include lib/LSD/CMakeFiles/lsd.dir/depend.make

# Include the progress variables for this target.
include lib/LSD/CMakeFiles/lsd.dir/progress.make

# Include the compile flags for this target's objects.
include lib/LSD/CMakeFiles/lsd.dir/flags.make

lib/LSD/CMakeFiles/lsd.dir/lsd.c.o: lib/LSD/CMakeFiles/lsd.dir/flags.make
lib/LSD/CMakeFiles/lsd.dir/lsd.c.o: ../lib/LSD/lsd.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hspark/colmap/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object lib/LSD/CMakeFiles/lsd.dir/lsd.c.o"
	cd /home/hspark/colmap/build/lib/LSD && /usr/bin/gcc-6 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/lsd.dir/lsd.c.o   -c /home/hspark/colmap/lib/LSD/lsd.c

lib/LSD/CMakeFiles/lsd.dir/lsd.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lsd.dir/lsd.c.i"
	cd /home/hspark/colmap/build/lib/LSD && /usr/bin/gcc-6 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/hspark/colmap/lib/LSD/lsd.c > CMakeFiles/lsd.dir/lsd.c.i

lib/LSD/CMakeFiles/lsd.dir/lsd.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lsd.dir/lsd.c.s"
	cd /home/hspark/colmap/build/lib/LSD && /usr/bin/gcc-6 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/hspark/colmap/lib/LSD/lsd.c -o CMakeFiles/lsd.dir/lsd.c.s

# Object files for target lsd
lsd_OBJECTS = \
"CMakeFiles/lsd.dir/lsd.c.o"

# External object files for target lsd
lsd_EXTERNAL_OBJECTS =

lib/LSD/liblsd.a: lib/LSD/CMakeFiles/lsd.dir/lsd.c.o
lib/LSD/liblsd.a: lib/LSD/CMakeFiles/lsd.dir/build.make
lib/LSD/liblsd.a: lib/LSD/CMakeFiles/lsd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hspark/colmap/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library liblsd.a"
	cd /home/hspark/colmap/build/lib/LSD && $(CMAKE_COMMAND) -P CMakeFiles/lsd.dir/cmake_clean_target.cmake
	cd /home/hspark/colmap/build/lib/LSD && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lsd.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/LSD/CMakeFiles/lsd.dir/build: lib/LSD/liblsd.a

.PHONY : lib/LSD/CMakeFiles/lsd.dir/build

lib/LSD/CMakeFiles/lsd.dir/clean:
	cd /home/hspark/colmap/build/lib/LSD && $(CMAKE_COMMAND) -P CMakeFiles/lsd.dir/cmake_clean.cmake
.PHONY : lib/LSD/CMakeFiles/lsd.dir/clean

lib/LSD/CMakeFiles/lsd.dir/depend:
	cd /home/hspark/colmap/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hspark/colmap /home/hspark/colmap/lib/LSD /home/hspark/colmap/build /home/hspark/colmap/build/lib/LSD /home/hspark/colmap/build/lib/LSD/CMakeFiles/lsd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/LSD/CMakeFiles/lsd.dir/depend

