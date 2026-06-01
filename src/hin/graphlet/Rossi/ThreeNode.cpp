#include "ThreeNode.hpp"


using namespace std;


void Rossi::count3NodeForEdge(CHIN *cg,
                              NodeIdx v_i,
                              NodeIdx v_j,
                              std::unordered_set<NodeIdx> &S_i,
                              std::unordered_set<NodeIdx> &S_j,
                              std::unordered_set<NodeIdx> &T_ij,
                              CaTSCAN::ThreeNodeInfo &result) {
    // assume i < j
    TypeIdx t_i = cg->nodeTypes[v_i];
    TypeIdx t_j = cg->nodeTypes[v_j];
    TypeVector t;

    for (EdgeIdx e_ik = cg->offsets[v_i]; e_ik < cg->offsets[v_i + 1]; e_ik++) {
        NodeIdx v_k = cg->nbors[e_ik];
        if (v_k != v_j)
            S_i.insert(v_k);
    }

    for (EdgeIdx e_jk = cg->offsets[v_j]; e_jk < cg->offsets[v_j + 1]; e_jk++) {
        NodeIdx v_k = cg->nbors[e_jk];
        TypeIdx t_k = cg->nodeTypes[v_k];
        t = {t_j, t_i, t_k};    // if i and k are not connected, j is the wedge center and thus listed first
        if (v_k != v_i) {
            if (S_i.find(v_k) != S_i.end()) {
                S_i.erase(v_k);
                T_ij.insert(v_k);
                result.triangles.update("triangle", t, 1);
            } else {
                S_j.insert(v_k);
                result.wedges.update("wedge", t, 1);
            }
        }
    }

    for (auto & v_k : S_i) {
        TypeIdx t_k = cg->nodeTypes[v_k];
        t = {t_i, t_j, t_k};
        result.wedges.update("wedge", t, 1);
    }
}


ThreeNodeInfo Rossi::count3Node(CHIN *cg) {
    ThreeNodeInfo result = ThreeNodeInfo(cg->nNodes, cg->nEdges);
    for (NodeIdx v_i = 0; v_i < cg->nNodes; v_i++) {
        for (EdgeIdx e_ij = cg->offsets[v_i]; e_ij < cg->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = cg->nbors[e_ij];

            if (v_j < v_i)
                continue;

            Count d_i = cg->degree(v_i);
            Count d_j = cg->degree(v_j);

            unordered_set<NodeIdx> S_i;
            S_i.reserve(d_i - 1);
            unordered_set<NodeIdx> S_j;
            S_j.reserve(d_j - 1);
            unordered_set<NodeIdx> T_ij;
            T_ij.reserve(min(d_i, d_j) - 1);

            count3NodeForEdge(cg, v_i, v_j, S_i, S_j, T_ij, result);
        }
    }
    Rossi::correctThreeNodeCounts(result);
    return result;
}


void Rossi::correctThreeNodeCounts(CaTSCAN::ThreeNodeInfo &counts) {
    counts.triangles.total /= 3;
    for (auto & [_, count] : counts.triangles.typedTotals)
        count /= 3;
    counts.wedges.total /= 2;
    for (auto & [_, count] : counts.wedges.typedTotals)
        count /= 2;
}
