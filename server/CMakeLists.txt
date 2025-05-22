cmake_minimum_required(VERSION 3.14)
project(FillerWebSocketServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to uWebSockets
set(UWS_PATH "$ENV{HOME}/dev/libs/uWebSockets")
set(USOCKETS_PATH "${UWS_PATH}/uSockets")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --------------------------------------------------------------------------------------------------
# Step 1: Build libusockets.a
file(GLOB USOCKETS_SRC
    "${USOCKETS_PATH}/src/*.c"
    "${USOCKETS_PATH}/src/eventing/*.c"
    "${USOCKETS_PATH}/src/crypto/*.c"
    "${USOCKETS_PATH}/src/io_uring/*.c"
)

add_custom_command(
    OUTPUT ${USOCKETS_PATH}/libusockets.a
    COMMAND ${CMAKE_C_COMPILER} -std=c11 -DLIBUS_NO_SSL -O3 -flto -Isrc -I${USOCKETS_PATH}/src -c ${USOCKETS_SRC}
    COMMAND ar rcs libusockets.a *.o
    WORKING_DIRECTORY ${USOCKETS_PATH}
    COMMENT "Building libusockets.a from uSockets"
)

add_custom_target(usockets_build ALL
    DEPENDS ${USOCKETS_PATH}/libusockets.a
)

# --------------------------------------------------------------------------------------------------
# Step 2: Build your server

# Your source files
add_executable(filler_server
    src/main.cpp
    src/game_logic.cpp
)

# Include paths
target_include_directories(filler_server PRIVATE
    ${UWS_PATH}
    ${UWS_PATH}/src
    ${USOCKETS_PATH}/src
)

# uWebSockets source files
file(GLOB UWS_SRC
    ${UWS_PATH}/src/*.cpp
)

target_sources(filler_server PRIVATE ${UWS_SRC})

# Compile definitions
target_compile_definitions(filler_server PRIVATE UWS_NO_ZLIB=0)

# Dependencies
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

# Ensure server builds after usockets
add_dependencies(filler_server usockets_build)

# Link libraries
target_link_libraries(filler_server
    ${USOCKETS_PATH}/libusockets.a
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    Threads::Threads
)
