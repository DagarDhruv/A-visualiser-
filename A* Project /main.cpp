#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Header/Graph_AdjacencyList.h"
#include "Header/NodeData.h"
#include "Header/MazeGenerator.h"
#include "Header/MazeVisualizer.h"

typedef GraphArc<NodeData, int> Arc;
typedef GraphNode<NodeData, int> Node;

void visit( Node * t_node )
{
    std::cout << "Visiting: " << t_node->m_data.m_name << std::endl;
}

int main()
{
    // Create a graph with capacity for 25 nodes.
    // Templates parameters are NodeType (NodeData), ArcType (int)
    // Generate 12x20 maze as required by assignment
    MazeGenerator mazeGen(12, 20);
    mazeGen.generateMaze("/Users/dhruvdagar/Documents/c++/year2/ds25-DagarDhruv/A* Project /Include/nodes.txt", "/Users/dhruvdagar/Documents/c++/year2/ds25-DagarDhruv/A* Project /Include/arcs.txt");
    int startnode=0;
    int endnode=5;

    // Create graph with 240 nodes (12 * 20)
    Graph_AdjacencyList<NodeData, int> graph(240);

    NodeData nodeData;
    int nodeIndex = 0;
    std::ifstream myfile;

    // Create each node, and assign a name (nodeLabel) from the text file
    myfile.open ("/Users/dhruvdagar/Documents/c++/year2/ds25-DagarDhruv/A* Project /Include/nodes.txt");
    while ( myfile >> nodeData.m_name )
    {
        // Set row and col for each node (assuming 5 columns for current test)
        nodeData.row = nodeIndex / 5;
        nodeData.col = nodeIndex % 5;
        graph.addNode(nodeData, nodeIndex++);
    }
    myfile.close();

    // Wire up the graph by creating arcs between nodes.
    myfile.open("/Users/dhruvdagar/Documents/c++/year2/ds25-DagarDhruv/A* Project /Include/arcs.txt");
    int from, to, weight;
    while ( myfile >> from >> to >> weight )
    {
        graph.addArc(from, to, weight);
    }
    myfile.close();

    std::cout << "Searching from node start  to node end" << std::endl;

    // Vector to store the path
    std::vector<Node*> path;

    // Run A* from node 0 to node 19
    graph.aStar(graph.nodeIndex(startnode), graph.nodeIndex(endnode), visit, path);

    // Display the path
    if (!path.empty())
    {
        std::cout << "Path Found" << std::endl;
        std::cout << "Path length: " << path.size() << " nodes" << std::endl;
        std::cout << "Path: ";
        for (int i = 0; i < path.size(); i++)
        {
            std::cout << path[i]->m_data.m_name;
            if (i < path.size() - 1)
            {
                std::cout << " -> ";
            }
        }
        std::cout << std::endl;

        // Calculate total cost
        float totalCost = path.back()->m_data.g;
        std::cout << "Total cost: " << totalCost << std::endl;
    }
    else
    {
        std::cout << "\nNo path found!" << std::endl;
    }
    // Create visualizer
    MazeVisualizer visualizer(&graph);
    visualizer.setStartNode(startnode);
    visualizer.setEndNode(endnode);

    // Run visualization
    visualizer.run();

    std::cout << "\nVisualization closed." << std::endl;


    return 0;
}