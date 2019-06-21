cmake_minimum_required (VERSION 2.8)
set( APPNAME P2PLive)
project( ${APPNAME} )
SET(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc )
SET(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++ )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/device/common )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../include )

AUX_SOURCE_DIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR} DIR_SRCS)
AUX_SOURCE_DIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/device/common DIR_SRCS)
AUX_SOURCE_DIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/device/simulator DIR_SRCS)

if ( CROSS_COMPILE )
	link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/aarch64 )
else()
	link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x86 )
endif()

ADD_EXECUTABLE( ${APPNAME} ${DIR_SRCS} )
target_link_libraries( ${APPNAME} qnlinking pthread m )

