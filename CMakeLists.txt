cmake_minimum_required(VERSION 3.5)


project(nanoros LANGUAGES CXX VERSION 0.0.1)
set(CMAKE_USE_RELATIVE_PATHS True)
set(CMAKE_CXX_STANDARD 17)


add_subdirectory(thirdparty)

add_subdirectory(include)
add_subdirectorY(src)

add_subdirectory(apps)
add_subdirectory(examples)
add_subdirectory(basic_stubs)