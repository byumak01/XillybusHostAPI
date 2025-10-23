cmake_minimum_required(VERSION 3.16)
project(myapi VERSION 1.0.0 LANGUAGES CXX)

# Force static linking
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)

# Source files
set(API_SOURCES
    src/main.cpp
    src/Server.cpp
    src/XillybusManager.cpp
    # Add other source files here
)

# Header files (for IDE support)
set(API_HEADERS
        include/Server.h
        include/Task.h
        include/TaskPool.h
        include/XillybusManager.h
        include/headers.h
        src/TaskPool.cpp
        src/Task.cpp
        # Add other header files here
)

# Include directories
include_directories(include)

find_package(Boost REQUIRED)

# fetch content
include(FetchContent)

# spdlog
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG ce0424bb375805955c0c5482a32ddb821708eb47
)
FetchContent_MakeAvailable(spdlog)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Binaries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")


# Create the executable
add_executable(${PROJECT_NAME} ${API_SOURCES} ${API_HEADERS})

target_include_directories(myapi PRIVATE ${Boost_INCLUDE_DIRS})

# Target link libraries after executable and find package etc. lines.
target_link_libraries(${PROJECT_NAME} PRIVATE
        spdlog::spdlog 
        Boost::boost
)

target_link_options(${PROJECT_NAME} PRIVATE -static)
