#include "FiveNode.hpp"


// Pattern 9: Hourglass
//
//      1-----2
//       \   /
//        \ /
//         0
//        / \
//       /   \
//      3-----4
//
// Method:
//   Loop over all nodes of the graph as node 0.
//   Choose two distinct triangles that node 0 participates in.
//   In the hourglass case the 2 triangles do not share an edge:
//   Subtract the cases that share an edge (diamonds).
//
// Arguments:
//   g: CSR of the graph
//   fourNodeInfo: Counts + Info of 4-node graphlets
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 6 in the graph

GraphletInfo CaTSCAN::countHourglasses(CHIN *g,
                                       FourNodeInfo& fourNodeInfo,
                                       ThreeNodeInfo& threeNodeInfo ) {

    GraphletInfo ret;
    Count count;
    TypeVector types;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        CountMap& tris_i = threeNodeInfo.trianglesPerNode[v_i];
        TypeIdx t_i = g->nodeTypes[v_i];

        for (auto tri1 = tris_i.begin(); tri1 != tris_i.end(); tri1++) {
            Count tri1_count = tri1->second;
            if (tri1_count == 0)
                continue;
            TypeVector tri1_types = decodeNodeTriangle(tri1->first);
            count = tri1_count * (tri1_count - 1) / 2;    // n choose 2
            types = {t_i, tri1_types[0], tri1_types[1], tri1_types[0], tri1_types[1]};
            ret.update("hourglasses", types, count);

            for (auto tri2 = next(tri1); tri2 != tris_i.end(); tri2++) {
                Count tri2_count = tri2->second;
                if (tri2_count == 0)
                    continue;
                TypeVector tri2_types = decodeNodeTriangle(tri2->first);
                count = tri1_count * tri2_count;
                types = {t_i, tri1_types[0], tri1_types[1], tri2_types[0], tri2_types[1]};
                ret.update("hourglasses", types, count);
            }
        }
    }

    // Apply correction for over-counting
    CountMap &Diamonds = fourNodeInfo.diamonds.typedTotals;
    for (auto const& [type_hash, diamond_count] : Diamonds) {
        TypeVector t = decodeHash(type_hash);
        TypeVector HG_types_1 = {t[0], t[1], t[2], t[1], t[3]};
        TypeVector HG_types_2 = {t[1], t[0], t[2], t[0], t[3]};
        ret.update("hourglasses", HG_types_1, -diamond_count);
        ret.update("hourglasses", HG_types_2, -diamond_count);
    }
    return ret;
}
