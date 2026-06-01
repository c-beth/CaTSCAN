#include <iostream>
#include <fstream>
#include <chrono>
#include "hin/HIN.hpp"
#include "hin/DatasetLoader.hpp"
#include "hin/graphlet/Rossi/ThreeNode.hpp"
#include "hin/graphlet/Rossi/FourNode.hpp"
#include <filesystem>


using namespace std;
using namespace std::chrono;
using namespace CaTSCAN;
namespace fs = std::filesystem;


/// Main function to load a Heterogeneous Information Network (HIN) dataset
/// using our implementation of the Rossi algorithm.
/// Count graphlets of size 3 and 4 based on the specified graphlet size,
/// and write the results to files in the specified output directory.
int main(int argc, char** argv) {

    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <dataset_dir> <output_dir> <size of graphlets>" << endl;
        return 1;
    }
    int graphlet_size = stoi(argv[3]);
    if (graphlet_size < 3 or graphlet_size > 4) {
        cerr << "Graphlet size must be 3 or 4." << endl;
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
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds >(stop - start);
    string results = to_string(duration.count()) + "\t";

    if (graphlet_size == 3) {
        ThreeNodeInfo three_node_info = Rossi::count3Node(&CG_relabel);
        stop = high_resolution_clock::now();
        duration = duration_cast<milliseconds >(stop - start);
        results += to_string(duration.count());
    } else {
        CountInfo counts = Rossi::count3And4Node(&CG_relabel);
        stop = high_resolution_clock::now();
        duration = duration_cast<milliseconds >(stop - start);
        results += to_string(duration.count());
    }

    delCHIN(CG_relabel);

    if (!fs::is_directory(output_path) or !fs::exists(output_path))
        fs::create_directories(output_path);
    ofstream out(output_path +  + "/Rossi_times_" + argv[3] + ".tsv", std::ios_base::app);
    out << results << endl << flush;
    out.close();
    return 0;
}
