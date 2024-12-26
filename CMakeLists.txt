cmake_minimum_required(VERSION 3.12)
project(EscapeDungeon)
set(CMAKE_CXX_STANDARD 17)

# pre-define
add_definitions(-DDEBUG)

# include & link
include_directories(
	SYSTEM "${CMAKE_SOURCE_DIR}/thirdparty/include"
	"${CMAKE_SOURCE_DIR}/src"
)
link_directories(
	SYSTEM "${CMAKE_SOURCE_DIR}/thirdparty/lib"
)

aux_source_directory(. MAIN)
file(GLOB_RECURSE SRC_INNER ${CMAKE_SOURCE_DIR}/src/*.cpp)
message(STATUS "Source files: ${SRC_INNER}")

add_executable(EscapeDungeon 
	${MAIN} 
	${SRC_INNER}
)
target_link_libraries(EscapeDungeon glfw3.lib)

# assets
file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})


# libs
# file(GLOB LIBS "./libs/*.cpp")
# add_library(EscapeDungeon STATIC ${LIBS})
# target_include_directories(libs PUBLIC "./libs")
# target_link_libraries(EscapeDungeon PRIVATE libs)
