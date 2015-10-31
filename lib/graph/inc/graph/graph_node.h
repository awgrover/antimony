#pragma once

#include "graph/node.h"

class Graph;

class GraphNode : public Node
{
public:
    explicit GraphNode(std::string name, Graph* root);
    explicit GraphNode(std::string name, uint32_t uid, Graph* root);

    Graph* getGraph() const { return subgraph.get(); }

protected:
    std::unique_ptr<Graph> subgraph;
};
