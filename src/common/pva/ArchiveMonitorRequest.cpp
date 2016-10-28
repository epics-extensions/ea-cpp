#include "pva/ArchiveMonitorCommand.h"
#include "pva/ArchiveMonitorRequest.h"
#include "pva/ArchiveMonitorServiceRegistry.h"
#include "pva/ArchiveMonitorCommand.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

ArchiveMonitorRequest::ArchiveMonitorRequest(string const & name, 
	    MonitorRequesterPtr const & requester,
	    PVStructurePtr const & request) :
        m_channelName(name),
        m_monitorRequester(requester),  
        m_first(true),
        m_lock(),
        m_count(0) {

  PVACCESS_REFCOUNT_MONITOR_CONSTRUCT(archiveMonitor);

  // command/process

  ArchiveMonitorServiceRegistry* monRegistry = 
    ArchiveMonitorServiceRegistry::getInstance();
  ArchiveMonitorServicePtr monService = monRegistry->getMonitorService();

  string commandName =  monService->getCommandName(request);
  ArchiveCommand* command = monService->getMonitorCommand(commandName);
  m_monCommand = dynamic_pointer_cast<ArchiveMonitorCommand>(command->createCommand());

  // result type and structure

  m_monCommand->setRequest(request);
  m_resultType = m_monCommand->createResultType();

  //  string str;
  //  m_resultType->toString(&str);
  //  cout << str << endl;

  // PVDataCreatePtr dataFactory = getPVDataCreate();
  // m_pvStructure =  dataFactory->createPVStructure(m_resultType);

  // m_changedBitSet.reset(new BitSet(m_pvStructure->getNumberFields()));
  // m_overrunBitSet.reset(new BitSet(m_pvStructure->getNumberFields()));

  // m_changedBitSet->set(0);

  // monitor element

  // m_thisPtr->pvStructurePtr = m_pvStructure;
  // m_thisPtr->changedBitSet  = m_changedBitSet;
  // m_thisPtr->overrunBitSet  = m_overrunBitSet;

  m_monThread.reset(new epicsThread(*m_monCommand, 
				    "ArchiveMonitor", 
  				    epicsThreadGetStackSize(epicsThreadStackSmall), 
				    highPriority));
}

ArchiveMonitorRequest::~ArchiveMonitorRequest(){
  PVACCESS_REFCOUNT_MONITOR_DESTRUCT(archiveMonitor);
}

Status ArchiveMonitorRequest::start() {

  std::cout << "start" << std::endl;
        
  m_active.set();

  ArchiveMonitorRequestPtr thisPtr = shared_from_this();
  m_monCommand->setMonitorRequest(thisPtr);
     
  // first monitor
  // Monitor::shared_pointer thisPtr = shared_from_this();
  // m_monitorRequester->monitorEvent(thisPtr);

  m_monThread->start();

  return Status::Ok;
}

Status ArchiveMonitorRequest::stop(){
  std::cout << "stop" << std::endl;
  m_active.clear();
  m_monThread->exitWait();
  return Status::Ok;
}

MonitorElement::shared_pointer ArchiveMonitorRequest::poll() {   

  std::cout << "poll, elements: " << m_monElements.size() << std::endl;    

  Lock xx(m_lock);
        
  if (m_monElements.size() == 0) {
    return m_nullElement;
  } else {
    MonitorElementPtr monElement = m_monElements.front();
    m_monElements.pop_front();
    return monElement;
  }  
}

void ArchiveMonitorRequest::release(MonitorElement::shared_pointer const & monElement) {

  if(monElement.get() == 0) return;

  std::cout << "release, ref counter: " << monElement.use_count() << std::endl;    

  Lock xx(m_lock);
        
  monElement->pvStructurePtr.reset();
  monElement->changedBitSet.reset();
  monElement->overrunBitSet.reset();

}

void ArchiveMonitorRequest::destroy() {   
  std::cout << "destroy" << std::endl;    
  stop();
}

void ArchiveMonitorRequest::lock() {
  // structureStoreMutex.lock();
}

void ArchiveMonitorRequest::unlock() {
  // structureStoreMutex.unlock();
}

void ArchiveMonitorRequest::addResult(PVStructurePtr& result){

  MonitorElementPtr monElement(new MonitorElement());

  // monitor element

  monElement->pvStructurePtr = result;
  monElement->changedBitSet.reset(new BitSet(result->getNumberFields()));
  monElement->overrunBitSet.reset(new BitSet(result->getNumberFields()));

  monElement->changedBitSet->set(0);

  {
    Lock xx(m_lock);
    m_monElements.push_back(monElement);
  }

  ArchiveMonitorRequestPtr thisPtr = shared_from_this();
  m_monitorRequester->monitorEvent(thisPtr);
}

  }}
