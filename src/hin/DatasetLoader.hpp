#ifndef CATSCAN_DATASETLOADER_HPP
#define CATSCAN_DATASETLOADER_HPP


#include <string>
#include "HIN.hpp"


// Load a dataset from a specify directory.
// The directory must contain a schema file named "meta.csv",
// and a set of files named "edges.csv", "nodes.csv", and "node_types.csv".
void LoadDataset(const std::string& path, CaTSCAN::HIN& G);



#endif //CATSCAN_DATASETLOADER_HPP
