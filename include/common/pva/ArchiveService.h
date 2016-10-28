#ifndef EA4_ARCHIVE_SERVICE_H
#define EA4_ARCHIVE_SERVICE_H

#include <memory>
#include "pva/Def.h"

namespace ea4 { namespace pva {

/** Basis class of the EA4 services */
class ArchiveService {

 public:

   /** Pointer definitions */
   POINTER_DEFINITIONS(ArchiveService);

   /** destructor */
   virtual ~ArchiveService() {}

 public:

   /** returns a command name of the request */
   std::string getCommandName(PVStructurePtr const & request ) const;

 public:

  /** returns a channel name */
  inline const std::string& getChannelName() const;

  /** returns a service type (such as ea3 or mongo) */
  inline const std::string& getServiceType() const;

  /** returns a service name (such as nsls2) */
  inline const std::string& getServiceName() const;

  /** defines a service name */
  virtual void setServiceName(const std::string& name) = 0;

 protected:

  /** channel name */
  std::string channelName;

  /** service type */
  std::string serviceType;

  /** service name */
  std::string serviceName;

};

inline const std::string& 
ArchiveService::getChannelName() const {
  return channelName;
}

inline const std::string& 
ArchiveService::getServiceType() const {
  return serviceType;
}

inline const std::string& 
ArchiveService::getServiceName() const {
  return serviceName;
}

typedef ArchiveService::shared_pointer ArchiveServicePtr;

}}

#endif
