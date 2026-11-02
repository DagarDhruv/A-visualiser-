#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <functional>
#include <set>
#include <cmath>
#include <algorithm>
#include <limits>



template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;

// ---------------------------------------------------------------------
//  Name:           Graph
//  Description:    Manages the nodes and connections (arc) between them
// ---------------------------------------------------------------------
template<class NodeType, class ArcType>
class Graph_AdjacencyList
{
public:
    // typedef the classes to make our lives easier.
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;

    // Constructor and destructor functions
    Graph_AdjacencyList( int size );
    ~Graph_AdjacencyList();

    // Accessors
    Node * nodeIndex(int index) const
    {
        return m_nodes.at(index);
    }

    // Public member functions.
    bool addNode( NodeType data, int index );
    void removeNode( int index );
    bool addArc( int from, int to, ArcType weight );
    void removeArc( int from, int to );
    Arc* getArc( int from, int to );
    void clearMarks();

    void depthFirst( Node* node, std::function<void(Node *)> f_visit);

    // To be completed
    void depthFirstNonRec(Node* node, std::function<void(Node*)> f_visit);

    void breadthFirst( Node* node, std::function<void(Node *)> f_visit);

    // To be completed
    void adaptedBreadthFirst( Node* current, Node* goal );

    // To be completed
    void adaptedDepthFirst(Node* start, Node* goal);

    float manhattanDistance(Node* a, Node* b);
    void resetNodes();
    void aStar(Node* start, Node* dest,
               std::function<void(Node*)> f_visit,
               std::vector<Node*>& path);

private:


    // ----------------------------------------------------------------
    //  Description:    A container of all the nodes in the graph.
    // ----------------------------------------------------------------

    std::vector<Node*> m_nodes;

};

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    Constructor, this constructs an empty graph
//  Arguments:      The maximum number of nodes.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph_AdjacencyList<NodeType, ArcType>::Graph_AdjacencyList( int maxNodes ) : m_nodes( maxNodes, nullptr)
{
}

// ----------------------------------------------------------------
//  Name:           ~Graph
//  Description:    destructor, This deletes every node
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph_AdjacencyList<NodeType, ArcType>::~Graph_AdjacencyList() {
    for( int index = 0; index < m_nodes.size(); index++ )
    {
        if( m_nodes[index] != nullptr )
        {
            delete m_nodes.at(index);
        }
    }
}

// ----------------------------------------------------------------
//  Name:           addNode
//  Description:    This adds a node at a given index in the graph.
//  Arguments:      The first parameter is the data to store in the node.
//                  The second parameter is the index to store the node.
//  Return Value:   true if successful
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph_AdjacencyList<NodeType, ArcType>::addNode( NodeType data, int index )
{
    bool nodeNotPresent = false;
    // find out if a node does not exist at that index.
    if ( nullptr == m_nodes.at(index) )
    {
        nodeNotPresent = true;
        // create a new node, put the data in it, and unmark it.
        m_nodes.at(index) = new Node;
        m_nodes.at(index)->m_data = data;
        m_nodes.at(index)->setMarked(false);
    }

    return nodeNotPresent;
}

// ----------------------------------------------------------------
//  Name:           removeNode
//  Description:    This removes a node from the graph
//  Arguments:      The index of the node to return.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::removeNode( int index )
{
    // Only proceed if node does exist.
    if( nullptr != m_nodes.at(index) )
    {
        // now find every arc that points to the node that
        // is being removed and remove it.
        Arc* arc;

        // loop through every node
        for( int node = 0; node < m_nodes.size(); node++ )
        {
            // if the node is valid...
            if( nullptr != m_nodes.at(node) ) {
                // see if the node has an arc pointing to the current node.
                arc = m_nodes.at(node)->getArc(m_nodes.at(index) );
            }
            // if it has an arc pointing to the current node, then
            // remove the arc.
            if( arc != 0 ) {
                removeArc( node, index );
            }
        }


        // now that every arc pointing to the current node has been removed,
        // the node can be deleted.
        delete m_nodes.at(index);
        m_nodes.at(index) = nullptr;
    }
}

// ----------------------------------------------------------------
//  Name:           addArd
//  Description:    Adds an arc from the first index to the
//                  second index with the specified weight.
//  Arguments:      The first argument is the originating node index
//                  The second argument is the ending node index
//                  The third argument is the weight of the arc
//  Return Value:   true on success.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph_AdjacencyList<NodeType, ArcType>::addArc( int from, int to, ArcType weight )
{
    bool proceed = true;
    // make sure both nodes exist.
    if( nullptr == m_nodes.at(from) || nullptr == m_nodes.at(to) )
    {
        proceed = false;
    }

    // if an arc already exists we should not proceed
    if( m_nodes.at(from)->getArc( m_nodes.at(to) ) != nullptr )
    {
        proceed = false;
    }

    if (proceed == true)
    {
        // add the arc to the "from" node.
        m_nodes.at(from)->addArc(m_nodes.at(to), weight );
    }

    return proceed;
}

// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This removes the arc from the first index to the second index
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::removeArc( int from, int to )
{
    // Make sure that the node exists before trying to remove
    // an arc from it.
    bool nodeExists = true;
    if( nullptr == m_nodes.at(from) || nullptr == m_nodes.at(to) )
    {
        nodeExists = false;
    }

    if (nodeExists == true)
    {
        // remove the arc.
        m_nodes.at(from)->removeArc(m_nodes.at(to) );
    }
}


// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    Gets a pointer to an arc from the first index
//                  to the second index.
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   pointer to the arc, or 0 if it doesn't exist.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
GraphArc<NodeType, ArcType>* Graph_AdjacencyList<NodeType, ArcType>::getArc( int from, int to )
{
    Arc* arc = 0;
    // make sure the to and from nodes exist
    if( nullptr != m_nodes.at(from) && nullptr != m_nodes.at(to) )
    {
        arc = m_nodes.at(from)->getArc(m_nodes.at(to) );
    }

    return arc;
}


// ----------------------------------------------------------------
//  Name:           clearMarks
//  Description:    This clears every mark on every node.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::clearMarks()
{
    for( int index = 0; index < m_nodes.size(); index++ )
    {
        if( nullptr != m_nodes.at(index) )
        {
            m_nodes.at(index)->setMarked(false);
        }
    }
}


// ----------------------------------------------------------------
//  Name:           depthFirst
//  Description:    Performs a depth-first traversal on the specified
//                  node.
//  Arguments:      The first argument is the starting node
//                  The second argument is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::depthFirst( Node* node, std::function<void(Node *)> f_visit )
{
    if( nullptr != node )
    {
        // process the current node and mark it
        f_visit( node );
        node->setMarked(true);

        // go through each connecting node
        auto iter = node->arcList().begin();
        auto endIter = node->arcList().end();

        for( ; iter != endIter; ++iter)
        {
            // process the linked node if it isn't already marked.
            if ( (*iter).destNode()->marked() == false )
            {
                depthFirst( (*iter).destNode(), f_visit);
            }
        }
    }
}

template<class NodeType, class ArcType>
inline void Graph_AdjacencyList<NodeType, ArcType>::depthFirstNonRec(Node* node, std::function<void(Node*)> f_visit)
{
    if (nullptr != node)
    {
        std::stack<Node*> nodeStack;
        nodeStack.push(node);
        node->setMarked(true);

        while (!nodeStack.empty())
        {
            Node* currentNode = nodeStack.top();
            nodeStack.pop();
            f_visit(currentNode);


            auto iter = currentNode->arcList().begin();
            auto endIter = currentNode->arcList().end();

            for (; iter != endIter; ++iter)
            {
                Node* adjacentNode = (*iter).destNode();


                if (!adjacentNode->marked())
                {

                    adjacentNode->setMarked(true);
                    nodeStack.push(adjacentNode);
                }
            }
        }
    }
}


// ----------------------------------------------------------------
//  Name:           breadthFirst
//  Description:    Performs a depth-first traversal the starting node
//                  specified as an input parameter.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::breadthFirst( Node* node, std::function<void(Node *)> f_visit)
{
    if( nullptr != node )
    {
        std::queue<Node*> nodeQueue;
        // place the first node on the queue, and mark it.
        nodeQueue.push( node );
        node->setMarked(true);

        // loop through the queue while there are nodes in it.
        while( nodeQueue.size() != 0 )
        {
            // process the node at the front of the queue.
            f_visit( nodeQueue.front() );

            // add all of the child nodes that have not been
            // marked into the queue
            auto iter = nodeQueue.front()->arcList().begin();
            auto endIter = nodeQueue.front()->arcList().end();

            for( ; iter != endIter; iter++ )
            {
                if ( (*iter).destNode()->marked() == false)
                {
                    // mark the node and add it to the queue.
                    (*iter).destNode()->setMarked(true);
                    nodeQueue.push( (*iter).destNode() );
                }
            }

            // dequeue the current node.
            nodeQueue.pop();
        }
    }
}

template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::adaptedBreadthFirst(Node* startNode, Node* targetNode)
{
    // Clear all marks and previous pointers
    clearMarks();
    for (int i = 0; i < m_nodes.size(); i++)
    {
        if (m_nodes[i] != nullptr)
        {
            m_nodes[i]->setPrevious(nullptr);
        }
    }

    // Create a queue for BFS
    std::queue<Node*> queue;

    // Mark the start node and enqueue it
    startNode->setMarked(true);
    queue.push(startNode);

    bool found = false;

    // BFS loop
    while (!queue.empty() && !found)
    {
        // Dequeue a node
        Node* currentNode = queue.front();
        queue.pop();

        // Check if we found the target
        if (currentNode == targetNode)
        {
            found = true;
            break;
        }

        // Explore all adjacent nodes (neighbors)
        auto arcIter = currentNode->arcList().begin();
        auto arcEnd = currentNode->arcList().end();

        for (; arcIter != arcEnd; ++arcIter)
        {
            Node* neighbor = (*arcIter).destNode();

            // If neighbor hasn't been visited
            if (!neighbor->marked())
            {
                // Mark it, set previous pointer, and enqueue
                neighbor->setMarked(true);
                neighbor->setPrevious(currentNode);
                queue.push(neighbor);
            }
        }
    }
}


// ----------------------------------------------------------------
//  Name:           adaptedBreadthFirst
//  Description:    Performs a breadth-first traversal the starting node
//                  specified as an input parameter, terminating at the goal.
//  Arguments:      The first parameter is the starting node.
//                  The second parameter is the goal node.
//  Return Value:   None.
// ----------------------------------------------------------------

template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::adaptedDepthFirst(Node* start, Node* goal) {
    clearMarks();
    for (int i = 0; i < (int)m_nodes.size(); i++) {
        if (m_nodes[i] != nullptr) {
            m_nodes[i]->setPrevious(nullptr);
        }
    }

    std::stack<Node*> stack;
    stack.push(start);
    start->setMarked(true);

    bool found = false;

    while (!stack.empty() && !found) {
        Node* current = stack.top();
        stack.pop();

        if (current == goal) {
            found = true;
            break;
        }

        std::vector<Node*> neighbors;

        auto it = current->arcList().begin();
        auto end = current->arcList().end();

        for (; it != end; ++it) {
            const Arc& arc = *it;

            if (arc.weight() == 1) {
                continue;
            }

            Node* neighbor = arc.destNode();

            if (!neighbor->marked()) {
                neighbors.push_back(neighbor);
            }
        }

        for (int i = (int)neighbors.size() - 1; i >= 0; i--) {
            neighbors[i]->setMarked(true);
            neighbors[i]->setPrevious(current);
            stack.push(neighbors[i]);
        }
    }

    if (!found) {
        std::cout << "No path found." << std::endl;
        return;
    }

    std::vector<Node*> path;
    for (Node* n = goal; n != nullptr; n = n->previous()) {
        path.push_back(n);
    }

    std::cout << "Path found: ";
    for (int i = (int)path.size() - 1; i >= 0; i--) {
        std::cout << path[i]->m_data.m_name;
        if (i > 0) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
}
// ----------------------------------------------------------------
//  Name:           manhattanDistance
//  Description:    Calculates Manhattan distance heuristic between two nodes
//  Arguments:      Two nodes to calculate distance between
//  Return Value:   Float representing Manhattan distance
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
float Graph_AdjacencyList<NodeType, ArcType>::manhattanDistance(Node* a, Node* b)
{
    return std::abs(a->m_data.row - b->m_data.row) +
           std::abs(a->m_data.col - b->m_data.col);
}

// ----------------------------------------------------------------
//  Name:           resetNodes
//  Description:    Resets all nodes' A* values for a new search
//  Arguments:      None
//  Return Value:   None
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::resetNodes()
{
    for (int i = 0; i < m_nodes.size(); i++)
    {
        if (m_nodes[i] != nullptr)
        {
            m_nodes[i]->m_data.g = std::numeric_limits<float>::infinity();
            m_nodes[i]->m_data.h = 0.0f;
            m_nodes[i]->m_data.f = std::numeric_limits<float>::infinity();
            m_nodes[i]->setPrevious(nullptr);
            m_nodes[i]->setMarked(false);
        }
    }
}

// ----------------------------------------------------------------
//  Name:           aStar
//  Description:    Performs A* pathfinding from start to dest node
//  Arguments:      start - starting node
//                  dest - destination node
//                  f_visit - function called when visiting each node
//                  path - vector to store the final path
//  Return Value:   None (path is returned via reference parameter)
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph_AdjacencyList<NodeType, ArcType>::aStar(Node* start, Node* dest,
                                                   std::function<void(Node*)> f_visit,
                                                   std::vector<Node*>& path)
{
    // Clear any previous path
    path.clear();

    // Reset all nodes' A* values
    resetNodes();

    // Priority queue comparator - min-heap based on f value
    auto compare = [](Node* a, Node* b)
    {
        // If f values are equal, use h as tiebreaker (prefer nodes closer to goal)
        if (a->m_data.f == b->m_data.f)
        {
            return a->m_data.h > b->m_data.h;
        }
        return a->m_data.f > b->m_data.f;  // Min-heap: smaller f = higher priority
    };

    // Open set: nodes to be evaluated (priority queue for efficient min extraction)
    std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> openSet(compare);

    // Open set lookup: for O(log n) membership checking
    std::set<Node*> openSetLookup;

    // Closed set: nodes already evaluated
    std::set<Node*> closedSet;

    // Initialize start node
    start->m_data.g = 0.0f;
    start->m_data.h = manhattanDistance(start, dest);
    start->m_data.f = start->m_data.g + start->m_data.h;

    // Add start node to open set
    openSet.push(start);
    openSetLookup.insert(start);

    // Main A* loop
    while (!openSet.empty())
    {
        // Get node with lowest f value
        Node* current = openSet.top();
        openSet.pop();
        openSetLookup.erase(current);

        // Call visit function (for visualization purposes)
        if (f_visit)
        {
            f_visit(current);
        }

        // Check if we've reached the goal
        if (current == dest)
        {
            // Reconstruct path by following previous pointers
            Node* pathNode = dest;
            while (pathNode != nullptr)
            {
                path.push_back(pathNode);
                pathNode = pathNode->previous();
            }

            // Reverse path to go from start to destination
            std::reverse(path.begin(), path.end());
            return;
        }

        // Add current node to closed set
        closedSet.insert(current);

        // Explore all neighbors
        auto arcIter = current->arcList().begin();
        auto arcEnd = current->arcList().end();

        for (; arcIter != arcEnd; ++arcIter)
        {
            Node* neighbor = (*arcIter).destNode();
            float edgeCost = (*arcIter).weight();

            // Skip if neighbor is in closed set (already evaluated)
            if (closedSet.find(neighbor) != closedSet.end())
            {
                continue;
            }

            // Calculate tentative g score (cost from start to neighbor through current)
            float tentativeG = current->m_data.g + edgeCost;

            // Check if neighbor is already in open set
            bool inOpenSet = (openSetLookup.find(neighbor) != openSetLookup.end());

            // If this path to neighbor is better than any previous path
            if (!inOpenSet || tentativeG < neighbor->m_data.g)
            {
                // Update neighbor's values
                neighbor->setPrevious(current);
                neighbor->m_data.g = tentativeG;
                neighbor->m_data.h = manhattanDistance(neighbor, dest);
                neighbor->m_data.f = neighbor->m_data.g + neighbor->m_data.h;

                // Add to open set if not already there
                if (!inOpenSet)
                {
                    openSet.push(neighbor);
                    openSetLookup.insert(neighbor);
                }
            }
        }
    }

    // No path found - path vector remains empty
    path.clear();
}

#include "Header/GraphNode.h"
#include "Header/GraphArc.h"


#endif
