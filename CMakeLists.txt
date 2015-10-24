cmake_minimum_required(VERSION 2.8.9)
include(${SCRIPT_DIR}/CMakeAll.txt)

project("huffman_prj")
message("Project name   : ${PROJECT_NAME}")
message("Project include: ${PROJECT_SOURCE_DIR}")

set(libname "huffman")
set(binname "huff")

set( HEADER
	Stream.h
    Huffman.h
)

set( SOURCE
    FrequencyTable.h
    FrequencyTable.cpp
    Huffman.h
    Huffman.cpp
    HuffmanCode.h
    HuffmanCode.cpp
    HuffmanNode.h
    HuffmanNode.cpp
    MemBuffer.h
    MemBuffer.cpp
    Stream.h
    Stream.cpp
    Util.h
    Util.cpp
    SymbolEncode.cpp
    SymbolEncode.h
    main.cpp
)

source_group( "code" FILES ${CODE} )

add_library(${libname}         SHARED ${SOURCE} )
add_library(${libname}_static  STATIC ${SOURCE} )

# install the bits to the final location
install(TARGETS ${libname}          DESTINATION ${inst_lib_dir} )
install(TARGETS ${libname}_static   DESTINATION ${inst_lib_dir} )

install(FILES   ${HEADER}   DESTINATION ${inst_include_dir}/${libname}) 

target_link_libraries(${libname}
)

############################################################################### 

set(ENV{SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
set(ENV{DOC_DIR}    ${inst_doc_dir}/${libname} )

message("Source dir: $ENV{SOURCE_DIR} " )
message("Doc    dir: $ENV{DOC_DIR} " )

add_custom_target(doc
    COMMAND ${SCRIPT_DIR}/doxigen_stup ${DOXYGEN_EXECUTABLE}  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.dox $ENV{SOURCE_DIR} $ENV{DOC_DIR}
    #export SOURCE_DIR=$ENV{SOURCE_DIR};export DOC_DIR=$ENV{DOC_DIR};${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.dox 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation" VERBATIN
)

###############################################################################

add_executable(${binname}
	main.cpp
)

target_link_libraries(  ${binname}
	${libname}_static
)

install(TARGETS ${binname} DESTINATION ${inst_bin_dir} )

###############################################################################

