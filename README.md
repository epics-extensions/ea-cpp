# ea-cpp
The project provides a V4-based middle layer of the EPICS archiver. The new extension addresses several tasks:
* replacement of the original XML-RPC interface with the efficient EPICS V4 protocol
* generalization of the I/O library with a plugin mechanism for supporting new data drivers
* transition to the MongoDB-based indexing service of the Channel Archiver
* integration of historical control data and metadata stores of experimental facilities 

Its interface is described in the eapy module (https://github.com/epics-extensions/eapy).

### Prerequisites: 

* **EPICS V4**: https://github.com/epics-base
* **EPICS V3**: http://www.aps.anl.gov/epics/

