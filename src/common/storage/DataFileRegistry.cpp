#include "storage/DataFileRegistry.h"

using namespace std;

namespace ea4 { namespace storage {

DataFileRegistry* DataFileRegistry::theInstance = 0;

DataFileRegistry* 
DataFileRegistry::getInstance(){

  if(theInstance == 0){
    theInstance = new DataFileRegistry();
  }
  return theInstance;
}

DataFileRegistry::DataFileRegistry() : manager(0) {
}

DataFileRegistry::~DataFileRegistry(){
  if(manager) delete manager;
}

void DataFileRegistry::setManager(DataFileManager* m){
  if(manager) return;
  manager = m;
}

DataFileManager* DataFileRegistry::getManager(){
  return manager;
}


}}
