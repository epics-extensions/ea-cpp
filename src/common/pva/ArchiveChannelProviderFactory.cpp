#include "pva/ArchiveChannelProviderFactory.h"
#include "pva/ArchiveChannelProvider.h"

using namespace std;
using namespace epics::pvAccess;
using namespace epics::pvData;

namespace ea4 { namespace pva {

ArchiveChannelProviderFactory::ArchiveChannelProviderFactory() :
  m_provider(new ArchiveChannelProvider()){
}

string ArchiveChannelProviderFactory::getFactoryName() {
  return m_provider->getProviderName();
}

ChannelProvider::shared_pointer 
ArchiveChannelProviderFactory::sharedInstance() {
  return m_provider;
}

ChannelProvider::shared_pointer 
ArchiveChannelProviderFactory::newInstance() {
  return ChannelProvider::shared_pointer(new ArchiveChannelProvider());
}


  }}
