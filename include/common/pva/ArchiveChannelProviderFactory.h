#ifndef EA4_ARCHIVE_CHANNEL_PROVIDER_FACTORY_H
#define EA4_ARCHIVE_CHANNEL_PROVIDER_FACTORY_H

#include "pva/Def.h"
#include "pva/ArchiveChannelProvider.h"

namespace ea4 { namespace pva {

 /** Channel provider factory of the EA4 service */
 class ArchiveChannelProviderFactory : 
  public epics::pvAccess::ChannelProviderFactory {

  public:

    POINTER_DEFINITIONS(ArchiveChannelProviderFactory);

    /** constructor */
    ArchiveChannelProviderFactory();

  public:

    /** returns ea4.pva.ChannelProvider */
    virtual std::string getFactoryName();

    /** returns a channel provider owned by factory */
    virtual ChannelProviderPtr sharedInstance();

    /** creates a new instance of a channel provider */
    virtual ChannelProviderPtr newInstance();

private:

    ArchiveChannelProvider::shared_pointer m_provider;

};

  }}

#endif
