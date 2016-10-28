#ifndef EA4_PVA_COLLECION_H
#define EA4_PVA_COLLECION_H

#include <string>

#include "pva/Def.h"
#include "pva/Action.h"
#include "pva/Session.h"

namespace ea4 { namespace pva {

/** Proxy collection of the ION data set */
class Collection : public std::tr1::enable_shared_from_this<Collection> {

friend class Session;

 public:

  CollectionPtr createProxy(epics::pvData::PVStructurePtr query);

 public:

  inline const std::string& getName() const;

 public:

  // Transformations

  // CollectionPtr aggregate(epics::pvData::PVStructurePtr query);

 CollectionPtr filter(epics::pvData::PVStructurePtr query);

 CollectionPtr read(epics::pvData::PVStructurePtr query);

  // Actions

  int cache(double timeout);

  PVStructurePtr collect(double timeout);

 private:

  /** constructor of a db-based proxy collection*/
  Collection(Session* session, const std::string& name); 

  Collection(Collection* parent, epics::pvData::PVStructurePtr query);  

 private:

  Session* session; 

  ea4::pva::Query::Request request;

};

inline const std::string& Collection::getName() const {
  return request.src.id;
}

}}

#endif
