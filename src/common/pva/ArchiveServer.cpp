#include "pva/ArchiveServer.h"

#include "pva/ArchiveChannelProviderFactory.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvAccess;
using namespace epics::pvData;

namespace ea4 { namespace pva {

ArchiveServer::ArchiveServer() {
    
  // TODO factory is never deregistered, 
  // multiple RPCServer instances create multiple factories, etc.

  m_providerFactory.reset(new ArchiveChannelProviderFactory());
  registerChannelProviderFactory(m_providerFactory);

  m_provider = m_providerFactory->sharedInstance();

  m_serverContext = ServerContextImpl::create();
  m_serverContext->setChannelProviderName(m_provider->getProviderName());
  m_serverContext->initialize(getChannelProviderRegistry());
}

ArchiveServer::~ArchiveServer(){
    // multiple destroy call is OK
    destroy();
}

void ArchiveServer::printInfo()
{
  cout << m_serverContext->getVersion().getVersionString() << endl;
  m_serverContext->printInfo();
}

void ArchiveServer::run(int seconds){
    m_serverContext->run(seconds);
}

void ArchiveServer::destroy(){
  m_serverContext->destroy();
}

  }}
