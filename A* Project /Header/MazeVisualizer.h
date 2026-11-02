// MazeVisualizer.h
// Multi-algorithm visualization: A* (A), Breadth-First (B), Depth-First (D)


#ifndef MAZE_VISUALIZER_H
#define MAZE_VISUALIZER_H

#include <SFML/Graphics.hpp>
#include "Graph_AdjacencyList.h"
#include "NodeData.h"
#include <set>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

typedef GraphArc<NodeData, int> Arc;
typedef GraphNode<NodeData, int> Node;

class MazeVisualizer
{
private:
    // Maze dimensions
    const int ROWS = 12;
    const int COLS = 20;
    const int CELL_SIZE = 35;
    const int WALL_THICKNESS = 3;
    const int HUD_WIDTH = 300;
    const int ANIMATION_DELAY_MS = 30;

    // Window dimensions
    int windowWidth;
    int windowHeight;
    int mazeOffsetX;
    int mazeOffsetY;

    // SFML window and font
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;

    // Graph reference
    Graph_AdjacencyList<NodeData, int>* graph;

    // Node positions
    int startNode;
    int endNode;
    bool startNodeSet;
    bool endNodeSet;

    // Visualization state
    std::set<int> openSetNodes;
    int currentExpandingNode;
    std::vector<int> pathNodes;
    int searchLength;
    int pathLength;
    bool isSearching;
    bool searchComplete;

    // Algorithm tracking
    std::string currentAlgorithm;

    // Colors
    sf::Color colorEmpty;
    sf::Color colorWall;
    sf::Color colorStart;
    sf::Color colorEnd;
    sf::Color colorOpenSet;
    sf::Color colorCurrentNode;
    sf::Color colorPath;

public:
    MazeVisualizer(Graph_AdjacencyList<NodeData, int>* g)
            : graph(g), startNode(-1), endNode(-1),
              startNodeSet(false), endNodeSet(false),
              currentExpandingNode(-1), searchLength(0), pathLength(0),
              isSearching(false), searchComplete(false),
              currentAlgorithm("None")
    {
        // Calculate window size
        mazeOffsetX = 20;
        mazeOffsetY = 20;
        windowWidth = COLS * CELL_SIZE + HUD_WIDTH + mazeOffsetX * 2;
        windowHeight = ROWS * CELL_SIZE + mazeOffsetY * 2;

        // Create window
        window.create(sf::VideoMode(windowWidth, windowHeight),
                      "Pathfinding Algorithm Comparison");
        window.setFramerateLimit(60);

        // Initialize colors
        colorEmpty = sf::Color::White;
        colorWall = sf::Color(40, 40, 40);
        colorStart = sf::Color(0, 255, 0);
        colorEnd = sf::Color(255, 0, 0);
        colorOpenSet = sf::Color(135, 206, 250);
        colorCurrentNode = sf::Color(255, 215, 0);
        colorPath = sf::Color(255, 105, 180);

        // Try to load font
        std::vector<std::string> fontPaths = {
                "/Users/dhruvdagar/Documents/c++/year2/ds25-DagarDhruv/A* Project /Include/AldotheApache.ttf",
        };

        for (const auto& path : fontPaths)
        {
            if (font.loadFromFile(path))
            {
                fontLoaded = true;
                break;
            }
        }

        if (!fontLoaded)
        {
            std::cout << "Warning: Could not load font." << std::endl;
        }
    }

    bool hasArc(int from, int to)
    {
        if (from < 0 || to < 0 || from >= 240 || to >= 240)
        {
            return false;
        }
        Arc* arc = graph->getArc(from, to);
        return (arc != nullptr);
    }

    int getNodeIndex(Node* node)
    {
        for (int i = 0; i < 240; i++)
        {
            if (graph->nodeIndex(i) == node)
            {
                return i;
            }
        }
        return -1;
    }

    void drawMaze()
    {
        // Draw all cells
        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                int nodeIdx = row * COLS + col;

                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(col * CELL_SIZE + mazeOffsetX, row * CELL_SIZE + mazeOffsetY);

                // Cell color
                if (nodeIdx == startNode)
                {
                    cell.setFillColor(colorStart);
                }
                else if (nodeIdx == endNode)
                {
                    cell.setFillColor(colorEnd);
                }
                else if (std::find(pathNodes.begin(), pathNodes.end(), nodeIdx) != pathNodes.end())
                {
                    cell.setFillColor(colorPath);
                }
                else if (nodeIdx == currentExpandingNode)
                {
                    cell.setFillColor(colorCurrentNode);
                }
                else if (openSetNodes.find(nodeIdx) != openSetNodes.end())
                {
                    cell.setFillColor(colorOpenSet);
                }
                else
                {
                    cell.setFillColor(colorEmpty);
                }

                cell.setOutlineColor(sf::Color(200, 200, 200));
                cell.setOutlineThickness(1);
                window.draw(cell);

                // Draw walls
                if (row > 0 && !hasArc(nodeIdx, nodeIdx - COLS))
                {
                    sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, WALL_THICKNESS));
                    wall.setPosition(col * CELL_SIZE + mazeOffsetX, row * CELL_SIZE + mazeOffsetY);
                    wall.setFillColor(colorWall);
                    window.draw(wall);
                }

                if (col > 0 && !hasArc(nodeIdx, nodeIdx - 1))
                {
                    sf::RectangleShape wall(sf::Vector2f(WALL_THICKNESS, CELL_SIZE));
                    wall.setPosition(col * CELL_SIZE + mazeOffsetX, row * CELL_SIZE + mazeOffsetY);
                    wall.setFillColor(colorWall);
                    window.draw(wall);
                }

                // Borders
                if (row == 0)
                {
                    sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, WALL_THICKNESS));
                    wall.setPosition(col * CELL_SIZE + mazeOffsetX, mazeOffsetY);
                    wall.setFillColor(colorWall);
                    window.draw(wall);
                }
                if (col == 0)
                {
                    sf::RectangleShape wall(sf::Vector2f(WALL_THICKNESS, CELL_SIZE));
                    wall.setPosition(mazeOffsetX, row * CELL_SIZE + mazeOffsetY);
                    wall.setFillColor(colorWall);
                    window.draw(wall);
                }
                if (row == ROWS - 1)
                {
                    sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, WALL_THICKNESS));
                    wall.setPosition(col * CELL_SIZE + mazeOffsetX,
                                     (row + 1) * CELL_SIZE + mazeOffsetY - WALL_THICKNESS);
                    wall.setFillColor(colorWall);
                    window.draw(wall);
                }
                if (col == COLS - 1)
                {
                    sf::RectangleShape wall(sf::Vector2f(WALL_THICKNESS, CELL_SIZE));
                    wall.setPosition((col + 1) * CELL_SIZE + mazeOffsetX - WALL_THICKNESS,
                                     row * CELL_SIZE + mazeOffsetY);
                    wall.setFillColor(colorWall);
                    window.draw(wall);
                }
            }
        }
    }

    void drawHUD()
    {
        if (!fontLoaded) return;

        int hudX = COLS * CELL_SIZE + mazeOffsetX + 20;
        int textY = 20;

        // Instructions
        sf::Text instructions;
        instructions.setFont(font);
        instructions.setCharacterSize(14);
        instructions.setFillColor(sf::Color::Black);
        instructions.setPosition(hudX, textY);

        std::ostringstream instrText;
        instrText << "Instructions:\n\n";

        if (!startNodeSet)
        {
            instrText << "Click grid to set\nSTART (green)\n\n";
        }
        else if (!endNodeSet)
        {
            instrText << "Click grid to set\nEND (red)\n\n";
        }
        else
        {
            instrText << "Press A = A* Search\n";
            instrText << "Press B = BFS\n";
            instrText << "Press D = DFS\n";
            instrText << "Press R = Reset\n\n";
        }

        instructions.setString(instrText.str());
        window.draw(instructions);

        textY += 140;

        // Node info
        if (startNodeSet)
        {
            sf::Text startInfo;
            startInfo.setFont(font);
            startInfo.setCharacterSize(13);
            startInfo.setFillColor(sf::Color::Black);
            startInfo.setPosition(hudX, textY);

            std::ostringstream startText;
            startText << "Start: " << startNode;
            startText << " (" << startNode / COLS << "," << startNode % COLS << ")";

            startInfo.setString(startText.str());
            window.draw(startInfo);
            textY += 30;
        }

        if (endNodeSet)
        {
            sf::Text endInfo;
            endInfo.setFont(font);
            endInfo.setCharacterSize(13);
            endInfo.setFillColor(sf::Color::Black);
            endInfo.setPosition(hudX, textY);

            std::ostringstream endText;
            endText << "End: " << endNode;
            endText << " (" << endNode / COLS << "," << endNode % COLS << ")";

            endInfo.setString(endText.str());
            window.draw(endInfo);
            textY += 50;
        }

        // Algorithm name and statistics
        if (searchComplete)
        {
            // Algorithm name with highlight
            sf::Text algoName;
            algoName.setFont(font);
            algoName.setCharacterSize(16);
            algoName.setFillColor(sf::Color(200, 0, 0));
            algoName.setPosition(hudX, textY);
            algoName.setStyle(sf::Text::Bold);
            algoName.setString("Algorithm: " + currentAlgorithm);
            window.draw(algoName);
            textY += 35;

            sf::Text stats;
            stats.setFont(font);
            stats.setCharacterSize(14);
            stats.setFillColor(sf::Color::Black);
            stats.setPosition(hudX, textY);

            std::ostringstream statsText;
            statsText << "Nodes Explored: " << searchLength << "\n\n";
            statsText << "Path Length: " << pathLength;

            stats.setString(statsText.str());
            window.draw(stats);
            textY += 80;
        }

        // Legend
        sf::Text legend;
        legend.setFont(font);
        legend.setCharacterSize(11);
        legend.setFillColor(sf::Color::Black);
        legend.setPosition(hudX, textY);

        std::ostringstream legendText;
        legendText << "Color:\n";
        legendText << "Green = Start\n";
        legendText << "Red = End\n";
        legendText << "Light Blue = Explored\n";
        legendText << "Gold = Current\n";
        legendText << "Pink = Path";

        legend.setString(legendText.str());
        window.draw(legend);
    }

    void reset()
    {
        openSetNodes.clear();
        currentExpandingNode = -1;
        pathNodes.clear();
        searchLength = 0;
        pathLength = 0;
        searchComplete = false;
        currentAlgorithm = "None";
    }

    void resetAll()
    {
        reset();
        startNodeSet = false;
        endNodeSet = false;
        startNode = -1;
        endNode = -1;
    }

    void runAStarWithVisualization()
    {
        if (!startNodeSet || !endNodeSet)
        {
            std::cout << "Please set both start and end nodes!" << std::endl;
            return;
        }

        std::cout << "\n=== Running A* Algorithm ===" << std::endl;
        std::cout << "Start: " << startNode << ", Goal: " << endNode << std::endl;

        reset();
        currentAlgorithm = "A* Completed";
        isSearching = true;

        Node* start = graph->nodeIndex(startNode);
        Node* dest = graph->nodeIndex(endNode);
        std::vector<Node*> path;

        auto visitFunc = [this](Node* node)
        {
            int nodeIdx = this->getNodeIndex(node);

            if (nodeIdx != -1)
            {
                this->currentExpandingNode = nodeIdx;
                this->searchLength++;

                auto arcIter = node->arcList().begin();
                auto arcEnd = node->arcList().end();

                for (; arcIter != arcEnd; ++arcIter)
                {
                    Node* neighbor = (*arcIter).destNode();
                    int neighborIdx = this->getNodeIndex(neighbor);
                    if (neighborIdx != -1)
                    {
                        this->openSetNodes.insert(neighborIdx);
                    }
                }
            }

            window.clear(sf::Color::White);
            this->drawMaze();
            this->drawHUD();
            window.display();

            std::this_thread::sleep_for(std::chrono::milliseconds(this->ANIMATION_DELAY_MS));
        };

        graph->aStar(start, dest, visitFunc, path);

        pathNodes.clear();
        for (Node* n : path)
        {
            int idx = getNodeIndex(n);
            if (idx != -1) pathNodes.push_back(idx);
        }

        pathLength = pathNodes.size();
        isSearching = false;
        searchComplete = true;
        currentExpandingNode = -1;

        window.clear(sf::Color::White);
        drawMaze();
        drawHUD();
        window.display();

        std::cout << "A* Complete - Nodes: " << searchLength << ", Path: " << pathLength << std::endl;
    }

    void runBreadthFirstWithVisualization()
    {
        if (!startNodeSet || !endNodeSet)
        {
            std::cout << "Please set both start and end nodes!" << std::endl;
            return;
        }

        std::cout << "\n=== Running Breadth-First Search ===" << std::endl;
        std::cout << "Start: " << startNode << ", Goal: " << endNode << std::endl;

        reset();
        currentAlgorithm = "BFS Completed";
        isSearching = true;

        Node* start = graph->nodeIndex(startNode);
        Node* dest = graph->nodeIndex(endNode);

        auto visitFunc = [this, dest](Node* node)
        {
            int nodeIdx = this->getNodeIndex(node);

            if (nodeIdx != -1)
            {
                this->currentExpandingNode = nodeIdx;
                this->openSetNodes.insert(nodeIdx);
                this->searchLength++;

                window.clear(sf::Color::White);
                this->drawMaze();
                this->drawHUD();
                window.display();

                std::this_thread::sleep_for(std::chrono::milliseconds(this->ANIMATION_DELAY_MS));
            }
        };

        // Visualize the traversal with visit function FIRST (no path shown yet)
        graph->clearMarks();
        graph->breadthFirst(start, visitFunc);

        // AFTER visualization, run adapted BFS to build path
        graph->clearMarks();
        graph->adaptedBreadthFirst(start, dest);

        // Build path from previous pointers
        pathNodes.clear();
        Node* current = dest;
        while (current != nullptr)
        {
            int idx = getNodeIndex(current);
            if (idx != -1) pathNodes.push_back(idx);
            current = current->previous();
        }
        std::reverse(pathNodes.begin(), pathNodes.end());

        pathLength = pathNodes.size();
        isSearching = false;
        searchComplete = true;
        currentExpandingNode = -1;

        window.clear(sf::Color::White);
        drawMaze();
        drawHUD();
        window.display();

        std::cout << "BFS Complete - Nodes: " << searchLength << ", Path: " << pathLength << std::endl;
    }
    void runDepthFirstWithVisualization()
    {
        if (!startNodeSet || !endNodeSet)
        {
            std::cout << "Please set both start and end nodes!" << std::endl;
            return;
        }

        std::cout << "\n=Running Depth-First Search =" << std::endl;
        std::cout << "Start: " << startNode << ", Goal: " << endNode << std::endl;

        reset();
        currentAlgorithm = "DFS Completed";
        isSearching = true;

        Node* start = graph->nodeIndex(startNode);
        Node* dest = graph->nodeIndex(endNode);

        auto visitFunc = [this](Node* node)
        {
            int nodeIdx = this->getNodeIndex(node);

            if (nodeIdx != -1)
            {
                this->currentExpandingNode = nodeIdx;
                this->openSetNodes.insert(nodeIdx);
                this->searchLength++;

                window.clear(sf::Color::White);
                this->drawMaze();
                this->drawHUD();
                window.display();

                std::this_thread::sleep_for(std::chrono::milliseconds(this->ANIMATION_DELAY_MS));
            }
        };

        // Run adapted DFS to get path (but don't build pathNodes yet!)
        graph->adaptedDepthFirst(start, dest);

        // Visualize with visit function FIRST
        graph->clearMarks();
        graph->depthFirst(start, visitFunc);

        // Build path AFTER visualization is complete
        pathNodes.clear();
        Node* current = dest;
        while (current != nullptr)
        {
            int idx = getNodeIndex(current);
            if (idx != -1) pathNodes.push_back(idx);
            current = current->previous();
        }
        std::reverse(pathNodes.begin(), pathNodes.end());

        pathLength = pathNodes.size();
        isSearching = false;
        searchComplete = true;
        currentExpandingNode = -1;

        window.clear(sf::Color::White);
        drawMaze();
        drawHUD();
        window.display();

        std::cout << "DFS Complete - Nodes: " << searchLength << ", Path: " << pathLength << std::endl;
    }

    void handleEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // Mouse click on grid
            if (event.type == sf::Event::MouseButtonPressed && !isSearching)
            {
                float mouseX = event.mouseButton.x;
                float mouseY = event.mouseButton.y;

                if (mouseX >= mazeOffsetX && mouseX < COLS * CELL_SIZE + mazeOffsetX &&
                    mouseY >= mazeOffsetY && mouseY < ROWS * CELL_SIZE + mazeOffsetY)
                {
                    int col = (mouseX - mazeOffsetX) / CELL_SIZE;
                    int row = (mouseY - mazeOffsetY) / CELL_SIZE;
                    int nodeIdx = row * COLS + col;

                    if (!startNodeSet)
                    {
                        startNode = nodeIdx;
                        startNodeSet = true;
                        std::cout << "Start node: " << nodeIdx << std::endl;
                    }
                    else if (!endNodeSet)
                    {
                        endNode = nodeIdx;
                        endNodeSet = true;
                        std::cout << "End node: " << nodeIdx << std::endl;
                        std::cout << "Press A/B/D to run algorithm" << std::endl;
                    }
                }
            }

            // Keyboard controls
            if (event.type == sf::Event::KeyPressed && !isSearching)
            {
                // A = A*
                if (event.key.code == sf::Keyboard::A)
                {
                    runAStarWithVisualization();
                }
                    // B = Breadth-First
                else if (event.key.code == sf::Keyboard::B)
                {
                    runBreadthFirstWithVisualization();
                }
                    // D = Depth-First
                else if (event.key.code == sf::Keyboard::D)
                {
                    runDepthFirstWithVisualization();
                }
                    // R = Reset (keep start/end)
                else if (event.key.code == sf::Keyboard::R)
                {
                    resetAll();
                    std::cout << "Reset. Click to set start node." << std::endl;
                }
                    // ESC = Exit
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }
    }

    void run()
    {
        while (window.isOpen())
        {
            handleEvents();

            if (!isSearching)
            {
                window.clear(sf::Color::White);
                drawMaze();
                drawHUD();
                window.display();
            }
        }
    }

    bool isOpen() { return window.isOpen(); }

    int getStartNode() const { return startNode; }
    int getEndNode() const { return endNode; }
    int getSearchLength() const { return searchLength; }
    int getPathLength() const { return pathLength; }

    void setStartNode(int node)
    {
        if (node >= 0 && node < 240)
        {
            startNode = node;
            startNodeSet = true;
        }
    }

    void setEndNode(int node)
    {
        if (node >= 0 && node < 240)
        {
            endNode = node;
            endNodeSet = true;
        }
    }
};

#endif // MAZE_VISUALIZER_H