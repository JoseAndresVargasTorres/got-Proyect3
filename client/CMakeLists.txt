cmake_minimum_required(VERSION 3.8)

project(client)

set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(src
    src/main.cpp
    src/huffman.cpp
    src/md5.cpp
    src/client.cpp
    src/commands.cpp
    src/controlServerCliente.cpp
)

set(head
    include/client.hpp
    include/huffman.hpp
    include/md5.h
    include/commands.hpp
    include/controlServerCliente.hpp
)

include_directories(${PROJECT_SOURCE_DIR}/include/)

#Logging
add_subdirectory(spdlog)
add_subdirectory(cpr)
link_directories(PUBLIC /usr/include/jsoncpp/json/ PUBLIC /usr/include/json/)

add_executable(got ${src} ${head})
set_target_properties(got PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/repo")

target_link_libraries(got spdlog cpr::cpr jsoncpp)
