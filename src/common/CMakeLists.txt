
set (ea4tools_src
  ./tools/timing.cpp
  ./tools/ArchiveException.cpp
  ./tools/BinIO.cpp
  ./tools/Bitset.cpp
  ./tools/Conversions.cpp
  ./tools/GenericException.cpp
  ./tools/epicsTimeHelper.cpp
  ./tools/MsgLogger.cpp
  ./tools/Filename.cpp
  ./tools/FUX.cpp
  ./tools/OrderedMutex.cpp
  ./tools/Guard.cpp
  ./tools/ConcurrentList.cpp
  ./tools/Throttle.cpp
  ./tools/ThrottledMsgLogger.cpp
  ./tools/NetTools.cpp
  ./tools/ArgParser.cpp
  ./tools/Lockfile.cpp
  ./tools/CGIDemangler.cpp
  ./tools/RegularExpression.cpp
  ./tools/IndexConfig.cpp
  ./tools/ASCIIParser.cpp
  ./tools/BenchTimer.cpp
  ./tools/UnitTestLib.cpp
)

set (ea4pva_src
  ./pva/ArchiveServer.cpp
  ./pva/ArchiveChannelProviderFactory.cpp
  ./pva/ArchiveChannelProvider.cpp
  ./pva/ArchiveChannelRequester.cpp
  ./pva/ArchiveService.cpp
  ./pva/Archives.cpp
  ./pva/ArchiveRPCChannel.cpp
  ./pva/ArchiveRPCRequest.cpp
  ./pva/ArchiveRPCServiceRegistry.cpp
  ./pva/ArchiveRPCService.cpp
  ./pva/ArchiveRPCClient.cpp
  ./pva/ArchiveMonitorChannel.cpp
  ./pva/ArchiveMonitorRequest.cpp
  ./pva/ArchiveMonitorServiceRegistry.cpp
  ./pva/ArchiveMonitorService.cpp
  ./pva/ArchiveMonitorClient.cpp
  ./pva/ArchiveCommand.cpp
  ./pva/Session.cpp
  ./pva/OpenSessionCommand.cpp
  ./pva/Collection.cpp
  ./pva/ActCommand.cpp
  ./pva/StreamValuesCommand.cpp
  ./pva/Status.cpp
  ./pva/Query.cpp
  ./pva/ReadQuery.cpp
  ./pva/RawValuesBuilder.cpp
)

set (ea4storage_src
  ./storage/FileAllocator.cpp
  ./storage/NameHash.cpp
  ./storage/Interval.cpp
  ./storage/RTree.cpp
  ./storage/Index.cpp
  ./storage/IndexFile.cpp
  ./storage/CtrlInfo.cpp
  ./storage/RawValue.cpp
  ./storage/DataReader.cpp
#  ./storage/RawDataReader.cpp
#  ./storage/RawDataReaderRegistry.cpp
#  ./storage/ShallowIndexRawDataReader.cpp
#  ./storage/PlotReader.cpp
#  ./storage/AverageReader.cpp
#  ./storage/LinearReader.cpp
#  ./storage/ReaderFactory.cpp
  ./storage/AutoIndex.cpp
  ./storage/ListIndex.cpp
  ./storage/HashTable.cpp
  ./storage/DataFileRegistry.cpp
#  ./storage/SpreadsheetReader.cpp
)

add_library(ea4Common SHARED ${ea4storage_src} ${ea4tools_src} ${ea4pva_src})
add_library(ea4CommonStatic STATIC ${ea4storage_src} ${ea4tools_src} ${ea4pva_src})

set_target_properties(ea4CommonStatic PROPERTIES OUTPUT_NAME ea4Common)

install(TARGETS ea4Common LIBRARY DESTINATION lib/${EPICS_HOST_ARCH})
install(FILES ${CMAKE_BINARY_DIR}/src/common/libea4Common.a DESTINATION ./lib/${EPICS_HOST_ARCH})



