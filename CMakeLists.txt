cmake_minimum_required(VERSION 3.9)
project(redisDemo)

ADD_DEFINITIONS(-DJSON_IS_AMALGAMATION)

set(CMAKE_CXX_STANDARD 11)

if (UNIX)
    unset(source_files)

    SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fpermissive")


    set(srcdirs
            src/json/
            src/redisClient/
            .)

    include_directories(
            includes/
            src/
    )

    foreach(srcdir ${srcdirs})
        aux_source_directory(${srcdir} source_files)
    endforeach()

    link_directories(
            libs/hiredis/
            .
    )
    message("on unix platfrom")
    add_executable(redisDemo ${source_files})
    target_link_libraries(redisDemo libhiredis.a pthread)

elseif(WIN32)
    message("on windows platfrom")
    unset(source_files)

    set(srcdirs
            src/json/
            src/redisClient/
            src/Win32_Interop/
            #src/win_stdafx/
            .)

    include_directories(
            includes/
            #includes/hiredis-win/
            #includes/mqtt-win/
            src/
    )

    foreach(srcdir ${srcdirs})
        aux_source_directory(${srcdir} source_files)
    endforeach()

    link_directories(
            libs/hiredis-win/
            .
    )
    add_executable(redisDemo ${source_files})
    target_link_libraries(redisDemo hiredis)

else()
    message("on a unsupport platfrom")
endif ()