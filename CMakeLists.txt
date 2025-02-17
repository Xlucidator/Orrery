cmake_minimum_required(VERSION 3.17)
project(Orrery)


# general setting
set(CMAKE_CXX_STANDARD 17)

set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")
set(THIRDPARTY_LIST assimp3 glad glfw glm irrklang stb)

set(PLATFORM_DIR "Windows/MSVC")
set(LIB_SUFFIX ".lib")
set(BIN_SUFFIX ".dll")


# options
option(ENABLE_PHYSICS "Enable physics module" ON)


# os & platform concerned
if(WIN32)
    message(STATUS "Building on Windows")

    if(MSVC)
        set(PLATFORM_DIR "Windows/MSVC")
        set(LIB_SUFFIX ".lib")
        set(BIN_SUFFIX ".dll")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
    if(MINGW)
        set(PLATFORM_DIR "Windows/MinGW")
        set(LIB_SUFFIX ".a")
        set(BIN_SUFFIX ".dll")
    endif()
elseif(UNIX)
    message(STATUS "Building on Unix/Linux")

    set(PLATFORM_DIR "Linux")
    set(LIB_SUFFIX ".a")
    set(BIN_SUFFIX ".so*")
endif()


# function define
## smart copy files in file_list from ${CMAKE_SOURCE_DIR} to {CMAKE_BIN_DIR}
function(smart_copy_files file_list flattened)
    set(copy_src_list "")
    set(copy_dst_list "")
    foreach(src_file IN LISTS file_list)
        # prepare paths
        if (flattened)  # file -> ${CMAKE_BINARY_DIR}/file
            get_filename_component(rel_path "${src_file}" NAME)
        else()          # ${CMAKE_SOURCE_DIR}/rel_path -> ${CMAKE_BIN_DIR}/rel_path
            file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${src_file}")
            #message(STATUS "    rel_path = ${rel_path}")
        endif()
        set(dst_path "${CMAKE_BINARY_DIR}/${rel_path}")         # dst_path
        get_filename_component(dst_dir "${dst_path}" DIRECTORY) # dst_dir
        file(MAKE_DIRECTORY "${dst_dir}") # ensure dst_dir exists

        # check if file needs copy
        set(need_copy FALSE)
        if(NOT EXISTS "${dst_path}")
            set(need_copy TRUE)
        else()
            file(TIMESTAMP "${src_file}" src_time)
            file(TIMESTAMP "${dst_path}" dst_time)
            # copy only if src_time > dst_time
            if(src_time STRGREATER dst_time)
                set(need_copy TRUE)
            endif()
        endif()

        # add to copy list
        if(need_copy)
            list(APPEND copy_src_list "${src_file}")
            list(APPEND copy_dst_list "${dst_dir}")
        endif()
    endforeach()

    # execute copy
    if(copy_src_list)
        message(STATUS "[smart_copy] Start to copy ...")
        foreach(src_file dest_dir IN ZIP_LISTS copy_src_list copy_dst_list) # CMake 3.17+
            file(COPY "${src_file}" DESTINATION "${dest_dir}")
            message(STATUS "    Copy ${src_file} to dir ${dest_dir}/")
        endforeach()
        message(STATUS "[smart_copy] Finish copying files (Updated only)")
    else()
        message(STATUS "[smart_copy] No updated files to copy")
    endif()
endfunction()

## smart copy ${CMAKE_SOURCE_DIR}/${targetdir} to ${CMAKE_BINARY_DIR} with struct
function(smart_copy_dir tar_dir)
    message(STATUS "[smart_copy] copy directory \"${CMAKE_SOURCE_DIR}/${tar_dir}\", keep inner struct.")
    file(GLOB_RECURSE file_list LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/${tar_dir}/*")
    smart_copy_files("${file_list}" OFF)
endfunction()

## smart copy file in file_lists to ${CMAKE_BINARY_DIR}, without struct
function(smart_copy file_list)
    message(STATUS "[smart_copy] copy files from \"${file_list}\", without struct.")
    smart_copy_files("${file_list}" ON)
endfunction()

# execute file
file(GLOB_RECURSE SRC_INNER ${CMAKE_SOURCE_DIR}/src/*.cpp)
message(STATUS "Source files: ${SRC_INNER}")
add_executable(Orrery 
	main.cpp 
	${THIRDPARTY_DIR}/glad/glad.c
	${SRC_INNER}
)


# option set & predefine
target_compile_definitions(Orrery PRIVATE DEBUG) # target_compile_definitions(Orrery PRIVATE $<$<CONFIG:Debug>:DEBUG>)
if(ENABLE_PHYSICS)
    target_compile_definitions(Orrery PRIVATE PHYSIC_IMPL)
    list(APPEND THIRDPARTY_LIST physx)
endif()


# include & link & binary
set(LINK_LIBS "")
set(USED_BINS "")
target_include_directories(Orrery SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/src")
foreach(category IN LISTS THIRDPARTY_LIST)
    target_include_directories(Orrery SYSTEM PRIVATE "${THIRDPARTY_DIR}/${category}/include")

    set(LIB_DIR "${THIRDPARTY_DIR}/${category}/lib/${PLATFORM_DIR}")
    if(EXISTS ${LIB_DIR})
        target_link_directories(Orrery PRIVATE "${LIB_DIR}")
        file(GLOB libfiles "${LIB_DIR}/*${LIB_SUFFIX}")
        list(APPEND LINK_LIBS ${libfiles})
    endif()

    set(BIN_DIR "${THIRDPARTY_DIR}/${category}/bin/${PLATFORM_DIR}")
    if(EXISTS ${BIN_DIR})
        file(GLOB binfiles "${BIN_DIR}/*${BIN_SUFFIX}")
        list(APPEND USED_BINS ${binfiles})
    endif()
endforeach()
target_link_libraries(Orrery ${LINK_LIBS})
message(STATUS "link libraries: ${LINK_LIBS}")
message(STATUS "used binaries : ${USED_BINS}")

# assets & dlls copy
smart_copy_dir("assets")
smart_copy("${USED_BINS}")

#smart_copy("assets" "*" OFF)
#smart_copy("thirdparty/bin/Windows" "*.dll" ON)
