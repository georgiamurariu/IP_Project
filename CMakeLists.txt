cmake_minimum_required(VERSION 3.20)
project(Project_PI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pe macOS cu Homebrew, OpenCV este de obicei aici.
# Daca OpenCV este instalat in alta parte, comenteaza linia sau schimba path-ul.
set(OpenCV_DIR "/opt/homebrew/opt/opencv/lib/cmake/opencv4")

find_package(OpenCV REQUIRED)

add_executable(Project_PI
        src/main.cpp
        src/ImageLoader.cpp
        src/ImageLoader.h
        src/AnaglyphGenerator.cpp
        src/AnaglyphGenerator.h
        src/DisparityMap.cpp
        src/DisparityMap.h
        src/VideoGenerator.cpp
        src/VideoGenerator.h
)

target_include_directories(Project_PI PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Project_PI PRIVATE ${OpenCV_LIBS})
target_compile_definitions(Project_PI PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
