// -*- c++ -*-
#if !defined(_HASHTABLE_H_)
#define _HASHTABLE_H_

/// \ingroup Storage
/// Hash table 
class HashTable   {

public:

  enum {
     HashTableSize = 256
  };

  /** data type of a hash value */
  typedef unsigned short HashValue;

  /** Returns a hash value for the specified string */
  static HashValue Hash (const char *string);

};

#endif
