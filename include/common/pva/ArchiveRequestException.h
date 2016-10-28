#ifndef EA4_ARCHIVE_REQUEST_EXCEPTION_H
#define EA4_ARCHIVE_REQUEST_EXCEPTION_H

#include <stdexcept>
#include <pv/sharedPtr.h>
#include <pv/pvAccess.h>
#include <pv/status.h>

namespace ea4  { namespace pva {

/** Run-time exception of the EA4 serice */
class ArchiveRequestException : public std::runtime_error {
public:
    
    ArchiveRequestException(epics::pvData::Status::StatusType status, 
			    std::string const & message) :
       std::runtime_error(message), m_status(status) {
    }

    /** returns status */
    epics::pvData::Status::StatusType getStatus() const {
        return m_status;
    }
    
private:

    epics::pvData::Status::StatusType m_status;
};

  }}

#endif
