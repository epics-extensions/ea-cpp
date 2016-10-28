#ifndef EA4_ARCHIVE_SERVER_H
#define EA4_ARCHIVE_SERVER_H

#include "pva/Def.h"
#include "pva/ArchiveRPCService.h"
#include "pva/ArchiveMonitorService.h"

namespace ea4 { namespace pva {

/** Server of the EA4 service */
class ArchiveServer {

  // TODO no thread poll implementation
    
 public:

    POINTER_DEFINITIONS(ArchiveServer);
    
    /** constructor: create context and register ArchiveChannelProviderFactory */
    ArchiveServer();

    /** destroys */
    virtual ~ArchiveServer();
    
    /** m_serverContext->run(seconds) */
    void run(int seconds = 0);
    
    /** m_serverContext->destroy() */
    void destroy();    
    
    /** print info */
    void printInfo();

 private:

    ServerContextImplPtr m_serverContext;

    ChannelProviderFactoryPtr m_providerFactory;
    ChannelProviderPtr m_provider;


};

}}

#endif
