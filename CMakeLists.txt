cmake_minimum_required(VERSION 3.10)

project(Interp_new VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add shared library
add_library(Interp_new SHARED   
                                lib/global_variable.h
                                lib/global_variable.cpp
                                lib/file_bin_write.h
                                lib/own_array.h
                                lib/own_array.cpp
                                lib/Interp_multithread.h
                                lib/Interp_multithread.cpp
                                lib/NEWINT2_k.H
                                lib/NEWINT2_k.C
)

# Define export macro for shared library
target_compile_definitions(Interp_new PRIVATE MY_LIBRARY_EXPORTS)

# Include directories for the library
target_include_directories(Interp_new PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Add executable application
add_executable(app app.cpp)

# Link executable to the shared library if needed
target_link_libraries(app PRIVATE Interp_new)

if (UNIX)
# Find and link to pthread (POSIX thread library)
find_package(Threads REQUIRED)
target_link_libraries(app PRIVATE ${CMAKE_THREAD_LIBS_INIT})

# Add the shared library `dl` if on Unix-like systems (optional for lopen, lclose ...)
find_library(DL_LIB dl)
target_link_libraries(app PRIVATE ${DL_LIB})
endif()