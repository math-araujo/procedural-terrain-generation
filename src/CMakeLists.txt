add_executable(main main.cpp)
target_link_libraries(main PRIVATE glad glfw glm::glm imgui::imgui)

if (MSVC)
    target_compile_options(main PRIVATE /W3)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()