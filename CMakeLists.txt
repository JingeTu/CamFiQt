PROJECT(CamFiQt)

# CMake settings
cmake_minimum_required(VERSION 2.8.3)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

option(Boost_USE_STATIC_LIBS "Use Boost static version" ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(Boost_USE_MULTITHREADED ON) 
set(Boost_USE_STATIC_RUNTIME OFF) 
set(BOOST_VER "1.55.0" CACHE STRING "boost version" )
find_package(Boost ${BOOST_VER} REQUIRED COMPONENTS system date_time random) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories(${CMAKE_SOURCE_DIR}/Src/CamFiSocket)
include_directories(${CMAKE_SOURCE_DIR}/Src/sioclient)
# include_directories(${Qt5Core_INCLUDES})
# include_directories(${Qt5Gui_INCLUDES})
# include_directories(${Qt5Network_INCLUDES})


add_subdirectory(Src)
add_subdirectory(Examples)