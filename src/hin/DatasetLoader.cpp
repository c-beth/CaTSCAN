#include "DatasetLoader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


using namespace std;
using namespace CaTSCAN;


void LoadDataset(const string& path, CaTSCAN::HIN& G) {

    string line;    // line buffer for reading files
    string metaFilePath = path + "/meta.csv";
    ifstream MetaFile(metaFilePath);
    if (!MetaFile.is_open()) throw runtime_error("Could not open meta file: " + metaFilePath);
    // First line contains number of nodes
    getline(MetaFile, line);
    G.nNodes = stoi(line);
    // Second line contains number of edges
    getline(MetaFile, line);
    G.types = new CaTSCAN::TypeIdx [G.nNodes];
    G.nEdges = 2 * stoi(line);
    G.srcs = new NodeIdx[G.nEdges];
    G.dsts = new NodeIdx[G.nEdges];
    // Third line contains number of types
    getline(MetaFile, line);
    G.nTypes = stoi(line);
    MetaFile.close();

    // parsing nodes
    string nodePath = path + "/nodes.csv";
    ifstream NodeFile(nodePath);
    if (!NodeFile.is_open()) throw runtime_error("Could not open node file: " + nodePath);
    int i = 0;
    while (getline(NodeFile, line)) {
        G.types[i] = stoi(line);
        i++;
    }
    NodeFile.close();

    // parsing edges
    string edgePath = path + "/edges.csv";
    ifstream EdgeFile(edgePath);
    if (!EdgeFile.is_open()) throw runtime_error("Could not open edge file " + edgePath);
    EdgeIdx e_i = 0;
    int src, dst;
    string chunk;
    vector<int> chunks;
    while (getline(EdgeFile, line)) {
        chunk = "";
        chunks.clear();
        // cumbersome way of splitting the string by ',' ...
        stringstream line_stream(line);
        while (getline(line_stream, chunk, ',')) {
            chunks.push_back(stoi(chunk));
        }
        src = chunks[0];
        dst = chunks[2];
        G.srcs[e_i] = src;
        G.dsts[e_i] = dst;
        e_i++;
        G.srcs[e_i] = dst;
        G.dsts[e_i] = src;
        e_i++;
    }
    EdgeFile.close();
}
