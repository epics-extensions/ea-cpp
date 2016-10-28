if(DOXYGEN_FOUND)

    set(DOXY_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
    set(DOXY_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

    set(CONFIGURE_DOXYGEN_GENERATE_HTML "YES")
    set(CONFIGURE_DOXYGEN_GENERATE_LATEX "NO")
    set(CONFIGURE_DOXYGEN_QUIET "NO")
    set(CONFIGURE_DOXYGEN_EXCLUDE_PATTERNS ".svn CMakeFiles ")

    set(IBASE ${CMAKE_SOURCE_DIR}/include/common/base)
    set(ITLS ${CMAKE_SOURCE_DIR}/include/common/tools)
    set(ISTRG ${CMAKE_SOURCE_DIR}/include/common/storage)
    set(IENG ${CMAKE_SOURCE_DIR}/include/common/engine)
    set(IPVA ${CMAKE_SOURCE_DIR}/include/common/pva)
    set(IION ${CMAKE_SOURCE_DIR}/include/common/ion)
    set(ICSTRG ${CMAKE_SOURCE_DIR}/include/classic/storage)
    set(ICENG ${CMAKE_SOURCE_DIR}/include/classic/engine)
    set(ICPVA ${CMAKE_SOURCE_DIR}/include/classic/pva)
    set(IMSTRG ${CMAKE_SOURCE_DIR}/include/mongo/storage)
    set(IMPVA ${CMAKE_SOURCE_DIR}/include/mongo/pva)

    set(CONFIGURE_DOXYGEN_INPUT 
      "${IBASE} ${ITLS} ${ISTRG} ${IENG} ${IPVA} ${IION} ${ICSTRG} ${ICENG} ${ICPVA} ${IMSTRG} ${IMPVA}")

    message(STATUS "Configuring ${DOXY_IN} as ${DOXY_CONFIG}")
    configure_file(${DOXY_IN} ${DOXY_CONFIG} @ONLY)

    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "html;latex")

  add_custom_target (Docs ALL 
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doc/api/Doxyfile
    SOURCES ${PROJECT_BINARY_DIR}/doc/api/Doxyfile)

else(DOXYGEN_FOUND)
    message(STATUS "Can not find doxygen. Documentation will not be generated.")
endif(DOXYGEN_FOUND)
