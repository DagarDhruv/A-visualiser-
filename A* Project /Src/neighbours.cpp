#include <iostream>
#include <fstream>

void neighbours(int t_rows, int t_cols, std::ofstream & nodesFile, std::ofstream& arcsFile)
{

    for (int i = 0; i < t_rows * t_cols; i++)
    {
        nodesFile << i << "\n";
    }

    for (int y = 0; y < t_rows; y++)
    {
        for (int x = 0; x < t_cols; x++)
        {
            std::cout << "Cell (" << x << ", " << y << ") has neighbours: ";
            // North (y - 1)
            if (y > 0)
            {
                /*std::cout << "(" << x << ", " << y - 1 << ") ";*/
                arcsFile << ((y * t_cols) + x) << " " << (((y-1) * t_cols) + x) << " " << 0 << "\n";
                /*std::cout << "Node number is: " << ((y * t_cols) + x) << "\n";*/
            }

            // South (y + 1)
            if (y < t_rows - 1)
            {
                /*std::cout << "(" << x << ", " << y + 1 << ") ";
                std::cout << "Node number is: " << ((y * t_cols) + x) << "\n";*/
                arcsFile << ((y * t_cols) + x) << " " << (((y+1) * t_cols) + x) << " " << 0 << "\n";
            }

            // West (x - 1)
            if (x > 0)
            {
                /*std::cout << "(" << x - 1 << ", " << y << ") ";
                std::cout << "Node number is: " << ((y * t_cols) + x) << "\n";*/
                arcsFile << ((y * t_cols) + x) << " " << ((y * t_cols) + (x-1)) << " " << 0 << "\n";
            }

            // East (x + 1)
            if (x < t_cols - 1)
            {
                /*std::cout << "(" << x + 1 << ", " << y << ") ";
                std::cout << "Node number is: " << ((y * t_cols) + x) << "\n";*/
                arcsFile << ((y * t_cols) + x) << " " << ((y * t_cols) + (x+1)) << " " << 0 << "\n";
            }
            std::cout << "\n";
        }
    }
}

int main1()
{
    std::ofstream nodesFile("/Users/dhruvdagar/Documents/c++/year2/ds25-DagarDhruv/A* Project /Include/nodes.txt");
    std::ofstream arcsFile("/Users/dhruvdagar/Documents/c++/year2/ds25-DagarDhruv/A* Project /Include/arcs.txt");

    // Show neighbours in a 10x10 grid.
    neighbours(4, 5, nodesFile, arcsFile);
    return 0;
}