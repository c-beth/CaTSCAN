#ifndef CATSCAN_HIN_HPP
#define CATSCAN_HIN_HPP


#include <cstdint>


namespace CaTSCAN {

    using NodeIdx = int64_t;
    using TypeIdx = int64_t;
    using EdgeIdx = int64_t;
    using Count = int64_t;

    const EdgeIdx invalidEdge = -1;


    // ------------------------------------------------------------------------------------------------
    // HIN in COO representation. These are shallow POD objects and can be copied freely.
    // Memory management is always done explicitly.
    // ------------------------------------------------------------------------------------------------

    struct HIN {
        NodeIdx nNodes;     // number of nodes in the graph
        EdgeIdx nEdges;     // number of edges in this list
        TypeIdx nTypes;     // number of node nodeTypes in the graph
        NodeIdx *srcs;      // array of source nodes
        NodeIdx *dsts;      // array of destination nodes
        TypeIdx *types;     // array of node nodeTypes

        [[nodiscard]] HIN copy() const;
    };

    // Allocate memory to hold a graph.
    HIN newHIN(NodeIdx nNodes, EdgeIdx nEdges, TypeIdx nTypes);

    // Release memory associated with the graph. Should have been allocated with newHIN.
    void delHIN(HIN g);

    struct Pair {
        NodeIdx first;
        NodeIdx second;
    };

    // comparator that compares the second in pair. If they are equal, then compare first in pair
    bool pairCompareSecond(Pair firstPair, Pair nextPair);


    // ------------------------------------------------------------------------------------------------
    // HIN in CSR representation.
    // ------------------------------------------------------------------------------------------------

    struct CHIN {
        NodeIdx nNodes;         // number of nodes in the graph
        EdgeIdx nEdges;         // number of edges in the graph
        TypeIdx nTypes;         // number of node nodeTypes in the graph
        EdgeIdx *offsets;       // node v's edges are [offset[v], offset[v + 1])
        EdgeIdx *t_offsets;     // relative type offsets (mark the end of nbors of that type).  Node v's edges of type t  [offset[v] + t_offsets[v * nTypes + t - 1], offset[v] + t_offsets[v * nTypes + t])
        NodeIdx *nbors;         // incoming or outgoing neighbors --> neighbors of v are [nbors[offset[v]], nbors[offset[v+1]]) this interval is sorted w.r.t. types
        TypeIdx *nodeTypes;     // array of node nodeTypes

        [[nodiscard]] CHIN copy() const;

        // Checks if edge (v1, v2) is present
        [[nodiscard]] bool isEdge(NodeIdx v1, NodeIdx v2) const;

        // Returns the index of the edge v1 -> v2 in the nbor list nbors.
        // Returns invalidEdge if v1 -> v2 does not exist
        [[nodiscard]] EdgeIdx getEdge(NodeIdx v1, NodeIdx v2) const;

        // Returns the index of the edge v1 -> v2 in the nbor list nbors or index of edge v2->v1.
        // Depending on how the edge is directed
        // Returns invalidEdge if v1 -> v2 or v2->v1 does not exist
        [[nodiscard]] EdgeIdx getEdgeUndirected(NodeIdx v1, NodeIdx v2) const;

        // Groups each individual adjacency list (segment in nbors between offsets) by destination types.
        // This is a prerequisite to build correct type offsets.
        void groupByType() const;

        // This sorts each individual typed adjacency list by vertex ID. This is useful for
        // doing a binary search, or for merging neighbor lists to find common neighbors.
        void sortById() const;

        // This groups the individual adjacency lists by nodeTypes and sets the typed offsets.
        void buildTypeOffsets() const;

        // This outputs a new, isomorphic CHIN where node labels are in increasing order corresponding to degree.
        // Thus, (after the relabeling), for all node IDs i < j, the degree of i is less than that of j.
        [[nodiscard]] CHIN renameByDegreeOrder() const;

        // Returns the overall degree of a node, irrespective of the neighbor types.
        [[nodiscard]] Count degree(NodeIdx v) const { return offsets[v + 1] - offsets[v]; }

        // Returns the degree constrained within a specified node type.
        [[nodiscard]] Count type_degree(NodeIdx v, TypeIdx t) const { return abs_type_offset(v, t + 1) - abs_type_offset(v, t); };

        // converts relative offsets to absolute offsets that behave just like un-typed offsets.
        [[nodiscard]] EdgeIdx abs_type_offset(NodeIdx v, TypeIdx t) const;

    };


    // Allocate memory for a CSR graph
    CHIN newCHIN(NodeIdx nNodes, EdgeIdx nEdges, TypeIdx nTypes);

    // Release memory associated with a CSR graph. Should have been allocated with newCHIN.
    void delCHIN(CHIN g);

    // Make a CSR HIN from a COO HIN. The input graph is destroyed, i.e. you should not call delCHIN on it.
    CHIN makeCSR(HIN g);


    // ------------------------------------------------------------------------------------------------
    // Directed Compressed HIN.
    // ------------------------------------------------------------------------------------------------

    // Contains two HINs in CSR format, where the respective nbors-list only contains either out-neighbors or in-neighbors.
    struct DCHIN {
        CHIN out_list;
        CHIN in_list;
    };

    // Construct a DAG version of a CHIN based on degree ordering
    DCHIN degreeOrdered(CHIN *g);


}   // namespace

#endif // CATSCAN_HIN_HPP
