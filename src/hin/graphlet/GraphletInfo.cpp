#include "GraphletInfo.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


using namespace std;


void GraphletInfo::update(const string& graphlet, const TypeVector& types, Count count) {
    if (count == 0)
        return;
    string typeHash = iso(types, graphlet);
    total += count;
    typedTotals[typeHash] += count;

    if (count < 0 ) {
        totalCorrections += count;
        typedCorrections[typeHash] += count;
    }
    if (typedTotals[typeHash] == 0)
        typedTotals.erase(typeHash);
}


void GraphletInfo::toFile(const string& path) {
    ofstream out(path);
    for (const auto& [typeHash, count] : typedTotals) {
        out << typeHash << "\t" << count << endl;
    }
    out.close();
}


ThreeNodeInfo::ThreeNodeInfo(Count nodes, Count edges) {
    trianglesPerNode = new CountMap[nodes];
    trianglesPerEdge = new CountMap[edges];
}


void ThreeNodeInfo::toFiles(const std::string &dir) {
    wedges.toFile(dir + "/wedges.tsv");
    triangles.toFile(dir + "/triangles.tsv");
}


void FourNodeInfo::toFiles(const std::string &dir) {
    threeStars.toFile(dir + "/three_stars.tsv");
    threePaths.toFile(dir + "/three_paths.tsv");
    tailedTriangles.toFile(dir + "/tailed_triangles.tsv");
    fourCycles.toFile(dir + "/four_cycles.tsv");
    diamonds.toFile(dir + "/diamonds.tsv");
    fourCliques.toFile(dir + "/four_cliques.tsv");
}


void FiveNodeInfo::toFiles(const std::string &dir) {
    fourStars.toFile(dir + "/four_stars.tsv");
    prongs.toFile(dir + "/prongs.tsv");
    fourPaths.toFile(dir + "/four_paths.tsv");
    forkedTailedTriangles.toFile(dir + "/forked_tailed_triangles.tsv");
    longTailedTriangles.toFile(dir + "/long_tailed_triangles.tsv");
    doubleTailedTriangles.toFile(dir + "/double_tailed_triangles.tsv");
    tailedFourCycles.toFile(dir + "/tailed_four_cycles.tsv");
    fiveCycles.toFile(dir + "/five_cycles.tsv");
    hourglasses.toFile(dir + "/hourglasses.tsv");
    cobras.toFile(dir + "/cobras.tsv");
    stingrays.toFile(dir + "/stingrays.tsv");
    hattedFourCycles.toFile(dir + "/hatted_four_cycles.tsv");
    threeWedgeCollisions.toFile(dir + "/three_wedge_collisions.tsv");
    stellateTridents.toFile(dir + "/stellate_tridents.tsv");
    tailedFourCliques.toFile(dir + "/tailed_four_cliques.tsv");
    triangleStrips.toFile(dir + "/tri_strips.tsv");
    chordalWedgeCollisions.toFile(dir + "/chordal_wedge_collisions.tsv");
    wheels.toFile(dir + "/wheels.tsv");
    hattedFourCliques.toFile(dir + "/hatted_four_cliques.tsv");
    almostFiveCliques.toFile(dir + "/almost_five_cliques.tsv");
    fiveCliques.toFile(dir + "/five_cliques.tsv");
}


CountInfo::CountInfo(Count nodes, Count edges) {
    threeNode = ThreeNodeInfo(nodes, edges);
}


void CountInfo::toFiles(const std::string &dir) {
    threeNode.toFiles(dir);
    fourNode.toFiles(dir);
    fiveNode.toFiles(dir);
}


string CaTSCAN::iso(const TypeVector& types, const string& graphlet) {
    if (graphlet == "wedge")
        return isoWedge(types);
    else if (graphlet == "triangle" or graphlet == "4-clique" or graphlet == "5-clique")
        return isoClique(types);
    else if (graphlet == "3-star")
        return isoStar(types);
    else if (graphlet == "3-path")
        return isoThreePath(types);
    else if (graphlet == "tailed-triangle")
        return isoTailedTriangle(types);
    else if (graphlet == "4-cycle")
        return isoFourCycle(types);
    else if (graphlet == "diamond")
        return isoDiamond(types);
    else if (graphlet == "4-star")
        return isoStar(types);
    else if (graphlet == "4-path")
        return isoFourPaths(types);
    else if (graphlet == "prong")
        return isoProngs(types);
    else if (graphlet == "forked-tailed-triangle")
        return isoForkedTailedTriangles(types);
    else if (graphlet == "double-tailed-triangle")
        return isoDoubleTailedTriangles(types);
    else if (graphlet == "long-tailed-triangle")
        return isoLongTailedTriangles(types);
    else if (graphlet == "hourglasses")
        return isoHourglass(types);
    else if (graphlet == "stingrays")
        return isoStingray(types);
    else if (graphlet == "stellate-trident")
        return isoStellateTrident(types);
    else if (graphlet == "hatted-4-cycle")
        return isoHattedFourCycle(types);
    else if (graphlet == "tailed-4-cycle")
        return isoTailedFourCycle(types);
    else if (graphlet == "tailed-4-clique")
        return isoTailedFourClique(types);
    else if (graphlet == "hatted-4-clique")
        return isoHattedFourClique(types);
    else if (graphlet == "5-cycle")
        return isoFiveCycle(types);
    else if (graphlet == "3-wedge-collision")
        return isoThreeWedgeCollision(types);
    else if (graphlet == "chordal-wedge-collision")
        return isoChordalWedgeCollision(types);
    else if (graphlet == "almost-5-clique")
        return isoAlmost5Clique(types);
    else if (graphlet == "cobra")
        return isoCobra(types);
    else if (graphlet == "wheel")
        return isoWheel(types);
    else if (graphlet == "tri-strip")
        return isoTriStrip(types);
    else {
        cerr << "Unknown type isomorphism: " << graphlet << endl;
        exit(1);
    }
}


string CaTSCAN::prettyPrintHash(const TypeVector& types) {
    string ret = "(";
    for (TypeIdx t : types)
        ret += to_string(t) + ", ";
    ret.pop_back();     // remove last comma
    ret.pop_back();     // remove last space
    ret += ")";
    return ret;
}


TypeVector CaTSCAN::decodeHash(const string& hash){
    TypeVector types;
    string type_list = hash.substr(1,hash.size()-2);
    int start, end = -1;
    do {
        start = end + 1;
        end = (int) type_list.find(',', start);
        TypeIdx type = stoi(type_list.substr(start, end - start));
        types.push_back(type);
    } while (end != -1);
    return types;
}


TypeVector CaTSCAN::decodeNodeTriangle(const string &hash) {
    string chunk;
    TypeVector chunks;
    // cumbersome way of splitting the string by '-' ...
    stringstream line_stream(hash);
    while (getline(line_stream, chunk, '-'))
        chunks.push_back(stoi(chunk));
    return chunks;
}



// -------------------------------------------------------------------------------------------
// -------------------- Detailed Iso Functions -----------------------------------------------
// -------------------------------------------------------------------------------------------



// Overloaded to use for cliques of all sizes
string CaTSCAN::isoClique(const TypeVector &types) {
    TypeVector t = types;
    sort(t.begin(), t.end());
    return prettyPrintHash(t);
}

// Overloaded to use for stars of all sizes
string CaTSCAN::isoStar(const CaTSCAN::TypeVector &types) {
    TypeVector t = types;
    sort(t.begin() + 1, t.end());
    return prettyPrintHash(t);
}



// -------------------- Three node graphlets -------------------------------------------------


string CaTSCAN::isoWedge(const TypeVector& types) {
    TypeVector t{types[0],
                 min(types[1], types[2]),
                 max(types[1], types[2])};
    return prettyPrintHash(t);
}



// -------------------- Four node graphlets --------------------------------------------------


string CaTSCAN::isoThreePath(const CaTSCAN::TypeVector &types) {
    TypeVector t_0{types[0], types[1], types[2], types[3]};
    TypeVector t_1{types[3], types[2], types[1], types[0]};
    return prettyPrintHash(min(t_0, t_1));
}


string CaTSCAN::isoTailedTriangle(const CaTSCAN::TypeVector &types) {
    // order: center, tail, triangle node 1, triangle node 2
    TypeVector t{types[0],
                 types[1],
                 min(types[2], types[3]),
                 max(types[2], types[3])};
    return prettyPrintHash(t);
}


string CaTSCAN::isoFourCycle(const CaTSCAN::TypeVector &types) {
    TypeVector t_0 = {types[0], types[1], types[2], types[3]};
    TypeVector t_1 = {types[1], types[2], types[3], types[0]};
    TypeVector t_2 = {types[2], types[3], types[0], types[1]};
    TypeVector t_3 = {types[3], types[0], types[1], types[2]};

    TypeVector t_4 = {types[3], types[2], types[1], types[0]};
    TypeVector t_5 = {types[2], types[1], types[0], types[3]};
    TypeVector t_6 = {types[1], types[0], types[3], types[2]};
    TypeVector t_7 = {types[0], types[3], types[2], types[1]};

    return prettyPrintHash(min({t_0, t_1, t_2, t_3, t_4, t_5, t_6, t_7}));
}


string CaTSCAN::isoDiamond(const CaTSCAN::TypeVector &types) {
    TypeVector t{min(types[0], types[1]),
                 max(types[0], types[1]),
                 min(types[2], types[3]),
                 max(types[2], types[3])};
    return prettyPrintHash(t);
}



// -------------------- Five node graphlets --------------------------------------------------

    
string CaTSCAN::isoFourPaths(const TypeVector& types){
    TypeVector t_0{types[0], types[1], types[2], types[3], types[4]};
    TypeVector t_1{types[4], types[3], types[2], types[1], types[0]};
    return prettyPrintHash(min(t_0, t_1));
}


string CaTSCAN::isoProngs(const TypeVector& types){
    TypeVector t{types[0],
                 types[1],
                 types[2],
                 min(types[3], types[4]),
                 max(types[3], types[4])};
    return prettyPrintHash(t);
}


string CaTSCAN::isoForkedTailedTriangles(const TypeVector& types){
    TypeVector t{types[0],
                 min(types[1], types[2]),
                 max(types[1], types[2]),
                 min(types[3], types[4]),
                 max(types[3], types[4])};
    return prettyPrintHash(t);
}


string CaTSCAN::isoDoubleTailedTriangles(const TypeVector& types){
    TypeVector t_0{types[0], types[1], types[2], types[3], types[4]};
    TypeVector t_1{types[1], types[0], types[2], types[4], types[3]};
    return prettyPrintHash(min(t_0, t_1));
}


string CaTSCAN::isoLongTailedTriangles(const TypeVector& types){
    TypeVector t{types[0],
                 types[1],
                 types[2],
                 min(types[3], types[4]),
                 max(types[3], types[4])};
    return prettyPrintHash(t);
}


string CaTSCAN::isoHourglass(const TypeVector& types){
    TypeVector t_0{types[0],
                   min(types[1], types[2]),
                   max(types[1], types[2]),
                   min(types[3], types[4]),
                   max(types[3], types[4])};

    TypeVector t_1{types[0],
                   min(types[3], types[4]),
                   max(types[3], types[4]),
                   min(types[1], types[2]),
                   max(types[1], types[2])};
    return prettyPrintHash(min(t_0, t_1));
}


string CaTSCAN::isoStingray(const TypeVector& types){
    TypeVector t{types[0],
                 types[1],
                 types[2],
                 min(types[3], types[4]),
                 max(types[3], types[4])};
    return prettyPrintHash(t);
}


string CaTSCAN::isoStellateTrident(const TypeVector& types){
    TypeVector t = {min(types[0], types[1]),
                    max(types[0], types[1]),
                    types[2],
                    types[3],
                    types[4]};
    sort(t.end() - 3, t.end());
    return prettyPrintHash(t);
}


string CaTSCAN::isoHattedFourCycle(const TypeVector& types){
    TypeVector t_0{types[0], types[1], types[2], types[3], types[4]};
    TypeVector t_1{types[0], types[3], types[4], types[1], types[2]};
    return prettyPrintHash(min(t_0, t_1));
}


string CaTSCAN::isoTailedFourCycle(const TypeVector& types){
    TypeVector t = {types[0],
                    types[1],
                    min(types[2], types[3]),
                    max(types[2], types[3]),
                    types[4]};
    return prettyPrintHash(t);
}


string CaTSCAN::isoTailedFourClique(const TypeVector& types){
    TypeVector t{types[0], types[1], types[2], types[3], types[4]};
    sort(t.end() - 3, t.end());
    return prettyPrintHash(t);
}


string CaTSCAN::isoHattedFourClique(const TypeVector& types){
    TypeVector t{types[0],
                 min(types[1], types[2]),
                 max(types[1], types[2]),
                 min(types[3], types[4]),
                 max(types[3], types[4])};
    return prettyPrintHash(t);
}


string CaTSCAN::isoFiveCycle(const CaTSCAN::TypeVector &types) {
    TypeVector t_0 = {types[0], types[1], types[2], types[3], types[4]};
    TypeVector t_1 = {types[1], types[2], types[3], types[4], types[0]};
    TypeVector t_2 = {types[2], types[3], types[4], types[0], types[1]};
    TypeVector t_3 = {types[3], types[4], types[0], types[1], types[2]};
    TypeVector t_4 = {types[4], types[0], types[1], types[2], types[3]};

    TypeVector t_5 = {types[4], types[3], types[2], types[1], types[0]};
    TypeVector t_6 = {types[3], types[2], types[1], types[0], types[4]};
    TypeVector t_7 = {types[2], types[1], types[0], types[4], types[3]};
    TypeVector t_8 = {types[1], types[0], types[4], types[3], types[2]};
    TypeVector t_9 = {types[0], types[4], types[3], types[2], types[1]};

    return prettyPrintHash(min({t_0, t_1, t_2, t_3, t_4, t_5, t_6, t_7, t_8, t_9}));
}


string CaTSCAN::isoThreeWedgeCollision(const CaTSCAN::TypeVector &types) {
    TypeVector t = {min(types[0], types[1]),
                    max(types[0], types[1]),
                    types[2],
                    types[3],
                    types[4]};
    sort(t.end() - 3, t.end());
    return prettyPrintHash(t);
}


string CaTSCAN::isoChordalWedgeCollision(const CaTSCAN::TypeVector &types) {
    TypeVector t = {types[0],
                    min(types[1], types[3]),
                    min(types[2], types[4]),
                    max(types[1], types[3]),
                    max(types[2], types[4])};
    return prettyPrintHash(t);
}


string CaTSCAN::isoWheel(const CaTSCAN::TypeVector &types) {
    TypeVector t_0{types[0], types[1], types[2], types[3], types[4]};
    TypeVector t_1{types[0], types[2], types[3], types[4], types[1]};
    TypeVector t_2{types[0], types[3], types[4], types[1], types[2]};
    TypeVector t_3{types[0], types[4], types[1], types[2], types[3]};

    TypeVector t_4{types[0], types[4], types[3], types[2], types[1]};
    TypeVector t_5{types[0], types[3], types[2], types[1], types[4]};
    TypeVector t_6{types[0], types[2], types[1], types[4], types[3]};
    TypeVector t_7{types[0], types[1], types[4], types[3], types[2]};

    return prettyPrintHash(min({t_0, t_1, t_2, t_3, t_4, t_5, t_6, t_7}));
}


string CaTSCAN::isoTriStrip(const CaTSCAN::TypeVector &types) {
    TypeVector t_0{types[0], types[1], types[2], types[3], types[4]};
    TypeVector t_1{types[0], types[2], types[1], types[4], types[3]};
    return prettyPrintHash(min(t_0, t_1));
}


string CaTSCAN::isoCobra(const CaTSCAN::TypeVector &types) {
    TypeVector t{types[0],
                 types[1],
                 min(types[2], types[3]),
                 max(types[2], types[3]),
                 types[4]};
    return prettyPrintHash(t);
}


string CaTSCAN::isoAlmost5Clique(const CaTSCAN::TypeVector &types) {
    TypeVector t = {min(types[0], types[1]),
                    max(types[0], types[1]),
                    types[2],
                    types[3],
                    types[4]};
    sort(t.end() - 3, t.end());
    return prettyPrintHash(t);
}


TriangleList::TriangleList(CaTSCAN::CHIN *g, CaTSCAN::Count nTris) {
    total = 3 * nTris;
    triangles = new NodeIdx[3 * nTris];
    trioffsets = new EdgeIdx[g->nEdges + 1];
    EdgeIdx e_ij = 0;
    EdgeIdx current = 0;
    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        Count deg_i = g->degree(v_i);
        for (e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            Count deg_j = g->degree(v_j);
            trioffsets[e_ij] = current;
            if (deg_j < deg_i or (deg_j == deg_i and v_j < v_i))
                continue;

            for (EdgeIdx e_ik = g->offsets[v_i]; e_ik < g->offsets[v_i + 1]; e_ik++) {
                NodeIdx v_k = g->nbors[e_ik];
                if (g->getEdge(v_k, v_j) != invalidEdge) {
                    triangles[current] = v_k;
                    current++;
                }
            }
        }
    }
    trioffsets[e_ij] = current; // e_ij is the index after the last edge in g. we set this final offset to the current position
    for (EdgeIdx i = 0; i < g->nEdges; i++)
        std::sort(triangles + trioffsets[i], triangles + trioffsets[i + 1]);
}
