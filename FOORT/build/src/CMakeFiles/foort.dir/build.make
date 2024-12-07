# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build

# Include any dependencies generated for this target.
include src/CMakeFiles/foort.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/foort.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/foort.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/foort.dir/flags.make

src/CMakeFiles/foort.dir/codegen:
.PHONY : src/CMakeFiles/foort.dir/codegen

src/CMakeFiles/foort.dir/Main.cpp.o: src/CMakeFiles/foort.dir/flags.make
src/CMakeFiles/foort.dir/Main.cpp.o: /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/src/Main.cpp
src/CMakeFiles/foort.dir/Main.cpp.o: src/CMakeFiles/foort.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/foort.dir/Main.cpp.o"
	cd /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/src && /opt/homebrew/bin/g++-14 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/foort.dir/Main.cpp.o -MF CMakeFiles/foort.dir/Main.cpp.o.d -o CMakeFiles/foort.dir/Main.cpp.o -c /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/src/Main.cpp

src/CMakeFiles/foort.dir/Main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/foort.dir/Main.cpp.i"
	cd /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/src && /opt/homebrew/bin/g++-14 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/src/Main.cpp > CMakeFiles/foort.dir/Main.cpp.i

src/CMakeFiles/foort.dir/Main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/foort.dir/Main.cpp.s"
	cd /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/src && /opt/homebrew/bin/g++-14 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/src/Main.cpp -o CMakeFiles/foort.dir/Main.cpp.s

# Object files for target foort
foort_OBJECTS = \
"CMakeFiles/foort.dir/Main.cpp.o"

# External object files for target foort
foort_EXTERNAL_OBJECTS =

/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/CMakeFiles/foort.dir/Main.cpp.o
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/CMakeFiles/foort.dir/build.make
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libconfig_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libconfig_reader_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libdiagnostics_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libdiagnostics_emission_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libgeodesic_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libinput_output_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libintegrators_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libmesh_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libmetric_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libterminations_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libutilities_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/libviewscreen_lib.a
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: /opt/homebrew/Cellar/gcc/14.2.0_1/lib/gcc/current/libgomp.dylib
/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort: src/CMakeFiles/foort.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort"
	cd /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/foort.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/foort.dir/build: /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/bin/foort
.PHONY : src/CMakeFiles/foort.dir/build

src/CMakeFiles/foort.dir/clean:
	cd /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/src && $(CMAKE_COMMAND) -P CMakeFiles/foort.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/foort.dir/clean

src/CMakeFiles/foort.dir/depend:
	cd /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/src /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/src /Users/seppestaelens/Documents/GitHubRepos/FOORT/FOORT/build/src/CMakeFiles/foort.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/CMakeFiles/foort.dir/depend

