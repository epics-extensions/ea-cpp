project (apps-mongo-pva-src)

set(CMAKE_CXX_FLAGS 
"${CMAKE_CXX_FLAGS} -D ARCH_VERSION=4 -D ARCH_RELEASE=0 -D ARCH_PATCH=0")

add_executable(ArchiveServerMONGO ArchiveServerMONGO.cpp)

target_link_libraries(ArchiveServerMONGO ea4Common ea4Classic ea4Mongo)
target_link_libraries(ArchiveServerMONGO ${MongoDB_LIBRARIES})
target_link_libraries(ArchiveServerMONGO ${EPICS_LIB_CA})
target_link_libraries(ArchiveServerMONGO ${EPICS_LIB_COM})
target_link_libraries(ArchiveServerMONGO ${EPICS4_CPP_LIBPVMB})
target_link_libraries(ArchiveServerMONGO ${EPICS4_CPP_LIBPVDATA})
target_link_libraries(ArchiveServerMONGO ${EPICS4_CPP_LIBPVACCESS})
target_link_libraries(ArchiveServerMONGO ${Boost_LIBRARIES} )
target_link_libraries(ArchiveServerMONGO ${XERCESC_LIBRARIES} rt)

install(TARGETS ArchiveServerMONGO RUNTIME DESTINATION bin/${EPICS_HOST_ARCH})



