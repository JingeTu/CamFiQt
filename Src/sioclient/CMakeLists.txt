PROJECT(sioclient)
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_CAPITALIZED)

# CMake settings
cmake_minimum_required(VERSION 2.8.3)
add_definitions(-DUNICODE)

aux_source_directory(. ALL_SRC)
aux_source_directory(./internal ALL_SRC)
file(GLOB ALL_HEADERS ./*.h )

add_library(${PROJECT_NAME} STATIC ${ALL_SRC})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS} 
    . 
    ../../GeneralLibs 
    ../../GeneralLibs/websocketpp
    ../../GeneralLibs/rapidjson/include
)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL ${PROJECT_NAME_CAPITALIZED}_LIB)
set_property(TARGET sioclient PROPERTY CXX_STANDARD 11)
set_property(TARGET sioclient PROPERTY CXX_STANDARD_REQUIRED ON)