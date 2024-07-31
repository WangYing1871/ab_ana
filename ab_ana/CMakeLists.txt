cmake_minimum_required(VERSION 3.1...3.5)

project("JW_ab_ana")

option (UNIX_USE_CCACHE "Treat warnings as error" ON)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to use")

find_package(Boost REQUIRED COMPONENTS system timer)
find_package(ROOT REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR}
    )
find_program(ROOTCINT rootcint PATH_SUFFIXES bin REQUIRED)
set(root_cint_out "root_cint_out.cpp")
execute_process(COMMAND "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR} -f ${root_cint_out} -c data_strcut_cint.h LinkDef.h)

#file(COPY ${root_cint_out} DESTINATION ${PROJECT_SOURCE_DIR})

file(GLOB sources 
    ${PROJECT_SOURCE_DIR}/*.cpp
    ${PROJECT_SOURCE_DIR}/*.cxx
    )
file(GLOB headers
    ${PROJECT_SOURCE_DIR}/*.hpp
    ${PROJECT_SOURCE_DIR}/*.h
    ${PROJECT_SOURCE_DIR}/*.hpp
    )

set(exec_name "main")
add_executable(${exec_name} main.cpp ${sources} ${headers})
target_link_libraries(${exec_name} ${ROOT_LIBRARIES} Boost::timer)
