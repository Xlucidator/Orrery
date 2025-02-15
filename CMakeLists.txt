cmake_minimum_required(VERSION 3.17)
project(Orrery)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


# options
option(ENABLE_PHYSICS "Enable physics module" ON)

# function define
function(smart_copy category pattern flattened)
    message(STATUS "[smart_copy] Scan \"${pattern}\" files from \"${CMAKE_SOURCE_DIR}/${category}\"... flattened: ${flattened} ")

    file(GLOB_RECURSE files 
        LIST_DIRECTORIES false
        "${CMAKE_SOURCE_DIR}/${category}/${pattern}"
    )

    set(copy_src_list "")
    set(copy_dst_list "")
    foreach(src_file IN LISTS files)
        # prepare paths: dst_path & dst_dir
        if (flattened)
            get_filename_component(rel_path "${src_file}" NAME)
        else()
            file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${src_file}")
        endif()
        set(dst_path "${CMAKE_BINARY_DIR}/${rel_path}") 
        get_filename_component(dst_dir "${dst_path}" DIRECTORY) # dst_path = dst_dir/{file_name}
        file(MAKE_DIRECTORY "${dst_dir}") # ensure directory exists

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


# include & link
include_directories(
	SYSTEM "${CMAKE_SOURCE_DIR}/thirdparty/include"
	"${CMAKE_SOURCE_DIR}/src"
)
link_directories(
	SYSTEM "${CMAKE_SOURCE_DIR}/thirdparty/lib"
)


# execute file
file(GLOB_RECURSE SRC_INNER ${CMAKE_SOURCE_DIR}/src/*.cpp)
message(STATUS "Source files: ${SRC_INNER}")

add_executable(Orrery 
	main.cpp 
	src/glad.c
	${SRC_INNER}
)


# pre-define
target_compile_definitions(Orrery PRIVATE DEBUG) # target_compile_definitions(Orrery PRIVATE $<$<CONFIG:Debug>:DEBUG>)
if(ENABLE_PHYSICS)
    target_compile_definitions(Orrery PRIVATE PHYSIC_IMPL)
endif()


# libs link
file(GLOB ALL_LIBS "${CMAKE_SOURCE_DIR}/thirdparty/lib/*.lib")

set(PHYSX_LIBS "") # glob PhysX libs
foreach(lib ${ALL_LIBS})
    if (lib MATCHES ".*[Pp]hys[Xx].*\\.lib$" OR lib MATCHES ".*PVDRuntime.*\\.lib$")
        list(APPEND PHYSX_LIBS ${lib})
    endif()
endforeach()

set(LIBS ${ALL_LIBS}) # set used libs
if (NOT ENABLE_PHYSICS)
    foreach(physx_lib ${PHYSX_LIBS})
        list(REMOVE_ITEM LIBS ${physx_lib})
    endforeach()
endif()
target_link_libraries(Orrery ${LIBS})
message(STATUS "link libraries: ${LIBS}")


# assets & dlls copy
smart_copy("assets" "*" OFF)
smart_copy("thirdparty/dll" "*.dll" ON)
