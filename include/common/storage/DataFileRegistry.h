#ifndef __EA4_STORAGE_DATA_FILE_REGISTRY_H
#define __EA4_STORAGE_DATA_FILE_REGISTRY_H


namespace ea4 { namespace storage {

/**  Basic class of the format-specific managers of data files */
class DataFileManager {

 public:

  /** Closes all data files that are fully released.
   *
   * @return Returns the number of files which are
   * left open because there is still
   * a reference to them.
   */
   virtual unsigned int clear_cache() = 0;

};

/** Registry the format-specific managers */
class DataFileRegistry {

 public:

  /** Returns singleteton */
  static DataFileRegistry* getInstance();

  /** Registers a manager */
  void setManager(DataFileManager* manager);

  /** Returns a factory for the specified file */
  DataFileManager* getManager();

 private:

  DataFileRegistry();
 ~DataFileRegistry();

 private:

  static DataFileRegistry* theInstance;

  DataFileManager* manager;

};

}}

#endif
