#include "pva/Archives.h"

#include <stdio.h>
#include <iostream>

using namespace std;

namespace ea4 { namespace pva {

Archives* Archives::theInstance = 0;

Archives* Archives::getInstance(){
  if(theInstance == 0) {
    theInstance = new Archives();
  }
  return theInstance;
}

void Archives::dump() {

  map<int, ArchivesEntry>::iterator it;

  for ( it = archives.begin(); it != archives.end(); ++it){
    ArchivesEntry& entry = it->second;
    printf("key %d: '%s' in '%s'\n",
	   entry.key, entry.name.c_str(), entry.path.c_str());
  }
}

bool Archives::findPath(int key, string& path) {

  map<int, ArchivesEntry>::iterator it;

  it = archives.find(key);
  if(it == archives.end() ) return false;

  path = it->second.path;
  return true;

}

}}
