cmake_minimum_required(VERSION 3.0.0)
project(ESP8266Game VERSION 0.1.0)
include_directories( ${MY_SOURCE_DIR} )

add_executable(${PROJECT_NAME} 
    game.cpp 
    myfont.cpp 
    gameCommon.cpp 
    myGame.cpp 
    platform_core.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

find_package(OpenAL QUIET)
if (OPENAL_FOUND)
    find_package(OpenAL REQUIRED)
    add_definitions(-DOPENAL_FOUND)
    include_directories(${ALUT_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${ALUT_LIBRARY})
    include_directories(${OPENAL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${OPENAL_LIBRARY})
else (OPENAL_FOUND)
endif (OPENAL_FOUND)

if (WIN32)
    find_package(SDL2 CONFIG)

    if (SDL2_FOUND)
        find_package(SDL2 CONFIG REQUIRED)
        add_definitions(-DSDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2::SDL2 SDL2::SDL2main)
    endif(SDL2_FOUND)
endif (WIN32)

if (UNIX)
    find_package(SDL2 QUIET)
    if (SDL2_FOUND)
        add_definitions(-DSDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    endif (SDL2_FOUND)
    if (NOT SDL2_FOUND)
        find_package(Curses REQUIRED)
        include_directories(${CURSES_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})
    endif (NOT SDL2_FOUND)
endif (UNIX)

include(CPack)
