if(NOT DEFINED Meet_NAME)
    set(Meet_NAME "Meet")
endif()


# Meet::Meet is header-only:
add_library(${Meet_NAME} INTERFACE)

target_include_directories(${Meet_NAME} INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/include/"
)

add_library("${Meet_NAME}::Meet" ALIAS ${Meet_NAME})

