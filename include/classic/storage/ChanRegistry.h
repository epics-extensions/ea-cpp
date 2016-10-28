#ifndef EA4_CHAN_REGISTRY_H
#define EA4_CHAN_REGISTRY_H

#include <map>

#include "CtrlInfo.h"

namespace ea4 {

/** \ingroup Storage
Container with the meta-information, including type  
*/

class ChanInfo {

 public:

  /** Sets the data type of this channel */
  void setType(unsigned short dbr_type, unsigned short dbr_count);
 
public:

  /** DBR type (see db_access.h) */
  unsigned short dbr_type;

  /** DBR count (1 - scalar, >1 - waveform) */
  unsigned short dbr_count;

  /** Estimated periodicity of the channel */
  double period;

  /** Max number of samples in the chunk */
  int nSamples;

  /** Max number of chunks in the buffer */
  int nChunks;

};

/** Memory-resident registry of the channel infos */

class ChanRegistry {

 public:

  /** Returns a singleton */
  static ChanRegistry* getInstance();

 public:

  /** Sets the control info for the specified channel */
  void setCtrlInfo(const char* chName, const CtrlInfo& info);

  /** Checks if the specified channel is registered */
  bool containsCtrlInfo(const char* chName) const;

  /** Returns the control info for the specified channel */
  const CtrlInfo& getCtrlInfo(const char* chName) const;

 public:

  /** Sets the channel info for the specified channel */
  void setChanInfo(const char* chName, const ChanInfo& info);

  /** Checks if the specified channel is registered */
  bool containsChanInfo(const char* chName) const;

  /** Returns the channel info for the specified channel */
  const ChanInfo& getChanInfo(const char* chName) const;

 private:

  ChanRegistry();

 private:

  static ChanRegistry* theInstance;

  CtrlInfo emptyCtrlInfo;
  std::map<std::string, CtrlInfo> ctrlInfos;

  ChanInfo emptyChanInfo;
  std::map<std::string, ChanInfo> chanInfos;

};

}

#endif
