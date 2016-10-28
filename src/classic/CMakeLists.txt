
set (ea4Classic_src
  ./storage/DataFile.cpp
  ./storage/DataFileManagerEA3.cpp
  ./storage/DataWriter.cpp
  ./storage/RawDataReaderEA3.cpp
  ./storage/ShallowIndexRawDataReaderEA3.cpp
#  ./storage/RawDataReaderFactoryEA3.cpp
  ./storage/ReaderFactoryEA3.cpp
  ./pva/ServerConfigEA3.cpp
  ./pva/InfoQueryCLS.cpp
  ./pva/ArchivesQueryCLS.cpp
  ./pva/PVsQueryCLS.cpp
  ./pva/ReadQueryCLS.cpp
  ./pva/DbrBuilderCLS.cpp
  ./pva/ArchiveRPCServiceEA3.cpp
  ./pva/ActCommandCLS.cpp
  ./pva/CollectActionCLS.cpp
)

add_library(ea4Classic SHARED ${ea4Classic_src})
add_library(ea4ClassicStatic STATIC ${ea4Classic_src})

set_target_properties(ea4ClassicStatic PROPERTIES OUTPUT_NAME ea4Classic)

install(TARGETS ea4Classic LIBRARY DESTINATION lib/${EPICS_HOST_ARCH})
install(FILES ${CMAKE_BINARY_DIR}/src/classic/libea4Classic.a DESTINATION ./lib/${EPICS_HOST_ARCH})


