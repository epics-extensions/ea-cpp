#include "pva/ArchiveService.h"
#include "pva/ArchiveRequestException.h"

using namespace std;
using namespace epics::pvData;

namespace ea4 { namespace pva {

string
ArchiveService::getCommandName(PVStructurePtr const & request ) const {

    // Extract the arguments. Just one in this case.
    // Report an error by throwing a RPCRequestException 
    PVStringPtr commandField = request->getStringField("command");

    if (commandField == NULL) {
        throw ArchiveRequestException(Status::STATUSTYPE_ERROR,
	    "PVString field with name 'command' expected.");
    }

    return commandField->get();
}

}}
