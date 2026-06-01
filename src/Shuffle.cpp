#include <iostream>
#include <fstream>
#include <chrono>
#include "hin/HIN.hpp"
#include "hin/DatasetLoader.hpp"
#include "hin/graphlet/ThreeNode/ThreeNode.hpp"
#include "hin/graphlet/FourNode/FourNode.hpp"
#include "hin/graphlet/FiveNode/FiveNode.hpp"
#include <random>
#include <algorithm>


using namespace std;
using namespace std::chrono;
using namespace CaTSCAN;
namespace fs = std::filesystem;


/// Main function to load a Heterogeneous Information Network (HIN) dataset,
/// shuffle the node types, and count graphlets of size 3, 4, and 5.
/// The results are written to files in the specified output directory.
int main(int argc, char** argv) {

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <dataset_dir> <output_dir>" << endl;
        return 1;
    }

    string dataset_dir = argv[1];
    string output_path = argv[2];

    auto start = high_resolution_clock::now();
    HIN G{};
    LoadDataset(dataset_dir, G);

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(G.types, G.types + G.nNodes, g);

    CHIN CG = CaTSCAN::makeCSR(G);
    delHIN(G);
    CHIN CG_relabel = CG.renameByDegreeOrder();
    delCHIN(CG);
    DCHIN CDAG = degreeOrdered(&CG_relabel);

    cout << "Finished loading and preprocessing graph. Starting graphlet counting..." << endl;

    ThreeNodeInfo three_node_info = threeNodeEnumerator(&CG_relabel, &CDAG.out_list);
    FourNodeInfo four_node_info = fourNodeCalculator(&CG_relabel, &CDAG.out_list, &CDAG.in_list, three_node_info);
    FiveNodeInfo five_node_info = fiveNodeCalculator(&CG_relabel, &CDAG.out_list, &CDAG.in_list, three_node_info, four_node_info);

    cout << "Finished counting graphlets. Writing results to files..." << endl;

    three_node_info.toFiles(output_path);
    four_node_info.toFiles(output_path);
    five_node_info.toFiles(output_path);

    cout << "Finished writing results." << endl;

    delCHIN(CG_relabel);
    delCHIN(CDAG.in_list);
    delCHIN(CDAG.out_list);
    return 0;
}
