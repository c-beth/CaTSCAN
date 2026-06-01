#include "HIN.hpp"
#include "JointSort.hpp"
#include <algorithm>


using namespace CaTSCAN;
using namespace std;


bool CaTSCAN::pairCompareSecond(Pair firstPair, Pair nextPair) {
    if (firstPair.second != nextPair.second)
        return firstPair.second < nextPair.second;
    return firstPair.first < nextPair.first;
}



// ------------------------------------------------------------------------------------------------
// Regular HIN in COO format
// ------------------------------------------------------------------------------------------------



HIN CaTSCAN::newHIN(NodeIdx nNodes, EdgeIdx nEdges, TypeIdx nTypes) {
    return {nNodes,
            nEdges,
            nTypes,
            new NodeIdx[nEdges],
            new NodeIdx[nEdges],
            new TypeIdx[nNodes]};
}


HIN CaTSCAN::HIN::copy() const {
    HIN ret = newHIN(nNodes, nEdges, nTypes);
    std::copy(srcs, srcs + nEdges, ret.srcs);
    std::copy(dsts, dsts + nEdges, ret.dsts);
    return ret;
}


void CaTSCAN::delHIN(HIN g) {
    delete[] g.srcs;
    delete[] g.dsts;
    delete[] g.types;
}



// ------------------------------------------------------------------------------------------------
// Compressed HIN in CSR format
// ------------------------------------------------------------------------------------------------



CHIN CaTSCAN::newCHIN(NodeIdx nNodes, EdgeIdx nEdges, TypeIdx nTypes) {
    return {nNodes,
            nEdges,
            nTypes,
            new EdgeIdx[nNodes + 1],
            new EdgeIdx[nNodes * nTypes],
            new NodeIdx[nEdges],
            new TypeIdx[nNodes]};
}


void CaTSCAN::delCHIN(CHIN g) {
    delete[] g.offsets;
    delete[] g.t_offsets;
    delete[] g.nbors;
    delete[] g.nodeTypes;
}


CHIN CHIN::copy() const {
    CHIN ret = newCHIN(nNodes, nEdges, nTypes);
    std::copy(offsets, offsets + nNodes + 1, ret.offsets);
    std::copy(t_offsets, t_offsets + nNodes * (nTypes + 1), ret.t_offsets);
    std::copy(nbors, nbors + nEdges, ret.nbors);
    std::copy(nodeTypes, nodeTypes + nNodes, ret.nodeTypes);
    return ret;
}


bool CHIN::isEdge(NodeIdx v1, NodeIdx v2) const {
    if (v1 >= nNodes)
        return false;
    TypeIdx t2 = nodeTypes[v2];
    EdgeIdx low = abs_type_offset(v1, t2);
    EdgeIdx high = abs_type_offset(v1, t2 + 1) - 1;
    EdgeIdx mid;
    while (low <= high) {
        mid = (low + high) / 2;
        if (nbors[mid] == v2)
            return true;
        if (nbors[mid] > v2)
            high = mid - 1;
        else
            low = mid + 1;
    }
    return false;
}


EdgeIdx CHIN::getEdge(NodeIdx v1, NodeIdx v2) const {
    if (v1 >= nNodes)
        return invalidEdge;
    TypeIdx t2 = nodeTypes[v2];
    EdgeIdx low = abs_type_offset(v1, t2);
    EdgeIdx high = abs_type_offset(v1, t2 + 1) - 1;
    EdgeIdx mid;
    while(low <= high) {
        mid = (low + high) / 2;
        if (nbors[mid] == v2)
            return mid;
        if (nbors[mid] > v2)
            high = mid - 1;
        else
            low = mid + 1;
    }
    return invalidEdge;
}


EdgeIdx CHIN::getEdgeUndirected(NodeIdx v1, NodeIdx v2) const {
    if (isEdge(v1, v2))
        return getEdge(v1, v2);
    else if (isEdge(v2, v1))
        return getEdge(v2,v1);
    else
        return invalidEdge;
}


void CHIN::groupByType() const {
    auto *dst_types = new TypeIdx[nEdges];
    for (EdgeIdx i = 0; i < nEdges; i++)
        dst_types[i] = nodeTypes[nbors[i]];
    for (NodeIdx i = 0; i < nNodes; i++) {
        auto begin = JSIterator<NodeIdx, NodeIdx>{dst_types + offsets[i], nbors + offsets[i]};
        auto end = begin + degree(i);
        std::sort(begin, end);
    }
    delete[] dst_types;
}


void CHIN::sortById() const {
    for (NodeIdx i = 0; i < nNodes; i++)
        for (TypeIdx t = 0; t < nTypes; t++)
            std::sort(nbors + abs_type_offset(i, t),
                      nbors + abs_type_offset(i, t + 1));
}


void CHIN::buildTypeOffsets() const {
    TypeIdx type_offset;
    NodeIdx rel_offset;
    for (NodeIdx i = 0; i < nNodes; i++) {
        type_offset = 0;
        rel_offset = 0;
        for (NodeIdx idx = offsets[i]; idx < offsets[i + 1]; idx++) {
            while (type_offset < nodeTypes[nbors[idx]]) {
                t_offsets[i * nTypes + type_offset] = rel_offset;
                type_offset++;
            }
            rel_offset++;
        }
        // in case we ran out of edges but did not encounter the last type(s)
        while (type_offset < nTypes) {
            t_offsets[i * nTypes + type_offset] = rel_offset;
            type_offset++;
        }
    }
}


CHIN CHIN::renameByDegreeOrder() const {
    CHIN ret = newCHIN(nNodes, nEdges, nTypes);
    Pair *deg_info = new Pair[nNodes];

    auto *mapping = new NodeIdx[nNodes];
    auto *inverse = new NodeIdx[nNodes];

    // Construct array of pairs, storing old node label and degree
    for (NodeIdx i = 0; i < nNodes; i++) {
        deg_info[i].first = i;
        deg_info[i].second = degree(i);
    }

    // sort the pairs by degree (if degree is same, sort by old node label)
    std::sort(deg_info, deg_info + nNodes, CaTSCAN::pairCompareSecond);

    // Construct the mapping of old node label to new node label
    // So mapping[i] is what i is mapped to
    // And inverse[i] is what maps to i
    for (NodeIdx i = 0; i < nNodes; i++) {
        mapping[deg_info[i].first] = i;
        inverse[i] = deg_info[i].first;
    }

    // Initialize offsets of output CGraph
    ret.offsets[0] = 0;
    EdgeIdx current = 0;

    NodeIdx old_label, old_nbr, new_nbr;
    // Loop over new nodes
    for (NodeIdx new_label = 0; new_label < nNodes; new_label++) {
        old_label = inverse[new_label];     // corresponding old label for new nodes
        // Loop over neighbors of old label
        for (EdgeIdx pos = offsets[old_label]; pos < offsets[old_label + 1]; pos++) {
            old_nbr = nbors[pos];
            new_nbr = mapping[old_nbr];     // corresponding new neighbor
            ret.nbors[current] = new_nbr;           // insert new neighbor in nbors of output
            current++;
        }
        // all neighbors of new_label have been added, so we set offset for new_label+1
        ret.offsets[new_label + 1] = current;
        ret.nodeTypes[new_label] = nodeTypes[old_label];
    }

    ret.groupByType();
    ret.buildTypeOffsets();
    ret.sortById();
    delete[] deg_info;
    delete[] mapping;
    delete[] inverse;
    return ret;
}


EdgeIdx CHIN::abs_type_offset(NodeIdx v, TypeIdx t) const{
    if (t == 0)
        return offsets[v];
    else
        return offsets[v] + t_offsets[v * nTypes + t - 1];
}


CHIN CaTSCAN::makeCSR(HIN G) {
    auto begin = JSIterator<NodeIdx, NodeIdx>{G.srcs, G.dsts};
    auto end = begin + G.nEdges;
    std::sort(begin, end);

    CHIN ret = {G.nNodes,
                G.nEdges,
                G.nTypes,
                new EdgeIdx[G.nNodes + 1],
                new EdgeIdx[G.nNodes * G.nTypes],
                new NodeIdx[G.nEdges],
                new TypeIdx[G.nNodes]
    };
    std::copy(G.dsts, G.dsts + G.nEdges, ret.nbors);
    std::copy(G.types, G.types + G.nNodes, ret.nodeTypes);

    // now that we have everything sorted by src, compress:
    NodeIdx current_vertex = 0;
    for (EdgeIdx i = 0; i < G.nEdges; ++i) {
        auto src = G.srcs[i];
        while (current_vertex <= src) {
            ret.offsets[current_vertex] = i;
            current_vertex++;
        }
    }
    // if nodes are left after processing all edges, assign nEdges as offset
    while (current_vertex <= G.nNodes) {
        ret.offsets[current_vertex] = G.nEdges;
        current_vertex++;
    }

    ret.groupByType();
    ret.buildTypeOffsets();
    ret.sortById();

    return ret;
}



// ------------------------------------------------------------------------------------------------
// Compressed Directed HIN.
// ------------------------------------------------------------------------------------------------


DCHIN CaTSCAN::degreeOrdered(CHIN *g) {
    DCHIN ret{};
    CHIN in_hin = {g->nNodes,
                   0,
                   g->nTypes,
                   new EdgeIdx[g->nNodes + 1],
                   new EdgeIdx[g->nNodes * (g->nTypes + 1)],
                   new NodeIdx[g->nEdges / 2],
                   new TypeIdx[g->nNodes]};
    CHIN out_hin = {g->nNodes,
                    0,
                    g->nTypes,
                    new EdgeIdx[g->nNodes + 1],
                    new EdgeIdx[g->nNodes * (g->nTypes + 1)],
                    new NodeIdx[g->nEdges / 2],
                    new TypeIdx[g->nNodes]};

    std::copy(g->nodeTypes, g->nodeTypes + g->nNodes, in_hin.nodeTypes);
    std::copy(g->nodeTypes, g->nodeTypes + g->nNodes, out_hin.nodeTypes);

    EdgeIdx cur_in = 0;
    EdgeIdx cur_out = 0;
    in_hin.offsets[0] = 0;
    out_hin.offsets[0] = 0;

    for (NodeIdx i = 0; i < g->nNodes; i++) {
        Count deg_i = g->degree(i);

        for (EdgeIdx e_ij = g->offsets[i]; e_ij < g->offsets[i + 1]; e_ij++) {
            NodeIdx j = g->nbors[e_ij];
            Count deg_j = g->degree(j);

            // assign edge to in or out based on degree ordering
            if (deg_i < deg_j or (deg_i == deg_j and i < j)) {
                out_hin.nbors[cur_out] = j;
                out_hin.nEdges++;
                cur_out++;
            } else {
                in_hin.nbors[cur_in] = j;
                in_hin.nEdges++;
                cur_in++;
            }
        }
        // Finished all edges incident to i; update offsets in DAGs.
        in_hin.offsets[i + 1] = cur_in;
        out_hin.offsets[i + 1] = cur_out;
    }

    out_hin.groupByType();
    out_hin.buildTypeOffsets();
    out_hin.sortById();

    in_hin.groupByType();
    in_hin.buildTypeOffsets();
    in_hin.sortById();

    ret.out_list = out_hin;
    ret.in_list = in_hin;
    return ret;
}
