PROJECT(CamFiSocket)
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_CAPITALIZED)

# CMake settings
cmake_minimum_required(VERSION 2.8.3)
add_definitions(-DUNICODE)

FILE(GLOB_RECURSE HDRS_FILES *.h *.hpp)
FILE(GLOB_RECURSE SRCS_FILES *.cpp)

add_library(${PROJECT_NAME} STATIC ${HDRS_FILES} ${SRCS_FILES})
target_link_libraries(${PROJECT_NAME} sioclient ${Qt5Core_LIBRARIES} ${Qt5Network_LIBRARIES})

add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Network_DEFINITIONS})

qt5_use_modules(${PROJECT_NAME} Core Network)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL ${PROJECT_NAME_CAPITALIZED}_LIB)