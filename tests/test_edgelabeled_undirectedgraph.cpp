#include <deque>
#include <list>
#include <stdexcept>

#include "BaseGraph/edgelabeled_undirectedgraph.hpp"
#include "fixtures.hpp"
#include "gtest/gtest.h"

typedef ::testing::Types<std::string, int> TestTypes;
typedef ::testing::Types<int> TestTypes_integral;

TYPED_TEST_SUITE(EdgeLabeledUndirectedGraph_, TestTypes);
TYPED_TEST_SUITE(EdgeLabeledUndirectedGraph_integral, TestTypes_integral);

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           getOutEdgesOf_vertexOutOfRange_throwInvalidArgument) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(0);

    EXPECT_THROW(graph.getOutEdgesOf(0), std::out_of_range);
    graph.resize(2);
    EXPECT_THROW(graph.getOutEdgesOf(2), std::out_of_range);
}

// When force=false in addEdge, hasEdge is called.
// Both methods depend on each other so one must be tested first arbitrarily.

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           addEdge_validEdge_successorInAdjacency) {
    this->graph.addEdge(0, 2, this->labels[0]);
    this->graph.addEdge(1, 0, this->labels[1]);

    this->EXPECT_NEIGHBOURS(0, {2, 1});
    this->EXPECT_NEIGHBOURS(1, {0});
    this->EXPECT_NEIGHBOURS(2, {0});
    this->EXPECT_LABEL({0, 2}, 0);
    this->EXPECT_LABEL({0, 1}, 1);
    EXPECT_EQ(this->graph.getEdgeNumber(), 2);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_, addEdge_selfLoop_successorInAdjacency) {
    this->graph.addEdge(1, 1, this->labels[0]);

    this->EXPECT_NEIGHBOURS(1, {1});
    this->EXPECT_LABEL({1, 1}, 0);
    EXPECT_EQ(this->graph.getEdgeNumber(), 1);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           addEdge_multiedge_successorInAdjacencyOnce) {
    this->graph.addEdge(1, 2, this->labels[0]);
    this->graph.addEdge(1, 2, this->labels[0]);
    this->graph.addEdge(2, 1, this->labels[0]);

    this->EXPECT_NEIGHBOURS(1, {2});
    this->EXPECT_NEIGHBOURS(2, {1});
    EXPECT_EQ(this->graph.getEdgeNumber(), 1);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           addEdge_multiedgeForced_successorInAdjacencyTwice) {
    this->graph.addEdge(1, 2, this->labels[0]);
    this->graph.addEdge(2, 1, this->labels[0], true);

    this->EXPECT_NEIGHBOURS(1, {2, 2});
    this->EXPECT_NEIGHBOURS(2, {1, 1});
    EXPECT_EQ(this->graph.getEdgeNumber(), 2);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           addEdge_vertexOutOfRange_throwInvalidArgument) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(0);

    EXPECT_THROW(graph.addEdge(0, 0, this->labels[0]), std::out_of_range);
    graph.resize(2);
    EXPECT_THROW(graph.addEdge(1, 2, this->labels[0]), std::out_of_range);
    EXPECT_THROW(graph.addEdge(2, 1, this->labels[0]), std::out_of_range);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_, hasEdge_existentEdge_ReturnTrue) {
    this->graph.addEdge(0, 2, this->labels[0]);
    this->graph.addEdge(0, 1, this->labels[1]);

    for (auto edge_label :
         std::list<std::tuple<BaseGraph::VertexIndex, BaseGraph::VertexIndex,
                              TypeParam>>{{0, 2, this->labels[0]},
                                          {0, 1, this->labels[1]}}) {
        BaseGraph::VertexIndex i(std::get<0>(edge_label)),
            j(std::get<1>(edge_label));
        auto label = std::get<2>(edge_label);

        // Check edges (i,j) and (j,i)
        for (auto v = 0; v < 2; v++) {
            EXPECT_TRUE(this->graph.hasEdge(j, i));
            EXPECT_TRUE(this->graph.hasEdge(j, i, label));
            std::swap(i, j);
        }
    }
}

TYPED_TEST(EdgeLabeledUndirectedGraph_, hasEdge_inexistentEdge_ReturnFalse) {
    this->graph.addEdge(0, 2, this->labels[0]);
    this->graph.addEdge(0, 1, this->labels[1]);

    EXPECT_FALSE(this->graph.hasEdge(2, 1));
    EXPECT_FALSE(this->graph.hasEdge(1, 2));

    for (auto edge_label :
         std::list<std::tuple<BaseGraph::VertexIndex, BaseGraph::VertexIndex,
                              TypeParam>>{{1, 2, this->labels[0]}}) {
        BaseGraph::VertexIndex i(std::get<0>(edge_label)),
            j(std::get<1>(edge_label));
        auto label = std::get<2>(edge_label);

        // Check edges (i,j) and (j,i)
        for (auto v = 0; v < 2; v++) {
            EXPECT_FALSE(this->graph.hasEdge(j, i));
            EXPECT_FALSE(this->graph.hasEdge(j, i, label));
            std::swap(i, j);
        }
    }
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           hasEdge_vertexOutOfRange_throwOutOfRange) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(0);

    EXPECT_THROW(graph.hasEdge(0, 0), std::out_of_range);
    EXPECT_THROW(graph.hasEdge(0, 0, this->labels[0]), std::out_of_range);
    graph.resize(2);
    for (auto edge : std::list<BaseGraph::Edge>{{1, 2}, {2, 1}}) {
        EXPECT_THROW(graph.hasEdge(edge.first, edge.second), std::out_of_range);
        EXPECT_THROW(graph.hasEdge(edge.first, edge.second, this->labels[0]),
                     std::out_of_range);
    }
}

template <template <class...> class Container, class... Args, typename Labels>
static void testAllEdgesExistForContainer(Labels &labels) {
    using EdgeLabel = typename Labels::value_type;
    Container<BaseGraph::LabeledEdge<EdgeLabel>> edges = {{0, 2, labels[0]},
                                                          {0, 1, labels[1]},
                                                          {0, 0, labels[2]},
                                                          {10, 5, labels[3]}};
    BaseGraph::EdgeLabeledUndirectedGraph<EdgeLabel> graph(edges);

    for (auto edge : edges) {
        EXPECT_TRUE(graph.hasEdge(std::get<0>(edge), std::get<1>(edge),
                                  std::get<2>(edge)));
        EXPECT_TRUE(graph.hasEdge(std::get<1>(edge), std::get<0>(edge),
                                  std::get<2>(edge)));
    }
    EXPECT_EQ(graph.getEdgeNumber(), 4);
    EXPECT_EQ(graph.getSize(), 11);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           edgeListConstructor_anyContainer_allEdgesExist) {
    testAllEdgesExistForContainer<std::vector>(this->labels);
    testAllEdgesExistForContainer<std::list>(this->labels);
    testAllEdgesExistForContainer<std::set>(this->labels);
    testAllEdgesExistForContainer<std::deque>(this->labels);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           getEdgeLabelOf_existentEdge_correctLabel) {
    this->graph.addEdge(0, 2, this->labels[0]);
    this->graph.addEdge(0, 1, this->labels[1]);

    this->EXPECT_LABEL({0, 2}, 0);
    this->EXPECT_LABEL({0, 1}, 1);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           getEdgeLabelOf_inexistentEdge_throwInvalidArgument) {
    EXPECT_THROW(this->graph.getEdgeLabelOf(0, 2, true), std::invalid_argument);
    EXPECT_THROW(this->graph.getEdgeLabelOf(2, 0, true), std::invalid_argument);
    this->graph.addEdge(0, 1, this->labels[0]);
    EXPECT_THROW(this->graph.getEdgeLabelOf(0, 2, true), std::invalid_argument);
    EXPECT_THROW(this->graph.getEdgeLabelOf(2, 0, true), std::invalid_argument);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           getEdgeLabelOf_vertexOutOfRange_throwInvalidArgument) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(0);

    EXPECT_THROW(graph.getEdgeLabelOf(0, 0), std::out_of_range);
    graph.resize(2);
    EXPECT_THROW(graph.getEdgeLabelOf(1, 2), std::out_of_range);
    EXPECT_THROW(graph.getEdgeLabelOf(2, 1), std::out_of_range);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           setEdgeLabel_existentEdge_labelChanged) {
    this->graph.addEdge(0, 2, this->labels[0]);
    this->graph.addEdge(0, 1, this->labels[1]);
    this->graph.setEdgeLabel(0, 1, this->labels[0]);
    this->graph.setEdgeLabel(0, 2, this->labels[1]);

    this->EXPECT_LABEL({0, 1}, 0);
    this->EXPECT_LABEL({0, 2}, 1);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           setEdgeLabel_inexistentEdge_throwInvalidArgument) {
    EXPECT_THROW(this->graph.setEdgeLabel(0, 2, this->labels[0]),
                 std::invalid_argument);
    this->graph.addEdge(0, 1, this->labels[0]);
    EXPECT_THROW(this->graph.setEdgeLabel(0, 2, this->labels[0]),
                 std::invalid_argument);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           setEdgeLabel_vertexOutOfRange_throwInvalidArgument) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(0);

    EXPECT_THROW(graph.setEdgeLabel(0, 0, this->labels[0]), std::out_of_range);
    graph.resize(2);
    EXPECT_THROW(graph.setEdgeLabel(1, 2, this->labels[0]), std::out_of_range);
    EXPECT_THROW(graph.setEdgeLabel(2, 1, this->labels[0]), std::out_of_range);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeEdge_existentEdge_edgeDoesntExist) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);
    this->graph.removeEdge(0, 2);

    EXPECT_TRUE(this->graph.hasEdge(0, 1));
    EXPECT_FALSE(this->graph.hasEdge(0, 2));
    EXPECT_EQ(this->graph.getEdgeNumber(), 1);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeEdge_existentSelfLoop_edgeDoesntExist) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 0, this->labels[1]);
    this->graph.removeEdge(0, 0);

    EXPECT_TRUE(this->graph.hasEdge(0, 1));
    EXPECT_FALSE(this->graph.hasEdge(0, 0));
    EXPECT_EQ(this->graph.getEdgeNumber(), 1);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeEdge_inexistentEdge_edgeDoesntExist) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.removeEdge(0, 2);

    EXPECT_TRUE(this->graph.hasEdge(0, 1));
    EXPECT_FALSE(this->graph.hasEdge(0, 2));
    EXPECT_EQ(this->graph.getEdgeNumber(), 1);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeEdge_vertexOutOfRange_throwInvalidArgument) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(0);

    EXPECT_THROW(graph.removeEdge(0, 0), std::out_of_range);
    graph.resize(2);
    EXPECT_THROW(graph.removeEdge(1, 2), std::out_of_range);
    EXPECT_THROW(graph.removeEdge(2, 1), std::out_of_range);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeDuplicateEdges_noMultiedge_doNothing) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);
    this->graph.addEdge(1, 1, this->labels[2]);

    this->graph.removeDuplicateEdges();

    this->EXPECT_NEIGHBOURS(0, {1, 2});
    this->EXPECT_NEIGHBOURS(1, {0, 1});
    this->EXPECT_NEIGHBOURS(2, {0});
    EXPECT_EQ(this->graph.getEdgeNumber(), 3);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeDuplicateEdges_multiedge_keepOneEdge) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);
    this->graph.addEdge(0, 1, this->labels[1], true);
    this->graph.addEdge(0, 1, this->labels[2], true);
    this->graph.addEdge(1, 1, this->labels[2]);

    this->graph.removeDuplicateEdges();

    this->EXPECT_NEIGHBOURS(0, {1, 2});
    this->EXPECT_NEIGHBOURS(1, {0, 1});
    this->EXPECT_NEIGHBOURS(2, {0});
    EXPECT_EQ(this->graph.getEdgeNumber(), 3);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeDuplicateEdges_multiSelfLoop_keepOnlyOneSelfLoop) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(1, 1, this->labels[1]);
    this->graph.addEdge(1, 1, this->labels[0], true);
    this->graph.addEdge(1, 2, this->labels[2]);
    this->graph.addEdge(1, 1, this->labels[2], true);

    this->graph.removeDuplicateEdges();

    this->EXPECT_NEIGHBOURS(0, {1});
    this->EXPECT_NEIGHBOURS(1, {0, 1, 2});
    this->EXPECT_NEIGHBOURS(2, {1});
    EXPECT_EQ(this->graph.getEdgeNumber(), 3);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_, removeSelfLoops_noSelfLoop_doNothing) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);

    this->graph.removeSelfLoops();

    this->EXPECT_NEIGHBOURS(0, {1, 2});
    this->EXPECT_NEIGHBOURS(1, {0});
    this->EXPECT_NEIGHBOURS(2, {0});
    EXPECT_EQ(this->graph.getEdgeNumber(), 2);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeSelfLoops_existentSelfLoop_removeSelfLoop) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);
    this->graph.addEdge(0, 0, this->labels[2]);

    this->graph.removeSelfLoops();

    this->EXPECT_NEIGHBOURS(0, {1, 2});
    this->EXPECT_NEIGHBOURS(1, {0});
    this->EXPECT_NEIGHBOURS(2, {0});
    EXPECT_EQ(this->graph.getEdgeNumber(), 2);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeVertexFromEdgeList_vertexInEdges_vertexNotInEdges) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 0, this->labels[1]);
    this->graph.addEdge(1, 2, this->labels[2]);
    this->graph.addEdge(1, 0, this->labels[3], true);
    this->graph.addEdge(1, 3, this->labels[4]);

    this->graph.removeVertexFromEdgeList(0);

    this->EXPECT_NEIGHBOURS(0, {});
    this->EXPECT_NEIGHBOURS(1, {2, 3});
    this->EXPECT_NEIGHBOURS(2, {1});
    this->EXPECT_NEIGHBOURS(3, {1});
    EXPECT_EQ(this->graph.getEdgeNumber(), 2);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           removeVertexFromEdgeList_vertexOutOfRange_throwInvalidArgument) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(0);

    EXPECT_THROW(graph.removeVertexFromEdgeList(0), std::out_of_range);
    graph.resize(2);
    EXPECT_THROW(graph.removeVertexFromEdgeList(2), std::out_of_range);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_, clearEdges_anyGraph_graphHasNoEdge) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 0, this->labels[1]);
    this->graph.addEdge(1, 2, this->labels[2]);
    this->graph.addEdge(1, 0, this->labels[3]);

    this->graph.clearEdges();

    this->EXPECT_NEIGHBOURS(0, {});
    this->EXPECT_NEIGHBOURS(1, {});
    this->EXPECT_NEIGHBOURS(2, {});
    EXPECT_EQ(this->graph.getEdgeNumber(), 0);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           equalityOperator_twoEmptyGraphs_returnTrue) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(2);
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph2(2);

    EXPECT_TRUE(graph == graph2);
    EXPECT_TRUE(graph2 == graph);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           equalityOperator_differentSize_returnFalse) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(3);
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph2(2);

    EXPECT_FALSE(this->graph == graph2);
    EXPECT_FALSE(graph2 == this->graph);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           equalityOperator_sameEdgesAndSize_returnTrue) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(3);
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph2(3);
    graph.addEdge(0, 2, this->labels[0]);
    graph.addEdge(0, 1, this->labels[1]);
    graph2.addEdge(0, 2, this->labels[0]);
    graph2.addEdge(0, 1, this->labels[1]);

    EXPECT_TRUE(graph == graph2);
    EXPECT_TRUE(graph2 == graph);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           equalityOperator_differentEdgeOrder_returnTrue) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(3);
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph2(3);
    graph.addEdge(0, 2, this->labels[0]);
    graph.addEdge(0, 1, this->labels[1]);
    graph2.addEdge(0, 1, this->labels[1]);
    graph2.addEdge(0, 2, this->labels[0]);

    EXPECT_TRUE(graph == graph2);
    EXPECT_TRUE(graph2 == graph);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           equalityOperator_differentLabels_returnFalse) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(3);
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph2(3);
    graph.addEdge(0, 2, this->labels[0]);
    graph.addEdge(0, 1, this->labels[1]);
    graph2.addEdge(0, 1, this->labels[1]);
    graph2.addEdge(0, 2, this->labels[1]);

    EXPECT_FALSE(graph == graph2);
    EXPECT_FALSE(graph2 == graph);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           equalityOperator_missingEdge_returnFalse) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(3);
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph2(3);
    graph.addEdge(0, 1, this->labels[0]);
    graph.addEdge(0, 2, this->labels[1]);
    graph2.addEdge(0, 2, this->labels[1]);

    EXPECT_FALSE(graph == graph2);
    EXPECT_FALSE(graph2 == graph);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_,
           equalityOperator_differentEdges_returnFalse) {
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph(3);
    BaseGraph::EdgeLabeledUndirectedGraph<TypeParam> graph2(3);
    graph.addEdge(0, 1, this->labels[0]);
    graph.addEdge(0, 2, this->labels[1]);
    graph2.addEdge(0, 2, this->labels[1]);
    graph2.addEdge(1, 2, this->labels[0]);

    EXPECT_FALSE(graph == graph2);
    EXPECT_FALSE(graph2 == graph);
}

// Test integral types only

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           addEdge_validEdge_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 2, this->labels[0]);
    this->graph.addEdge(0, 1, this->labels[1]);

    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[0] + this->labels[1]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           addEdge_selfLoop_totalEdgeNumberUpdated) {
    this->graph.addEdge(1, 1, this->labels[0]);

    EXPECT_EQ(this->graph.getTotalEdgeNumber(), this->labels[0]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           addEdge_multiedge_totalEdgeNumberUpdated) {
    this->graph.addEdge(1, 2, this->labels[0]);
    this->graph.addEdge(1, 2, this->labels[0]);

    EXPECT_EQ(this->graph.getTotalEdgeNumber(), this->labels[0]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           addEdge_multiedgeForced_totalEdgeNumberUpdated) {
    this->graph.addEdge(1, 2, this->labels[0]);
    this->graph.addEdge(1, 2, this->labels[0], true);

    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[0] + this->labels[0]);
}

template <template <class...> class Container, class... Args, typename Labels>
static void testCorrectTotalEdgeNumberForContainer(Labels &labels) {
    using EdgeLabel = typename Labels::value_type;
    Container<BaseGraph::LabeledEdge<EdgeLabel>> edges = {{0, 2, labels[0]},
                                                          {0, 1, labels[1]},
                                                          {0, 0, labels[2]},
                                                          {10, 5, labels[3]}};
    BaseGraph::EdgeLabeledDirectedGraph<EdgeLabel> graph(edges);
    EXPECT_EQ(graph.getTotalEdgeNumber(),
              labels[0] + labels[1] + labels[2] + labels[3]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           edgeListConstructor_anyContainer_correctTotalEdgeNumber) {
    testCorrectTotalEdgeNumberForContainer<std::vector>(this->labels);
    testCorrectTotalEdgeNumberForContainer<std::list>(this->labels);
    testCorrectTotalEdgeNumberForContainer<std::set>(this->labels);
    testCorrectTotalEdgeNumberForContainer<std::deque>(this->labels);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           setEdgeLabel_existentEdge_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 2, this->labels[0]);
    this->graph.addEdge(0, 1, this->labels[1]);

    this->graph.setEdgeLabel(0, 1, this->labels[0]);
    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[0] + this->labels[0]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeEdge_existentEdge_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);
    this->graph.removeEdge(0, 2);

    EXPECT_EQ(this->graph.getTotalEdgeNumber(), this->labels[0]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeEdge_existentSelfLoop_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 0, this->labels[1]);
    this->graph.removeEdge(0, 0);

    EXPECT_EQ(this->graph.getTotalEdgeNumber(), this->labels[0]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeEdge_inexistentEdge_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.removeEdge(0, 2);

    EXPECT_EQ(this->graph.getTotalEdgeNumber(), this->labels[0]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeDuplicateEdges_noMultiedge_doNothing) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);
    this->graph.addEdge(1, 1, this->labels[2]);

    this->graph.removeDuplicateEdges();

    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[0] + this->labels[1] + this->labels[2]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeDuplicateEdges_multiedge_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);
    this->graph.addEdge(0, 1, this->labels[0], true);
    this->graph.addEdge(0, 1, this->labels[0], true);
    this->graph.addEdge(1, 1, this->labels[2]);

    this->graph.removeDuplicateEdges();

    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[0] + this->labels[1] + this->labels[2]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeDuplicateEdges_multiSelfLoop_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(1, 1, this->labels[1]);
    this->graph.addEdge(1, 1, this->labels[1], true);
    this->graph.addEdge(1, 2, this->labels[2]);
    this->graph.addEdge(1, 1, this->labels[1], true);

    this->graph.removeDuplicateEdges();

    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[0] + this->labels[1] + this->labels[2]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeSelfLoops_noSelfLoop_doNothing) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);

    this->graph.removeSelfLoops();

    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[0] + this->labels[1]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeSelfLoops_existentSelfLoop_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 2, this->labels[1]);
    this->graph.addEdge(0, 0, this->labels[2]);

    this->graph.removeSelfLoops();

    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[0] + this->labels[1]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           removeVertexFromEdgeList_vertexInEdges_totalEdgeNumberUpdated) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 0, this->labels[1]);
    this->graph.addEdge(1, 2, this->labels[2]);
    this->graph.addEdge(1, 0, this->labels[3]);
    this->graph.addEdge(1, 3, this->labels[4]);

    this->graph.removeVertexFromEdgeList(0);

    EXPECT_EQ(this->graph.getTotalEdgeNumber(),
              this->labels[2] + this->labels[4]);
}

TYPED_TEST(EdgeLabeledUndirectedGraph_integral,
           clearEdges_anyGraph_graphHasNoEdge) {
    this->graph.addEdge(0, 1, this->labels[0]);
    this->graph.addEdge(0, 0, this->labels[1]);
    this->graph.addEdge(1, 2, this->labels[2]);
    this->graph.addEdge(1, 0, this->labels[3]);

    this->graph.clearEdges();

    EXPECT_EQ(this->graph.getTotalEdgeNumber(), 0);
}
