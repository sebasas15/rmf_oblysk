cmake_minimum_required(VERSION 3.22)
project(oblysk_dispatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rmf_task_msgs REQUIRED)
find_package(nlohmann_json REQUIRED)

# Strict warnings on oblysk code only; ROS headers are not ours to hold to -Werror.
set(OBLYSK_STRICT -Wall -Wextra -Wpedantic -Werror)

add_library(oblysk_dispatch_accept src/accept.cpp)
target_include_directories(oblysk_dispatch_accept PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(oblysk_dispatch_accept PUBLIC nlohmann_json::nlohmann_json)
target_compile_options(oblysk_dispatch_accept PRIVATE ${OBLYSK_STRICT})

add_executable(dispatch_node src/dispatch_node.cpp)
target_link_libraries(dispatch_node oblysk_dispatch_accept rclcpp::rclcpp ${rmf_task_msgs_TARGETS})
target_compile_options(dispatch_node PRIVATE -Wall -Wextra)

install(TARGETS dispatch_node DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_accept test/test_accept.cpp)
  target_link_libraries(test_accept oblysk_dispatch_accept)
  target_compile_options(test_accept PRIVATE ${OBLYSK_STRICT})
endif()

ament_package()
