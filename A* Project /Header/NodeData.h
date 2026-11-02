#pragma once
#include  <string>
// ---------------------------------------------------------------------
//  Name:           NodeData
//  Description:    A simple struct to encapsulate all the node data.
//                  Add or remove fields as required.
// ---------------------------------------------------------------------
struct NodeData
{
    std::string m_name;
    int m_pathCost;

    // A* algorithm specific fields
    float g;  // Cost from start to this node
    float h;  // Heuristic estimate from this node to goal
    float f;  // Total estimated cost: f = g + h

    // Position in maze grid (for Manhattan distance calculation)
    int row;
    int col;

    // Constructor to initialize A* values
    NodeData() : m_pathCost(0),
                 g(std::numeric_limits<float>::infinity()),
                 h(0.0f),
                 f(std::numeric_limits<float>::infinity()),
                 row(0),
                 col(0) {}
};