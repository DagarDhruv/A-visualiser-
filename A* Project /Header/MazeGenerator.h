//
// Created by Dhruv Dagar on 02/12/2025.
//


#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

#include <fstream>
#include <vector>
#include <random>
#include <stack>
#include <iostream>


class MazeGenerator
{
private:
    int rows;
    int cols;
    std::vector<std::vector<bool>> visited;
    std::mt19937 rng;

    // Get unvisited neighbors (up, down, left, right ONLY - no diagonals)
    std::vector<std::pair<int, int>> getUnvisitedNeighbors(int row, int col)
    {
        std::vector<std::pair<int, int>> neighbors;

        // North (up)
        if (row > 0 && !visited[row - 1][col])
        {
            neighbors.push_back({row - 1, col});
        }

        // South (down)
        if (row < rows - 1 && !visited[row + 1][col])
        {
            neighbors.push_back({row + 1, col});
        }

        // West (left)
        if (col > 0 && !visited[row][col - 1])
        {
            neighbors.push_back({row, col - 1});
        }

        // East (right)
        if (col < cols - 1 && !visited[row][col + 1])
        {
            neighbors.push_back({row, col + 1});
        }

        return neighbors;
    }

    // Convert (row, col) to node index
    int getNodeIndex(int row, int col)
    {
        return row * cols + col;
    }

public:
    MazeGenerator(int r, int c) : rows(r), cols(c)
    {
        // Initialize random seed
        std::random_device rd;
        rng.seed(rd());

        // Initialize visited grid (all false)
        visited.resize(rows, std::vector<bool>(cols, false));
    }

    // Generate maze using DFS algorithm from assignment spec
    void generateMaze(const std::string& nodesFile, const std::string& arcsFile)
    {
        std::ofstream nodes(nodesFile);
        std::ofstream arcs(arcsFile);

        if (!nodes.is_open() || !arcs.is_open())
        {
            std::cerr << "Error: Could not open output files!" << std::endl;
            return;
        }

        //Create all nodes (0 to 239 for 12x20 maze)
        std::cout << "Creating " << rows * cols << " nodes..." << std::endl;
        for (int i = 0; i < rows * cols; i++)
        {
            nodes << i << "\n";
        }
        nodes.close();

        // Generate maze using adapted DFS from spec
        // Choose a random starting cell
        std::uniform_int_distribution<> distRow(0, rows - 1);
        std::uniform_int_distribution<> distCol(0, cols - 1);

        int startRow = distRow(rng);
        int startCol = distCol(rng);

        std::cout << "Starting maze generation from cell (" << startRow << ", " << startCol << ")" << std::endl;

        // Initialize stack with starting cell
        std::stack<std::pair<int, int>> stack;
        stack.push({startRow, startCol});
        visited[startRow][startCol] = true;

        int arcCount = 0;

        // Main DFS loop (from spec)
        while (!stack.empty())
        {
            // Pop current cell from stack
            auto [currentRow, currentCol] = stack.top();

            // Get list of unvisited neighbours (up/down/left/right only)
            auto neighbors = getUnvisitedNeighbors(currentRow, currentCol);

            if (!neighbors.empty())
            {
                // Randomly select one unvisited neighbor
                std::uniform_int_distribution<> dist(0, neighbors.size() - 1);
                int selectedIdx = dist(rng);
                auto [nextRow, nextCol] = neighbors[selectedIdx];

                // Remove the arc (wall) between current and selected neighbor
                // In our implementation, we ADD arcs (passages) instead of removing walls
                int currentIdx = getNodeIndex(currentRow, currentCol);
                int nextIdx = getNodeIndex(nextRow, nextCol);

                // Create bidirectional arc (weight 0 = passable, no wall)
                arcs << currentIdx << " " << nextIdx << " 0\n";
                arcs << nextIdx << " " << currentIdx << " 0\n";
                arcCount += 2;

                // Mark selected neighbor as visited
                visited[nextRow][nextCol] = true;

                // Push current cell back onto stack (for backtracking)
                // Push selected neighbor onto stack (to continue from there)
                stack.push({nextRow, nextCol});
            }
            else
            {
                // No unvisited neighbors - backtrack
                stack.pop();
            }
        }

        arcs.close();

        // Verification and stats
        std::cout << "\n=== Maze Generation Complete ===" << std::endl;
        std::cout << "Dimensions: " << rows << " rows x " << cols << " columns" << std::endl;
        std::cout << "Total nodes: " << rows * cols << std::endl;
        std::cout << "Total arcs created: " << arcCount << std::endl;
        std::cout << "Expected arcs (perfect maze): " << 2 * (rows * cols - 1) << std::endl;

        // Verify all cells visited
        int visitedCount = 0;
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                if (visited[r][c]) visitedCount++;
            }
        }
        std::cout << "Cells visited: " << visitedCount << " / " << rows * cols << std::endl;

        if (visitedCount == rows * cols)
        {
            std::cout << "All cells connected so maze is valid!" << std::endl;
        }
        else
        {
            std::cout << "✗ Warning: Not all cells visited " << std::endl;
        }

        std::cout << "\nFiles created:" << std::endl;
        std::cout << "  - " << nodesFile << std::endl;
        std::cout << "  - " << arcsFile << std::endl;
    }
};

#endif // MAZE_GENERATOR_H