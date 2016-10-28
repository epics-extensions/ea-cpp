project (apps-common-indextool-scripts)

set (data_src_dir  "${CMAKE_SOURCE_DIR}/apps/common/indextool/scripts")
set (data_dest_dir "${CMAKE_BINARY_DIR}/apps/common/indextool/scripts")

configure_file(
    ${data_src_dir}/ArchiveIndexMakeConfig
    ${data_dest_dir}/ArchiveIndexMakeConfig
    COPYONLY
)

install(PROGRAMS ArchiveIndexMakeConfig DESTINATION bin/${EPICS_HOST_ARCH})
