cmake_minimum_required(VERSION 3.5)

project(ssdp_pinging LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ping main.cpp classping.cpp classping.h)

install(TARGETS ping
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
find_package(CURL REQUIRED)

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(ping ${CURL_LIBRARIES})

include_directories(${XML2JSON_INCLUDE_DIR})
target_link_libraries(ping ${XML2JSON_LIBRARIES})




