if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++ -lc++abi")
endif()

if (CMAKE_COMPILER_IS_GNUCC)
  message(STATUS "Using gcc ${CMAKE_CXX_COMPILER_VERSION}")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
    message(STATUS "USE CXX11_ABI")
    add_definitions("-D_GLIBCXX_USE_CXX11_ABI=1")
  endif()
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-strict-aliasing")
  endif()
endif()

check_unsupported_compiler_version()

add_definitions(-DTANGRAM_LINUX)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(APP_FRAMEWORK "GLFW" CACHE STRING "Application Framework. GLFW or SDL")
set(APP_FRAMEWORK "SDL")
if(APP_FRAMEWORK STREQUAL "GLFW")
  message(STATUS "Use GLFW application framework")
  include(cmake/glfw.cmake)
  set(APP_FRAMEWORK_SOURCES platforms/common/imgui_impl_glfw.cpp
      platforms/common/imgui_impl_opengl3.cpp
      platforms/common/glfwApp.cpp)
  set(APP_FRAMEWORK_LIBS glfw imgui)
  add_subdirectory(platforms/common/imgui)
  set(APP_FRAMEWORK_DEFINE "USE_GLFW")
else()
  message(STATUS "Use SDL application framework")
  set(APP_FRAMEWORK_SOURCES platforms/common/SDLApp.cpp)
  set(APP_FRAMEWORK_LIBS SDL2)
  set(APP_FRAMEWORK_DEFINE "USE_SDL")
endif()

# System font config
include(FindPkgConfig)
pkg_check_modules(FONTCONFIG REQUIRED "fontconfig")

find_package(CURL REQUIRED)

add_executable(tangram
  platforms/linux/src/linuxPlatform.cpp
  platforms/linux/src/main.cpp
  platforms/common/App.cpp
  platforms/common/platform_gl.cpp
  platforms/common/urlClient.cpp
  platforms/common/linuxSystemFontHelper.cpp
  ${APP_FRAMEWORK_SOURCES}
)

target_include_directories(tangram
  PRIVATE
  platforms/common
  ${FONTCONFIG_INCLUDE_DIRS}
)

target_compile_definitions(tangram PRIVATE ${APP_FRAMEWORK_DEFINE})
target_link_libraries(tangram
  PRIVATE
  tangram-core
  ${APP_FRAMEWORK_LIBS}
  ${OPENGL_LIBRARIES}
  ${FONTCONFIG_LDFLAGS}
  ${CURL_LIBRARIES}
  -pthread
  # only used when not using external lib
  -ldl
)

target_compile_options(tangram
  PRIVATE
  -std=c++1y
  -fno-omit-frame-pointer
  -Wall
  -Wreturn-type
  -Wsign-compare
  -Wignored-qualifiers
  -Wtype-limits
  -Wmissing-field-initializers
)

get_nextzen_api_key(NEXTZEN_API_KEY)
target_compile_definitions(tangram
  PRIVATE
  NEXTZEN_API_KEY="${NEXTZEN_API_KEY}")


add_resources(tangram "${PROJECT_SOURCE_DIR}/scenes" "res")
