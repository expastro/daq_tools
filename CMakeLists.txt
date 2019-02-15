cmake_minimum_required (VERSION 3.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
project(daq_tools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

# set up output paths for executable binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Sanity check : Check that all submodules are present
# =======================================================================

function(check_submodules_present)
    file(READ "${PROJECT_SOURCE_DIR}/.gitmodules" gitmodules)
    string(REGEX MATCHALL "path *= *[^ \t\r\n]*" gitmodules ${gitmodules})
    foreach(module ${gitmodules})
        string(REGEX REPLACE "path *= *" "" module ${module})
        if (NOT EXISTS "${PROJECT_SOURCE_DIR}/${module}/.git")
            message(SEND_ERROR "Git submodule ${module} not found."
            "Please run: git submodule update --init --recursive")
        endif()
    endforeach()
endfunction()
check_submodules_present()

# Setup a custom clang-format target (if clang-format can be found) that will run
# against all the src files. This should be used before making a pull request.
# =======================================================================

set(CLANG_FORMAT_POSTFIX "-6.0")
find_program(CLANG_FORMAT
NAMES clang-format${CLANG_FORMAT_POSTFIX}
clang-format
PATHS ${PROJECT_BINARY_DIR}/externals)
# if find_program doesn't find it, try to download from externals
if (NOT CLANG_FORMAT)
if (WIN32)
message(STATUS "Clang format not found! Downloading...")
set(CLANG_FORMAT "${PROJECT_BINARY_DIR}/externals/clang-format${CLANG_FORMAT_POSTFIX}.exe")
file(DOWNLOAD
https://github.com/yuzu-emu/ext-windows-bin/raw/master/clang-format${CLANG_FORMAT_POSTFIX}.exe
"${CLANG_FORMAT}" SHOW_PROGRESS
STATUS DOWNLOAD_SUCCESS)
if (NOT DOWNLOAD_SUCCESS EQUAL 0)
message(WARNING "Could not download clang format! Disabling the clang format target")
file(REMOVE ${CLANG_FORMAT})
unset(CLANG_FORMAT)
endif()
else()
message(WARNING "Clang format not found! Disabling the clang format target")
endif()
endif()

if (CLANG_FORMAT)
set(SRCS ${PROJECT_SOURCE_DIR}/src)
set(CCOMMENT "Running clang format against all the .h and .cpp files in src/")
if (WIN32)
add_custom_target(clang-format
COMMAND powershell.exe -Command "Get-ChildItem ${SRCS}/* -Include *.cpp,*.h -Recurse | Foreach {${CLANG_FORMAT} -i $_.fullname}"
COMMENT ${CCOMMENT})
elseif(MINGW)
add_custom_target(clang-format
COMMAND find `cygpath -u ${SRCS}` -iname *.h -o -iname *.cpp | xargs `cygpath -u ${CLANG_FORMAT}` -i
COMMENT ${CCOMMENT})
else()
add_custom_target(clang-format
COMMAND find ${SRCS} -iname *.h -o -iname *.cpp | xargs ${CLANG_FORMAT} -i
COMMENT ${CCOMMENT})
endif()
unset(SRCS)
unset(CCOMMENT)
endif()

# This function should be passed a list of all files in a target. It will automatically generate
# file groups following the directory hierarchy, so that the layout of the files in IDEs matches the
# one in the filesystem.
function(create_target_directory_groups target_name)
# Place any files that aren't in the source list in a separate group so that they don't get in
# the way.
source_group("Other Files" REGULAR_EXPRESSION ".")

get_target_property(target_sources "${target_name}" SOURCES)

foreach(file_name IN LISTS target_sources)
get_filename_component(dir_name "${file_name}" PATH)
# Group names use '\' as a separator even though the entire rest of CMake uses '/'...
string(REPLACE "/" "\\" group_name "${dir_name}")
source_group("${group_name}" FILES "${file_name}")
endforeach()
endfunction()

# Add all our own sources and headers
# =======================================================================

find_package(ROOT REQUIRED)

add_subdirectory(externals)

add_subdirectory(common)
add_subdirectory(time_sort)
add_subdirectory(tof)
