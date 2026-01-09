cmake_minimum_required(VERSION 3.15)
project(CharlieCoin LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectory
add_subdirectory(apps)
