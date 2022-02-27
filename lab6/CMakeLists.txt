cmake_minimum_required(VERSION 3.10)
project(os_lab_06)
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig)
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS})
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS})

add_library(api lab_api.cpp lab_api.hpp)
target_link_libraries(api PUBLIC ${ZeroMQ_LIBRARY})
target_include_directories(api PUBLIC ${ZeroMQ_INCLUDE_DIR})

add_library(tree tree.cpp tree.hpp)

add_executable(worker worker.cpp)
target_include_directories(worker PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(worker PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(worker PUBLIC api)

add_executable(manager manager.cpp)
target_include_directories(manager PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(manager PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(manager PUBLIC api)
target_link_libraries(manager PUBLIC tree)
