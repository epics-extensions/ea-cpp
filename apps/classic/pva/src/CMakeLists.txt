project (apps-classic-pva-src)

set(CMAKE_CXX_FLAGS 
"${CMAKE_CXX_FLAGS} -D ARCH_VERSION=4 -D ARCH_RELEASE=0 -D ARCH_PATCH=0")

add_executable(ArchiveServer ArchiveServer.cpp)

target_link_libraries(ArchiveServer ea4Common ea4Classic)
target_link_libraries(ArchiveServer ${MongoDB_LIBRARIES})
target_link_libraries(ArchiveServer ${EPICS_LIB_CA})
target_link_libraries(ArchiveServer ${EPICS_LIB_COM})
target_link_libraries(ArchiveServer ${EPICS4_CPP_LIBPVMB})
target_link_libraries(ArchiveServer ${EPICS4_CPP_LIBPVDATA})
target_link_libraries(ArchiveServer ${EPICS4_CPP_LIBPVACCESS})
target_link_libraries(ArchiveServer ${Boost_LIBRARIES} )
target_link_libraries(ArchiveServer ${XERCESC_LIBRARIES} rt)

install(TARGETS ArchiveServer RUNTIME DESTINATION bin/${EPICS_HOST_ARCH})







