add_library(glad ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include)