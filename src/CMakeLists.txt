add_executable(main 
    main.cpp
    application.hpp application.cpp
    mesh.hpp mesh.cpp
    shader.hpp shader.cpp
)

target_link_libraries(main PRIVATE glad::glad glfw glm::glm imgui::imgui)
target_include_directories(main PRIVATE ${STB_INCLUDE_DIRS})

if (MSVC)
    target_compile_options(main PRIVATE /W3)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()