#include <string>
#include <iostream>

// base
#include "epicsVersion.h"

// tools
#include "tools/AutoPtr.h"
#include "tools/BenchTimer.h"
#include "tools/Filename.h"
#include "tools/ArgParser.h"
#include "tools/IndexConfig.h"
#include "tools/Lockfile.h"

// storage
#include "storage/IndexFile.h"

using namespace std;

int main(int argc, const char *argv[]) {


        
  string index_name = "/DATA/RadMon/2015/year_index_ssd";
  string pv_name = "BR-AM{RadMon:01}Itvl:Data-I";
  string dot_name = pv_name + ".dot";

  try {
     
    Index* index = new IndexFile();

    index->open(index_name);

    Index::Result* result = index->findChannel(pv_name);

    RTree* rtree = result->getRTree();

    rtree->makeDot(dot_name.c_str());

  } catch (GenericException &e) {
    std::cout << e.what() << std::endl;
  }

}
