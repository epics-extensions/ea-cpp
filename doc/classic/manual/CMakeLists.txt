project (doc-classic-manual)

# add_custom_target (manual ALL 
#    COMMAND ${LATEX_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/manual
#    SOURCES ${PROJECT_BINARY_DIR}/doc/manual/)

set (data_src_dir  "${CMAKE_SOURCE_DIR}/doc/classic/manual")
set (data_dest_dir "${CMAKE_BINARY_DIR}/doc/classic/manual")

# DTD files

# engine

configure_file(${CMAKE_SOURCE_DIR}/apps/common/engine/xml/engineconfig.dtd
    ${data_dest_dir}/engineconfig.dtd
    COPYONLY
)

configure_file(${CMAKE_SOURCE_DIR}/apps/common/engine/xml/engineconfig.xml
    ${data_dest_dir}/engineconfig.xml
    COPYONLY
)

# data server

configure_file(${CMAKE_SOURCE_DIR}/apps/common/pvrpc/xml/serverconfig.dtd
    ${data_dest_dir}/serverconfig.dtd
    COPYONLY
)

configure_file(${CMAKE_SOURCE_DIR}/apps/common/pvrpc/xml/serverconfig.xml
    ${data_dest_dir}/serverconfig.xml
    COPYONLY
)

# indextool

configure_file(${CMAKE_SOURCE_DIR}/apps/common/indextool/xml/indexconfig.dtd
    ${data_dest_dir}/indexconfig.dtd
    COPYONLY
)

configure_file(${CMAKE_SOURCE_DIR}/apps/common/indextool/xml/indexconfig.xml
    ${data_dest_dir}/indexconfig.xml
    COPYONLY
)

# archive setup

configure_file(${CMAKE_SOURCE_DIR}/apps/legacy/daemon/xml/archiveconfig.dtd
    ${data_dest_dir}/archiveconfig.dtd
    COPYONLY
)

configure_file(${CMAKE_SOURCE_DIR}/apps/legacy/daemon/xml/archiveconfig.xml
    ${data_dest_dir}/archiveconfig.xml
    COPYONLY
)

# daemon

configure_file(${CMAKE_SOURCE_DIR}/apps/legacy/daemon/xml/ArchiveDaemon.dtd
    ${data_dest_dir}/ArchiveDaemon.dtd
    COPYONLY
)

configure_file(${CMAKE_SOURCE_DIR}/apps/legacy/daemon/xml/ArchiveDaemon.xml
    ${data_dest_dir}/ArchiveDaemon.xml
    COPYONLY
)

# TEX files

set(manual_inputs
 files/aiexample.db
 bib.tex
 binary.tex
 changes.tex
 channel.tex
 dataserver.tex
 datatool.tex
 engine.tex
 err_faq.tex
 examplesetup.tex
 export.tex
 indextool.tex
 indices.tex
 install.tex
 interpol.tex
 javaclient.tex
 legacy.tex
 matlab.tex
 overview.tex
 perlclient.tex
 retrieval.tex
 rtree.tex
 sampling.tex
 stats.tex
 storage.tex
 striptool.tex
 timezones.tex
 waveforms.tex
 xmlrpc.tex
)

add_latex_document(manual.tex
INPUTS ${manual_inputs}
CONFIGURE manual.tex
IMAGE_DIRS images
USE_INDEX
NO_DEFAULT)
# DEFAULT_PDF)

