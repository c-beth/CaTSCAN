#include <iostream>
#include <fstream>
#include <chrono>
#include "hin/HIN.hpp"
#include "hin/DatasetLoader.hpp"
#include "hin/graphlet/ThreeNode/ThreeNode.hpp"
#include "hin/graphlet/FourNode/FourNode.hpp"
#include "hin/graphlet/FiveNode/FiveNode.hpp"
#include <filesystem>


using namespace std;
using namespace std::chrono;
using namespace CaTSCAN;
namespace fs = std::filesystem;


/// Main function to load a Heterogeneous Information Network (HIN) dataset,
/// count graphlets of size 3, 4, and 5 based on the specified graphlet size,
/// and write the results to files in the specified output directory.
int main(int argc, char** argv) {

    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <dataset_dir> <output_dir> <size of graphlets>" << endl;
        return 1;
    }
    int graphlet_size = stoi(argv[3]);
    if (graphlet_size < 3 or graphlet_size > 5) {
        cerr << "Graphlet size must be 3, 4, or 5." << endl;
        return 1;
    }

    string dataset_dir = argv[1];
    string output_path = argv[2];

    auto start = high_resolution_clock::now();
    HIN G{};
    LoadDataset(dataset_dir, G);
    CHIN CG = CaTSCAN::makeCSR(G);
    delHIN(G);
    CHIN CG_relabel = CG.renameByDegreeOrder();
    delCHIN(CG);
    DCHIN CDAG = degreeOrdered(&CG_relabel);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    string results = to_string(duration.count()) + "\t";

    if (graphlet_size == 3) {
        ThreeNodeInfo three_node_info = threeNodeEnumerator(&CG_relabel, &CDAG.out_list);
        stop = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(stop - start);
        results += to_string(duration.count());
        three_node_info.toFiles(output_path);

    } else if (graphlet_size == 4) {
        ThreeNodeInfo three_node_info = threeNodeEnumerator(&CG_relabel, &CDAG.out_list);
        FourNodeInfo four_node_info = fourNodeCalculator(&CG_relabel, &CDAG.out_list, &CDAG.in_list, three_node_info);
        stop = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(stop - start);
        results += to_string(duration.count());
        three_node_info.toFiles(output_path);
        four_node_info.toFiles(output_path);

    } else {
        ThreeNodeInfo three_node_info = threeNodeEnumerator(&CG_relabel, &CDAG.out_list);
        FourNodeInfo four_node_info = fourNodeCalculator(&CG_relabel, &CDAG.out_list, &CDAG.in_list, three_node_info);
        FiveNodeInfo five_node_info = fiveNodeCalculator(&CG_relabel, &CDAG.out_list, &CDAG.in_list, three_node_info, four_node_info);
        stop = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(stop - start);
        results += to_string(duration.count());
        three_node_info.toFiles(output_path);
        four_node_info.toFiles(output_path);
        five_node_info.toFiles(output_path);

    }

    delCHIN(CG_relabel);
    delCHIN(CDAG.in_list);
    delCHIN(CDAG.out_list);

    if (!fs::is_directory(output_path) or !fs::exists(output_path))
        fs::create_directories(output_path);
    ofstream out(output_path +  + "/CaTSCAN_times_" + argv[3] + ".tsv", std::ios_base::app);
    out << results << endl << flush;
    out.close();
    return 0;
}
