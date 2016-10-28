project (apps-common-indexdot-src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D ARCH_VERSION=3 -D ARCH_RELEASE=0 -D ARCH_PATCH=0")

add_executable(ArchiveIndexDot IndexDot.cpp )

target_link_libraries(ArchiveIndexDot ea4Common)
target_link_libraries(ArchiveIndexDot ${EPICS_LIB_CA})
target_link_libraries(ArchiveIndexDot ${EPICS_LIB_COM})
target_link_libraries(ArchiveIndexDot ${EPICS4_CPP_LIBPVMB})
target_link_libraries(ArchiveIndexDot ${EPICS4_CPP_LIBPVDATA})
target_link_libraries(ArchiveIndexDot ${EPICS4_CPP_LIBPVACCESS})
target_link_libraries(ArchiveIndexDot ${Boost_LIBRARIES} )
target_link_libraries(ArchiveIndexDot ${XERCESC_LIBRARIES} rt)

install(TARGETS ArchiveIndexDot RUNTIME DESTINATION bin/${EPICS_HOST_ARCH})


