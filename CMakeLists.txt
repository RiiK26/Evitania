cmake_minimum_required(VERSION 3.20)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/config.json" VERSION_JSON_CONTENT)
string(JSON PARSED_VERSION GET "${VERSION_JSON_CONTENT}" "version")
string(JSON PARSED_PROJECT_NAME GET "${VERSION_JSON_CONTENT}" "project_name")

project(
    ${PARSED_PROJECT_NAME}
    VERSION ${PARSED_VERSION}
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(BUILD_CLEAN "Build clean binary with PDB symbols" OFF)
option(BUILD_INJECTOR "Build the injector executable" ON)

if(BUILD_CLEAN)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
    if(MSVC)
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Zi")
    else()
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
    endif()
else()
    set(CMAKE_BUILD_TYPE Release)
    if(MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-s")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release")

add_compile_definitions(
    PROJECT_VERSION=\"${PROJECT_VERSION}\"
    PROJECT_ROOT=\"${CMAKE_BINARY_DIR}/release\"
    NDEBUG # Disables asserts, hiding ImGui file paths
)
if(NOT MSVC)
    add_compile_options(-fpermissive -ffunction-sections -fdata-sections)
    add_link_options(-Wl,--gc-sections)
endif()

file(GLOB MINHOOK_SRC "src/Modules/MinHook/*.c" "src/Modules/MinHook/hde/*.c")
file(GLOB SRC_FILES "src/Cores/dll.cpp" "src/Cores/Scanner.cpp" "src/Modules/Hooks/*.cpp" "src/Modules/Menu/*.cpp" "src/Features/*/*.cpp")

add_library(Evitania MODULE
    ${SRC_FILES}
    ${MINHOOK_SRC}
    src/Modules/ImGui/imgui.cpp
    src/Modules/ImGui/imgui_draw.cpp
    src/Modules/ImGui/imgui_widgets.cpp
    src/Modules/ImGui/imgui_tables.cpp
    src/Modules/ImGui/imgui_impl_win32.cpp
    src/Modules/ImGui/imgui_impl_dx11.cpp
    src/Modules/ImGui/imgui_impl_dx12.cpp
)

# Build the injector separately
if(BUILD_INJECTOR)
    add_executable(injector WIN32 src/Cores/injector.cpp)
    set_target_properties(injector PROPERTIES SUFFIX ".exe")
    if(NOT MSVC)
        target_link_options(injector PRIVATE "-static" "-static-libgcc" "-static-libstdc++")
    endif()
endif()

target_include_directories(Evitania PRIVATE
    src/Modules/ImGui
    src/Modules/MinHook
    src/Modules/MinHook/include
    src
    src/Modules
    src/Features
)

set_target_properties(Evitania PROPERTIES PREFIX "" SUFFIX ".dll" OUTPUT_NAME "${PARSED_PROJECT_NAME} v${PARSED_VERSION}")
if(NOT MSVC)
    target_link_options(Evitania PRIVATE "-static" "-static-libgcc" "-static-libstdc++")
endif()
target_link_libraries(Evitania PRIVATE d3d11 d3d12 d3dcompiler dxgi gdi32 dwmapi)

if(NOT BUILD_CLEAN)
    if(NOT MSVC)
        target_link_options(Evitania PRIVATE "-s")
        # target_link_options(injector PRIVATE "-s") No need injector
    endif()
endif()
