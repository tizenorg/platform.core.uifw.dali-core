CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(tct_coreapi_utc)

INCLUDE(FindPkgConfig)
SET(BIN_DIR "/opt/usr/bin")
SET(ERR_LOG "\"/tmp/tclog\"")


INCLUDE_DIRECTORIES(
	src/
	src/common
)

add_definitions(-DERR_LOG=${ERR_LOG})
ADD_SUBDIRECTORY(src)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ")
