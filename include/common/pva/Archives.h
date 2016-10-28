#ifndef EA4_ARCHIVES_H
#define EA4_ARCHIVES_H

#include <tr1/memory>
#include <string>
#include <map>

namespace ea4 { namespace pva {

/** Collection of archives of the EA4 service */
class Archives {

 public:

  /** returns a singleton */
  static Archives* getInstance();

 public:

  /** archive's info */
  struct ArchivesEntry {

     inline void clear();

     int key;          ///< archive key
     std::string name; ///< archive name
     std::string path; ///< path to the Index file
     std::string type; ///< ea3, mongo
  };

  /** archives */
  std::map<int, ArchivesEntry> archives;

  /** archive keys */
  std::map< std::string, int> keys;

 public:

  /** find a path for a slected key */
  bool findPath(int key, std::string& path);

  /** print info */
  void dump();

 private:

  static Archives* theInstance;

};

}}


#endif
