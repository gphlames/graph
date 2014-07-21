#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>

#include "graph.h"

using namespace std;

const double INF = numeric_limits<double>::infinity();

void dijkstra(Graph<double,double> g, unsigned int source, unsigned int target, std::unordered_map<unsigned int, double> &return_dist, std::unordered_map<unsigned int, int> &return_prev)
{
    // Check that source and target are in graph
    if (g.nodes.find(source) == g.nodes.end() || g.nodes.find(target) == g.nodes.end())
    {
        cout << "Source and/or target nodes not in graph." << endl;
        return;
    }

    std::vector<unsigned int> list; // list of nodes to process
    std::unordered_map<unsigned int, double> dist; // id, distance
    std::unordered_map<unsigned int, int> prev; // id, previous

    // Initialize
    for(auto n : g.nodes)
    {
        list.push_back(n.first);
        dist.emplace(n.first, INF);
        prev.emplace(n.first, -1); // -1: no prev node
    }

    dist.at(source) = 0;

    while(!list.empty())
    {
        // Select node with smallest distance
        unsigned int node = list.at(0);
        for(auto id : list)
            if(dist.at(id) < dist.at(node))
                node = id;

        // Return if target (comment out if shortest path to all nodes is desired)
        if(node == target)
            break;

        // Remove node from list
        list.erase(std::remove(list.begin(), list.end(), node), list.end());

        // Check if target is unreachable
        if(dist.at(node) >= INF)
            break;

        // For each neighbour of node
        for(auto e : g.nodes.at(node)->edges_out)
        {
            auto neighbour = e->to->id;
            double new_dist = dist.at(node) + e->data;
            if(new_dist < dist.at(neighbour))
            {
                dist.at(neighbour) = new_dist;
                prev.at(neighbour) = node;
                // decrease-key v in Q; // Reorder v in the Queue (that is, heapify-down)
            }

        }
    }

    return_dist = dist;
    return_prev = prev;
}

struct Data
{
    Data() { bar.reserve(100000); }
    //~Data() { bar.clear(); }

    std::vector<double> bar;
};

int main()
{
    // Testing graph
    Graph<double,double> my_graph;
    for(int i=0; i<=5; i++)
        my_graph.add_node(i);

    my_graph.add_edge(0,1,4);
    my_graph.add_edge(0,2,2);
    my_graph.add_edge(1,2,5);
    my_graph.add_edge(1,3,10);
    my_graph.add_edge(2,4,3);
    my_graph.add_edge(4,3,4);
    my_graph.add_edge(3,5,11);

    cout << "My graph" << endl;
    cout << my_graph << endl;

    // Testing removal of edges and nodes
//    my_graph.remove_edge(1,0);
//    my_graph.remove_node(0);
//    cout << "My new graph" << endl;
//    cout << my_graph << endl;

    // Run Dijkstra
    std::unordered_map<unsigned int, double> dist; // id, distance
    std::unordered_map<unsigned int, int> prev; // id, previous
    unsigned int source = 0;
    unsigned int target = 5;
    dijkstra(my_graph, source, target, dist, prev);

    cout << "Prev result" << endl;
    for(auto n : prev)
        cout << n.first << ", " << n.second << endl;

    // Construct path
    std::vector<unsigned int> path;
    unsigned int u = target;
    while (prev.at(u) >= 0)
    {
        path.push_back(u);
        u = prev.at(u);
    }
    // Add source (will be target if no feasible path is found)
    path.push_back(u);

    cout << "Path " << endl;
    for(auto n : path)
        cout << n << endl;

    // Print results
    cout << "The shortest path from " << source << " to " << target << " is:" << endl;
    for(int i = path.size()-1; i>=0; i--)
    {
        unsigned int n = path.at(i);
        cout << n << " (" << dist.at(n) << ")";
        if(i > 0)
            cout << " => ";
    }
    cout << endl;

    // Testing add functionality
//    Graph<> g;

//    g.add_node(0,1.1);
//    g.add_node(1,2.2);
//    g.add_node(2,3.3);

//    g.add_edge(0,1,1);
//    g.add_edge(0,3,1);
//    g.add_edge(0,2,1);
//    g.add_edge(1,0,2);
//    g.add_edge(2,1,2);

//    cout << g << endl;

//    cout << "Hello World!" << endl;


    // ANDERS START HER!
    // Test for memory leaks
    Graph<Data,Data> mem_graph; // Data is just a struct with an empty vector. The vector is reserved a lot of space.

    // Add some initial nodes
    int i = 0;
    for(i = 0; i<10; i++)
        mem_graph.add_node(i);

    // A long loop where one node and one edge is added, and one node and one edge is removed
    // Expecting stable memory usage
    for(i; i<10000000; i++)
    {
        mem_graph.add_node(i);
        mem_graph.add_edge(i-1,i);
        mem_graph.remove_node(i-2);

        int c = 0;
        for(auto n : mem_graph.nodes)
        {
            c += n.second->edges_in.size();
            c += n.second->edges_out.size();
        }

        // Print some debug info
        cout << mem_graph.nodes.size() << " - " << mem_graph.edges.size() << " - " << c << " - " << mem_graph.nodes.bucket_count() << " - " << mem_graph.edges.bucket_count() << endl;
    }

    // Print buckets of nodes and edges map in graph

    cout << "Nodes" << endl;
    for (unsigned i=0; i<mem_graph.nodes.bucket_count(); ++i)
    {
        std::cout << "bucket #" << i << " contains: ";
        for (auto it = mem_graph.nodes.begin(i); it!=mem_graph.nodes.end(i); ++it)
        {
            std::cout << "[" << it->first << "] ";
        }
        std::cout << "\n";
    }

    cout << "Edges" << endl;
    for (unsigned i=0; i<mem_graph.edges.bucket_count(); ++i)
    {
        std::cout << "bucket #" << i << " contains: ";
        for (auto it = mem_graph.edges.begin(i); it!=mem_graph.edges.end(i); ++it)
        {
            std::cout << "[" << std::get<0>(it->first) << "] ";
        }
        std::cout << "\n";
    }

    return 0;
}

