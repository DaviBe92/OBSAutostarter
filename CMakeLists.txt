cmake_minimum_required(VERSION 3.16...3.26)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets Core)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
  target_compile_options(
    ${CMAKE_PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header
                                  -Wno-comma>)
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON
               AUTOUIC ON
               AUTORCC ON)
endif()

set(CMAKE_AUTORCC ON)

target_sources(
  ${CMAKE_PROJECT_NAME}
  PRIVATE src/plugin-main.cpp
          src/launch-widget.cpp
          src/launch-widget.hpp
          src/settings-widget.cpp
          src/settings-widget.hpp
          src/config.cpp
          src/config.hpp
          src/autostart.cpp
          src/autostart.hpp)

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})

# set_target_properties(
#   ${CMAKE_PROJECT_NAME}
#   PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY_DEBUG "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     RUNTIME_OUTPUT_DIRECTORY_RELEASE "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     LIBRARY_OUTPUT_DIRECTORY_DEBUG "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     LIBRARY_OUTPUT_DIRECTORY_RELEASE "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     ARCHIVE_OUTPUT_DIRECTORY_DEBUG "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     ARCHIVE_OUTPUT_DIRECTORY_RELEASE "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
#     ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "D:/Projects/Programming/OBS-Studio/obs-plugins/64bit"
# )

# add_custom_command(
#   TARGET ${CMAKE_PROJECT_NAME}
#   POST_BUILD
#   COMMAND cmd /c start /B "" "D:/Projects/Programming/OBS-Studio/bin/64bit/obs64.exe" >NUL 2>&1
#   WORKING_DIRECTORY "D:/Projects/Programming/OBS-Studio/bin/64bit"
#   COMMENT "Starting OBS after build"
# )