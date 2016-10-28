
set (ea4Mongo_src
  ./storage/DatablockMONGO.cpp
  ./storage/RawDataReaderMONGO.cpp
  ./storage/ReaderFactoryMONGO.cpp
  ./pva/ServerConfigMONGO.cpp
  ./pva/InfoQueryMONGO.cpp
  ./pva/FilterQueryMONGO.cpp
  ./pva/ReadQueryMONGO.cpp
  ./pva/ArchiveRPCServiceMONGO.cpp
  ./pva/ActCommandMONGO.cpp
  ./pva/CollectActionMONGO.cpp
)

add_library(ea4Mongo SHARED ${ea4Mongo_src})
add_library(ea4MongoStatic STATIC ${ea4Mongo_src})

set_target_properties(ea4MongoStatic PROPERTIES OUTPUT_NAME ea4Mongo)

install(TARGETS ea4Mongo LIBRARY DESTINATION lib/${EPICS_HOST_ARCH})
install(FILES ${CMAKE_BINARY_DIR}/src/mongo/libea4Mongo.a DESTINATION ./lib/${EPICS_HOST_ARCH})



