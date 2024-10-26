cmake_minimum_required(VERSION 3.5)

project("Meet" 
    LANGUAGES "CXX"
    DESCRIPTION "Header-only NetWork library ,TCP UDP."
    HOMEPAGE_URL "https://github.com/cngege/Meet"
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

set(Meet_NAME ${PROJECT_NAME})

include("${CMAKE_CURRENT_LIST_DIR}/MeetConfig.cmake")