project (apps-common-indextool-src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D ARCH_VERSION=3 -D ARCH_RELEASE=0 -D ARCH_PATCH=0")

set (indextool_src 
  IndexInfo.cpp
  MasterIndex.cpp
)

add_executable(ArchiveIndexTool ${indextool_src} IndexTool.cpp )

target_link_libraries(ArchiveIndexTool ea4Common)
target_link_libraries(ArchiveIndexTool ${EPICS_LIB_CA})
target_link_libraries(ArchiveIndexTool ${EPICS_LIB_COM})
target_link_libraries(ArchiveIndexTool ${EPICS4_CPP_LIBPVMB})
target_link_libraries(ArchiveIndexTool ${EPICS4_CPP_LIBPVDATA})
target_link_libraries(ArchiveIndexTool ${EPICS4_CPP_LIBPVACCESS})
target_link_libraries(ArchiveIndexTool ${Boost_LIBRARIES} )
target_link_libraries(ArchiveIndexTool ${XERCESC_LIBRARIES} rt)

install(TARGETS ArchiveIndexTool RUNTIME DESTINATION bin/${EPICS_HOST_ARCH})


