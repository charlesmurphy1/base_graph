#include <iostream>
#include <string>

#include "BaseGraph/edgelabeled_directedgraph.hpp"

int main() {
    struct Flight {
        std::string company;
        double distance;
    };

    BaseGraph::EdgeLabeledDirectedGraph<Flight> graph(5);
    graph.addEdge(0, 1, {"Company A", 10.});
    graph.addEdge(4, 3, {"Company B", 2.2});

    Flight flightA = graph.getEdgeLabelOf(0, 1);
    Flight flightB = graph.getEdgeLabelOf(4, 3);

    std::cout << "Flight from 0 to 1, company: " << flightA.company
              << ", distance: " << flightA.distance << std::endl;
    std::cout << "Flight from 4 to 3, company: " << flightB.company
              << ", distance: " << flightB.distance << std::endl;

    graph.setEdgeLabel(0, 1, {"Company B", 10.});

    std::cout << "Flight from 0 to 1 is now by company: "
              << graph.getEdgeLabelOf(0, 1).company << std::endl;

    return 0;
}
