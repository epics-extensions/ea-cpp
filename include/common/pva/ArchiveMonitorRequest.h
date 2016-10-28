#ifndef EA4_ARCHIVE_MONITOR_REQUEST_H
#define EA4_ARCHIVE_MONITOR_REQUEST_H

#include <memory>
#include <list>

#include "pva/Def.h"
#include "pva/ArchiveMonitorService.h"

namespace ea4 { namespace pva {

class ArchiveMonitorCommand;

PVACCESS_REFCOUNT_MONITOR_DEFINE(archiveMonitor);

/** Monitor of the EA4 service */

class ArchiveMonitorRequest : public epics::pvData::Monitor, 
  public std::tr1::enable_shared_from_this<ArchiveMonitorRequest> {

 public:

    /** gets a command name from a request, creates a command, sets a request 
	and creates a result type, creates a monitor thread with
        a created command */
    ArchiveMonitorRequest(std::string const & channelName, 
		MonitorRequesterPtr const & requester,
		PVStructurePtr const & request);

    /** destructor */
    virtual ~ArchiveMonitorRequest();

 public:

    /** returns the result type structure (created in constructor according 
	to request)*/
    inline epics::pvData::StructureConstPtr getResultType() const;

    /** adds a monitor element with result */
    virtual void addResult(PVStructurePtr& result);

 public:

    // Monitor API

    /**  m_monThread->start() */
    virtual epics::pvData::Status start();

    /** m_monThread->exitWait() */
    virtual epics::pvData::Status stop();

    /**
     * If monitor has occurred return data.
     * @return monitorElement for modified data.
     * Must call get to determine if data is available.
     */
    virtual MonitorElementPtr poll();

    /**
     * Release a MonitorElement that was returned by poll.
     * @param monitorElement
     */
    virtual void release(MonitorElementPtr const & );

    /** stop */
    virtual void destroy();

 public:
 
    /** empty */
    virtual void lock();

    /** empty */
    virtual void unlock();

protected:

    std::string m_channelName;
    MonitorRequesterPtr m_monitorRequester;

    epics::pvData::StructureConstPtr m_resultType;
    // PVStructurePtr m_pvStructure;

    // BitSetPtr m_changedBitSet;
    // BitSetPtr m_overrunBitSet;

    bool m_first;
    epics::pvData::Mutex m_lock;
    int m_count;
    epics::pvAccess::AtomicBoolean m_active;

    std::list<MonitorElementPtr> m_monElements;
    // MonitorElementPtr m_thisPtr;
    MonitorElementPtr m_nullElement;

    std::tr1::shared_ptr<ArchiveMonitorCommand> m_monCommand;

    std::auto_ptr<epicsThread> m_monThread;

};

inline epics::pvData::StructureConstPtr 
  ArchiveMonitorRequest::getResultType() const {
  return m_resultType;
}

typedef std::tr1::shared_ptr<ArchiveMonitorRequest> ArchiveMonitorRequestPtr;

  }}

#endif
