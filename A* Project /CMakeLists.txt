cmake_minimum_required(VERSION 3.27)
project(A__Project_)

set(CMAKE_CXX_STANDARD 17)
# Find SFML
find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)

# Include directories
include_directories(
        .
        /opt/homebrew/include # General include directory
)

# Link directories
link_directories(
        /opt/homebrew/lib # General library directory
)

add_executable(A__Project_ main.cpp
        Header/Graph_AdjacencyList.h
        Header/NodeData.h
        Src/neighbours.cpp
        Header/GraphNode.h
        Header/GraphArc.h
        Header/MazeGenerator.h
        Header/MazeVisualizer.h
)

target_link_libraries(A__Project_ PRIVATE sfml-graphics sfml-window sfml-system)
