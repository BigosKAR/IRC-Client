# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.30

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\build"

# Include any dependencies generated for this target.
include CMakeFiles/irc_client.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/irc_client.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/irc_client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/irc_client.dir/flags.make

CMakeFiles/irc_client.dir/main.c.obj: CMakeFiles/irc_client.dir/flags.make
CMakeFiles/irc_client.dir/main.c.obj: C:/Users/oskar/OneDrive/Desktop/Programowanie/IRC\ Client/main.c
CMakeFiles/irc_client.dir/main.c.obj: CMakeFiles/irc_client.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/irc_client.dir/main.c.obj"
	C:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/irc_client.dir/main.c.obj -MF CMakeFiles\irc_client.dir\main.c.obj.d -o CMakeFiles\irc_client.dir\main.c.obj -c "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\main.c"

CMakeFiles/irc_client.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/irc_client.dir/main.c.i"
	C:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\main.c" > CMakeFiles\irc_client.dir\main.c.i

CMakeFiles/irc_client.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/irc_client.dir/main.c.s"
	C:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\main.c" -o CMakeFiles\irc_client.dir\main.c.s

# Object files for target irc_client
irc_client_OBJECTS = \
"CMakeFiles/irc_client.dir/main.c.obj"

# External object files for target irc_client
irc_client_EXTERNAL_OBJECTS =

irc_client.exe: CMakeFiles/irc_client.dir/main.c.obj
irc_client.exe: CMakeFiles/irc_client.dir/build.make
irc_client.exe: CMakeFiles/irc_client.dir/linkLibs.rsp
irc_client.exe: CMakeFiles/irc_client.dir/objects1.rsp
irc_client.exe: CMakeFiles/irc_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable irc_client.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\irc_client.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/irc_client.dir/build: irc_client.exe
.PHONY : CMakeFiles/irc_client.dir/build

CMakeFiles/irc_client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\irc_client.dir\cmake_clean.cmake
.PHONY : CMakeFiles/irc_client.dir/clean

CMakeFiles/irc_client.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client" "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client" "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\build" "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\build" "C:\Users\oskar\OneDrive\Desktop\Programowanie\IRC Client\build\CMakeFiles\irc_client.dir\DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/irc_client.dir/depend
