#include "FiveNode.hpp"


// Pattern 7: Tailed-Four-Cycle
//
//      0
//      |
//      |
//      1-----3
//      |     |
//      |     |
//      2-----4
//
// Method:
//   Use decomposition into two wedges.
//   Combine out-out/in-out out-out/out-out in-out/in-out wedges to get cycles.
//   Count combine tail on all wedges with the number of wedges that share the same outer nodes.
//   Count tails from wedge center double so that all tails are counted twice.
//   In the end divide by two.
//   Correct for case that tail matches center node of second wedge (diamond).
//
// Arguments:
//   g: CSR  of the graph
//   gout: CSR (out-edges) of the graph
//   gin: CSR (in-edges) of the graph
//   fourNodeInfo: Counts + Info of 4-node graphlets
//
// Return:
//   Total count of pattern 7 in the graph

GraphletInfo CaTSCAN::countTailedFourCycles(CHIN *g,
                                            CHIN *gout,
                                            CHIN *gin,
                                            FourNodeInfo& fourNodeInfo) {

    GraphletInfo ret;
    unordered_map<string, double> typedTailedFourCycles;

    // for a node i, store the wedge count to all other nodes k that form a wedge with connecting type t_j: i-j-k
    auto *outout_count = new Count[gout->nNodes * gout->nTypes];
    auto *inout_count = new Count[gout->nNodes * gout->nTypes];

    double type1tailed, type2tailed, type3tailed;
    TypeVector types;
    string typeHash;

    // initialize the wedge_count array with 0s
    for (NodeIdx v_i = 0; v_i < g->nNodes * g->nTypes; v_i++) {
        outout_count[v_i] = 0;
        inout_count[v_i] = 0;
    }

    for (NodeIdx v_i = 0; v_i < gin->nNodes; v_i++) {
        TypeIdx t_i = gin->nodeTypes[v_i];

        // calculate count for all outout and inout wedges
        for (EdgeIdx e_ji = gin->offsets[v_i]; e_ji < gin->offsets[v_i + 1]; e_ji++) {
            NodeIdx v_j = gin->nbors[e_ji];
            TypeIdx t_j = gin->nodeTypes[v_j];

            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];    // i <- j -> k is outout wedge centered at j
                if (v_k == v_i)
                    continue;
                outout_count[v_k * g->nTypes + t_j]++;
            }

            for (EdgeIdx e_kj = gin->offsets[v_j]; e_kj < gin->offsets[v_j + 1]; e_kj++) {
                NodeIdx v_k = gin->nbors[e_kj];     // i <- j <- k is inout wedge centered at j
                if (v_k == v_i)
                    continue;
                inout_count[v_k * g->nTypes + t_j]++;
            }
        }

        // we loop over all outout and inout wedges again. By keeping track of the edges and triangles
        // incident to each wedge, and using outout_count and inout_count, we can get the final pattern count
        for (EdgeIdx e_ji = gin->offsets[v_i]; e_ji < gin->offsets[v_i + 1]; e_ji++) {
            NodeIdx v_j = gin->nbors[e_ji];
            TypeIdx t_j = gin->nodeTypes[v_j];

            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];    // i <- j -> k is outout wedge
                TypeIdx t_k = gout->nodeTypes[v_k];

                if (v_i == v_k)
                    continue;

                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    if (v_k < v_i)
                        continue;

                    // try each possible type as closing wedge
                    Count deg_outout = outout_count[v_k * g->nTypes + t_r];
                    if (t_j == t_r)
                        deg_outout--;
                    if (deg_outout == 0)
                        continue;

                    for (TypeIdx t_tail = 0; t_tail < g->nTypes; t_tail++) {
                        // try each possible type as tail type.
                        // Need to consider that it may be the same type as an adjacent wedge node.
                        Count deg_i_tail = g->type_degree(v_i, t_tail);     // degree of v_i with type t_tail
                        Count deg_j_tail = g->type_degree(v_j, t_tail);     // degree of v_j with type t_tail
                        Count deg_k_tail = g->type_degree(v_k, t_tail);     // degree of v_k with type t_tail
                        if (t_tail == t_i)
                            deg_j_tail--;
                        if (t_tail == t_k)
                            deg_j_tail--;
                        if (t_tail == t_j) {
                            deg_i_tail--;
                            deg_k_tail--;
                        }
                        if (t_tail == t_r) {
                            deg_i_tail--;
                            deg_k_tail--;
                        }

                        if (deg_i_tail == 0 and deg_j_tail == 0 and deg_k_tail == 0)
                            continue;
                        // type1tailed: outout with outout
                        type1tailed = (double) deg_i_tail / 2 * (double) deg_outout;
                        types = {t_tail, t_i, t_j, t_r, t_k};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type1tailed;

                        if (deg_j_tail == 0 and deg_k_tail == 0)
                            continue;
                        type1tailed = (double) deg_j_tail * (double) deg_outout;   // this should not be divided by 2
                        types = {t_tail, t_j, t_i, t_k, t_r};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type1tailed;

                        if (deg_k_tail == 0)
                            continue;
                        type1tailed = (double) deg_k_tail / 2 * (double) deg_outout;
                        types = {t_tail, t_k, t_j, t_r, t_i};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type1tailed;
                    }
                }

                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    Count deg_inout = inout_count[v_k * g->nTypes + t_r];
                    if (deg_inout == 0)
                        continue;

                    for (TypeIdx t_tail = 0; t_tail < g->nTypes; t_tail++) {
                        // try each possible type as tail type.
                        // Need to consider that it may be the same type as an adjacent wedge node.
                        Count deg_i_tail = g->type_degree(v_i, t_tail);     // degree of v_i with type t_tail
                        Count deg_j_tail = g->type_degree(v_j, t_tail);     // degree of v_j with type t_tail
                        Count deg_k_tail = g->type_degree(v_k, t_tail);     // degree of v_k with type t_tail
                        if (t_tail == t_i)
                            deg_j_tail--;
                        if (t_tail == t_k)
                            deg_j_tail--;
                        if (t_tail == t_j) {
                            deg_i_tail--;
                            deg_k_tail--;
                        }
                        if (t_tail == t_r) {
                            deg_i_tail--;
                            deg_k_tail--;
                        }

                        if (deg_i_tail == 0 and deg_j_tail == 0 and deg_k_tail == 0)
                            continue;
                        // type3tailed: outout with inout
                        type3tailed = (double) deg_i_tail * (double) deg_inout;
                        types = {t_tail, t_i, t_j, t_r, t_k};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type3tailed;

                        if (deg_j_tail == 0 and deg_k_tail == 0)
                            continue;
                        type3tailed = (double) deg_j_tail * (double) deg_inout;
                        types = {t_tail, t_j, t_i, t_k, t_r};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type3tailed;

                        if (deg_k_tail == 0)
                            continue;
                        type3tailed = (double) deg_k_tail * (double) deg_inout;
                        types = {t_tail, t_k, t_j, t_r, t_i};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type3tailed;
                    }
                }
            }

            for (EdgeIdx e_kj = gin->offsets[v_j]; e_kj < gin->offsets[v_j + 1]; e_kj++) {
                NodeIdx v_k = gin->nbors[e_kj];
                TypeIdx t_k = gin->nodeTypes[v_k];

                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    Count deg_outout = outout_count[v_k * g->nTypes + t_r];
                    if (deg_outout == 0)
                        continue;

                    for (TypeIdx t_tail = 0; t_tail < g->nTypes; t_tail++) {
                        Count deg_j_tail = g->type_degree(v_j, t_tail);
                        if (t_tail == t_i)
                            deg_j_tail--;
                        if (t_tail == t_k)
                            deg_j_tail--;

                        if (deg_j_tail == 0)
                            continue;
                        // type3tailed: inout with outout
                        type1tailed = (double) deg_j_tail * (double) deg_outout;
                        types = {t_tail, t_j, t_i, t_k, t_r};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type1tailed;
                    }
                }

                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    Count deg_inout = inout_count[v_k * g->nTypes + t_r];
                    if (t_j == t_r)
                        deg_inout--;
                    if (deg_inout == 0)
                        continue;

                    for (TypeIdx t_tail = 0; t_tail< g->nTypes; t_tail++) {
                        Count deg_i_tail = g->type_degree(v_i, t_tail);
                        Count deg_j_tail = g->type_degree(v_j, t_tail);
                        Count deg_k_tail = g->type_degree(v_k, t_tail);
                        if (t_tail == t_i)
                            deg_j_tail--;
                        if (t_tail == t_k)
                            deg_j_tail--;
                        if (t_tail == t_j) {
                            deg_i_tail--;
                            deg_k_tail--;
                        }
                        if (t_tail == t_r) {
                            deg_i_tail--;
                            deg_k_tail--;
                        }

                        if (deg_i_tail == 0 and deg_j_tail == 0 and deg_k_tail == 0)
                            continue;
                        // type2tailed: inout with inout
                        type2tailed = (double) deg_i_tail / 2 * (double) deg_inout;
                        types = {t_tail, t_i, t_j, t_r, t_k};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type2tailed;


                        if (deg_j_tail == 0 and deg_k_tail == 0)
                            continue;
                        type2tailed = (double) deg_j_tail * (double) deg_inout;     // this should not be divided by 2
                        types = {t_tail, t_j, t_i, t_k, t_r};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type2tailed;


                        if (deg_k_tail == 0)
                            continue;
                        type2tailed = (double) deg_k_tail / 2 * (double) deg_inout;
                        types = {t_tail, t_k, t_j, t_r, t_i};
                        typeHash = iso(types, "tailed-4-cycle");
                        typedTailedFourCycles[typeHash] += type2tailed;
                    }
                }
            }
        }

        // clearing outout_count and inout_count
        // loop over in-neighbors of i
        for (EdgeIdx e_ij = gin->offsets[v_i]; e_ij < gin->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gin->nbors[e_ij]; // j is current in-neighbor
            TypeIdx t_j = gin->nodeTypes[v_j];
            // loop over out-neighbors of j, note this gives an outout wedge
            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout-> offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];  // i <- j -> k is outout wedge centered at j
                outout_count[v_k * g->nTypes + t_j] = 0;
            }
            // loop over in-neighbors of j, note this gives inout wedge
            for (EdgeIdx e_jk = gin->offsets[v_j]; e_jk < gin-> offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gin->nbors[e_jk];
                inout_count[v_k * g->nTypes + t_j] = 0;
            }
        }
    }

    // convert to GraphletInfo
    for (auto const& [type_hash, tailed4CycleCount] : typedTailedFourCycles) {
        TypeVector t = decodeHash(type_hash);
        ret.update("tailed-4-cycle", t, (Count) tailed4CycleCount);
    }

    // Apply correction for over-counting
    CountMap &Diamonds = fourNodeInfo.diamonds.typedTotals;
    for (auto const& [type_hash, diamond_count] :  Diamonds) {
        TypeVector t = decodeHash(type_hash);
        TypeVector T4C_types_1 = {t[0], t[1], t[2], t[3], t[0]};
        TypeVector T4C_types_2 = {t[1], t[0], t[2], t[3], t[1]};
        ret.update("tailed-4-cycle", T4C_types_1, -diamond_count);
        ret.update("tailed-4-cycle", T4C_types_2, -diamond_count);
    }

    delete[] outout_count;
    delete[] inout_count;
    return ret;
}
