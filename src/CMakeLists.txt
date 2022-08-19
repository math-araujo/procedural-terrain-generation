add_executable(main 
    main.cpp
    application.hpp application.cpp
    mesh.hpp mesh.cpp
    shader.hpp shader.cpp
    image.hpp image.inl image.cpp
    texture.hpp texture.cpp
    noisegeneration.hpp noisegeneration.cpp
)

target_link_libraries(main PRIVATE glad::glad glfw glm::glm imgui::imgui)
target_compile_features(main PRIVATE cxx_std_20)
set_target_properties(main PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(main PRIVATE ${STB_INCLUDE_DIRS})

if (MSVC)
    target_compile_options(main PRIVATE /W3)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy 'data' directory to 'build' directory after build
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_CURRENT_BINARY_DIR}/
)