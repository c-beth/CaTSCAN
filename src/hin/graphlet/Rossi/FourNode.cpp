#include "FourNode.hpp"
#include "ThreeNode.hpp"


namespace Rossi {


    void count4NodeForEdge(CHIN *cg,
                           NodeIdx v_i,
                           NodeIdx v_j,
                           unordered_set<NodeIdx> &S_i,
                           unordered_set<NodeIdx> &S_j,
                           unordered_set<NodeIdx> &T_ij,
                           FourNodeInfo &result) {

        TypeIdx t_i = cg->nodeTypes[v_i];
        TypeIdx t_j = cg->nodeTypes[v_j];
        TypeVector t;

        // triangle-based graphlets
        for (auto const & v_k : T_ij) {
            TypeIdx t_k = cg->nodeTypes[v_k];
            for (EdgeIdx e_kr = cg->offsets[v_k]; e_kr < cg->offsets[v_k + 1]; e_kr++) {
                NodeIdx v_r = cg->nbors[e_kr];
                if (v_r == v_i or v_i == v_j)
                    continue;
                TypeIdx t_r = cg->nodeTypes[v_r];
                if (v_r < v_k and T_ij.find(v_r) != T_ij.end()) {   // 4-clique
                    t = {t_i, t_j, t_k, t_r};
                    result.fourCliques.update("4-clique", t, 1);
                } else if (S_i.find(v_r) != S_i.end()) {    // diamond with chord from i to k
                    t = {t_i, t_k, t_j, t_r};
                    result.diamonds.update("diamond", t, 1);
                } else if (S_j.find(v_r) != S_j.end()) {    // diamond with chord from j to k
                    t = {t_j, t_k, t_i, t_r};
                    result.diamonds.update("diamond", t, 1);
                } else if (not cg->isEdge(v_r, v_i) and not cg->isEdge(v_r, v_j)) {    // tailed triangle
                    t = {t_k, t_r, t_i, t_j};
                    result.tailedTriangles.update("tailed-triangle", t, 1);
                }
            }
        }

        // path-based graphlets
        for (auto const & v_k : S_i) {
            TypeIdx t_k = cg->nodeTypes[v_k];
            for (EdgeIdx e_kr = cg->offsets[v_k]; e_kr < cg->offsets[v_k + 1]; e_kr++) {
                NodeIdx v_r = cg->nbors[e_kr];
                if (v_r == v_i or v_r == v_j)
                    continue;
                TypeIdx t_r = cg->nodeTypes[v_r];
                if (not cg->isEdge(v_r, v_i) and not cg->isEdge(v_r, v_j)) {    // 3-path
                    t = {t_r, t_k, t_i, t_j};
                    result.threePaths.update("3-path", t, 1);
                } else if (v_r < v_k and S_i.find(v_r) != S_i.end()) {  // tailed triangle
                    t = {t_i, t_j, t_k, t_r};
                    result.tailedTriangles.update("tailed-triangle", t, 1);
                }
            }
        }
        for (auto const & v_k : S_j) {
            TypeIdx t_k = cg->nodeTypes[v_k];
            for (EdgeIdx e_kr = cg->offsets[v_k]; e_kr < cg->offsets[v_k + 1]; e_kr++) {
                NodeIdx v_r = cg->nbors[e_kr];
                if (v_r == v_i or v_r == v_j)
                    continue;
                TypeIdx t_r = cg->nodeTypes[v_r];
                if (not cg->isEdge(v_r, v_i) and not cg->isEdge(v_r, v_j)) {    // 3-path
                    t = {t_i, t_j, t_k, t_r};
                    result.threePaths.update("3-path", t, 1);
                } else if (v_r < v_k and S_j.find(v_r) != S_j.end()) {  // tailed triangle
                    t = {t_j, t_i, t_k, t_r};
                    result.tailedTriangles.update("tailed-triangle", t, 1);
                } else if (S_i.find(v_r) != S_i.end()) {
                    t = {t_i, t_j, t_k, t_r};
                    result.fourCycles.update("4-cycle", t, 1);
                }
            }
        }

        // derive rest with combinatorics
        for (TypeIdx t1 = 0; t1 < cg->nTypes; t1++) {
            for (TypeIdx t2 = t1; t2 < cg->nTypes; t2++) {
                unordered_set<NodeIdx> S_i_t1;
                unordered_set<NodeIdx> S_i_t2;
                unordered_set<NodeIdx> S_j_t1;
                unordered_set<NodeIdx> S_j_t2;
                unordered_set<NodeIdx> T_ij_t1;
                unordered_set<NodeIdx> T_ij_t2;

                for (auto const & v_k : S_i) {
                    if (cg->nodeTypes[v_k] == t1)
                        S_i_t1.insert(v_k);
                    else if (cg->nodeTypes[v_k] == t2)
                        S_i_t2.insert(v_k);
                }
                for (auto const & v_k : S_j) {
                    if (cg->nodeTypes[v_k] == t1)
                        S_j_t1.insert(v_k);
                    else if (cg->nodeTypes[v_k] == t2)
                        S_j_t2.insert(v_k);
                }
                for (auto const & v_k : T_ij) {
                    if (cg->nodeTypes[v_k] == t1)
                        T_ij_t1.insert(v_k);
                    else if (cg->nodeTypes[v_k] == t2)
                        T_ij_t2.insert(v_k);
                }

                // 3-paths (need to correct by subtracting tailed triangles)
                Count threePaths;
                if (t1 == t2) {
                    t = {t1, t_i, t_j, t1};
                    threePaths = S_i_t1.size() * S_j_t1.size();
                    result.threePaths.update("3-path", t, threePaths);
                } else {
                    t = {t1, t_i, t_j, t2};
                    threePaths = S_i_t1.size() * S_j_t2.size();
                    result.threePaths.update("3-path", t, threePaths);
                    t = {t2, t_i, t_j, t1};
                    threePaths = S_i_t2.size() * S_j_t1.size();
                    result.threePaths.update("3-path", t, threePaths);
                }

                // 3-stars (need to correct by subtracting tailed triangles)
                Count threeStars;
                if (t1 == t2) {
                    t = {t_i, t_j, t1, t2};  // v_i in star center
                    threeStars = S_i_t1.size() * (S_i_t1.size() - 1) / 2;   // S_i_t1.size() choose 2
                    result.threeStars.update("3-star", t, threeStars);
                    t = {t_j, t_i, t1, t2}; // v_j in star center
                    threeStars = S_j_t1.size() * (S_j_t1.size() - 1) / 2;   // S_j_t1.size() choose 2
                    result.threeStars.update("3-star", t, threeStars);
                } else {
                    t = {t_i, t_j, t1, t2};  // v_i in star center
                    threeStars = S_i_t1.size() * S_i_t2.size();
                    result.threeStars.update("3-star", t, threeStars);
                    t = {t_j, t_i, t1, t2}; // v_j in star center
                    threeStars = S_j_t1.size() * S_j_t2.size();
                    result.threeStars.update("3-star", t, threeStars);
                }

                // tailed triangles (need to correct by subtracting diamonds)
                Count tailedTriangles;
                if (t1 == t2) {
                    t = {t_i, t1, t_j, t1};
                    tailedTriangles = T_ij_t1.size() * S_i_t1.size();
                    result.tailedTriangles.update("tailed-triangle", t, tailedTriangles);
                    t = {t_j, t1, t_i, t1};
                    tailedTriangles = T_ij_t1.size() * S_j_t1.size();
                    result.tailedTriangles.update("tailed-triangle", t, tailedTriangles);
                } else {
                    t = {t_i, t1, t_j, t2};
                    tailedTriangles = T_ij_t2.size() * S_i_t1.size();
                    result.tailedTriangles.update("tailed-triangle", t, tailedTriangles);
                    t = {t_i, t2, t_j, t1};
                    tailedTriangles = T_ij_t1.size() * S_i_t2.size();
                    result.tailedTriangles.update("tailed-triangle", t, tailedTriangles);
                    t = {t_j, t1, t_i, t2};
                    tailedTriangles = T_ij_t2.size() * S_j_t1.size();
                    result.tailedTriangles.update("tailed-triangle", t, tailedTriangles);
                    t = {t_j, t2, t_i, t1};
                    tailedTriangles = T_ij_t1.size() * S_j_t2.size();
                    result.tailedTriangles.update("tailed-triangle", t, tailedTriangles);
                }

                // diamonds (need to correct by subtracting 4-cliques)
                Count diamonds;
                t = {t_i, t_j, t1, t2};
                if (t1 == t2)
                    diamonds = T_ij_t1.size() * (T_ij_t1.size() - 1) / 2;   // T_ij_t1.size() choose 2
                else
                    diamonds = T_ij_t1.size() * T_ij_t2.size();
                result.diamonds.update("diamond", t, diamonds);

            }
        }
    }


    CountInfo count3And4Node(CHIN *cg) {
        CountInfo result = CountInfo(cg->nNodes, cg->nEdges);
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

                count3NodeForEdge(cg, v_i, v_j, S_i, S_j, T_ij, result.threeNode);
                count4NodeForEdge(cg, v_i, v_j, S_i, S_j, T_ij, result.fourNode);
            }
        }
        correctThreeNodeCounts(result.threeNode);
        correctFourNodeCounts(result.fourNode);
        return result;
    }


    void correctFourNodeCounts(FourNodeInfo &counts) {

        TypeVector t, t1;

        counts.fourCliques.total /= 6;
        for (auto & [_, count] : counts.fourCliques.typedTotals)
            count /= 6;

        for (auto & [type_hash, cliques] : counts.fourCliques.typedTotals) {
            t = decodeHash(type_hash);
            t1 = {t[0], t[1], t[2], t[3]};
            counts.diamonds.update("diamond", t1, -cliques);
            t1 = {t[0], t[2], t[1], t[3]};
            counts.diamonds.update("diamond", t1, -cliques);
            t1 = {t[0], t[3], t[1], t[2]};
            counts.diamonds.update("diamond", t1, -cliques);
            t1 = {t[1], t[2], t[0], t[3]};
            counts.diamonds.update("diamond", t1, -cliques);
            t1 = {t[1], t[3], t[0], t[2]};
            counts.diamonds.update("diamond", t1, -cliques);
            t1 = {t[2], t[3], t[0], t[1]};
            counts.diamonds.update("diamond", t1, -cliques);
        }

        counts.diamonds.total /= 5;
        for (auto & [_, count] : counts.diamonds.typedTotals)
            count /= 5;

        for (auto & [type_hash, diamonds] : counts.diamonds.typedTotals) {
            t = decodeHash(type_hash);
            t1 = {t[0], t[2], t[1], t[3]};
            counts.tailedTriangles.update("tailed-triangle", t1, -diamonds);
            t1 = {t[0], t[3], t[1], t[2]};
            counts.tailedTriangles.update("tailed-triangle", t1, -diamonds);
            t1 = {t[1], t[2], t[0], t[3]};
            counts.tailedTriangles.update("tailed-triangle", t1, -diamonds);
            t1 = {t[1], t[3], t[0], t[2]};
            counts.tailedTriangles.update("tailed-triangle", t1, -diamonds);
        }

        counts.tailedTriangles.total /= 4;
        for (auto & [_, count] : counts.tailedTriangles.typedTotals)
            count /= 4;

        for (auto & [type_hash, tailedTriangles] : counts.tailedTriangles.typedTotals) {
            t = decodeHash(type_hash);
            counts.threeStars.update("3-star", t, -tailedTriangles);
        }

        counts.threeStars.total /= 3;
        for (auto & [_, count] : counts.threeStars.typedTotals)
            count /= 3;

        counts.fourCycles.total /= 4;
        for (auto & [_, count] : counts.fourCycles.typedTotals)
            count /= 4;

        for (auto & [type_hash, fourCycles] : counts.fourCycles.typedTotals) {
            t = decodeHash(type_hash);
            t1 = {t[0], t[1], t[2], t[3]};
            counts.threePaths.update("3-path", t1, -fourCycles);
            t1 = {t[1], t[2], t[3], t[0]};
            counts.threePaths.update("3-path", t1, -fourCycles);
            t1 = {t[2], t[3], t[0], t[1]};
            counts.threePaths.update("3-path", t1, -fourCycles);
            t1 = {t[3], t[0], t[1], t[2]};
            counts.threePaths.update("3-path", t1, -fourCycles);
        }

        counts.threePaths.total /= 3;
        for (auto & [_, count] : counts.threePaths.typedTotals)
            count /= 3;
    }


}   // namespace Rossi
