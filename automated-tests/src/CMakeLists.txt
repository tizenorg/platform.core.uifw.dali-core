SET(PKG_NAME "dali")

SET(EXEC_NAME "Cts-${PKG_NAME}-core")

SET(CAPI_LIB "dali-core")

SET(TC_SOURCES
        property-notification/CtsDali-property-notification.cpp
        property-notification/CtsDali-property-notification-common.cpp
)

LIST(APPEND TC_SOURCES
        test-harness.cpp
)

LIST(APPEND TC_SOURCES
         common/dali-common.cpp
)


PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
     dali-adaptor
     ${CAPI_LIB}
)
INCLUDE_DIRECTORIES(
    ${${CAPI_LIB}_INCLUDE_DIRS}
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
    pthread
)

SET(CMAKE_EXE_LINKER_FLAGS "-L${STACK_LIB_DIR} -lpthread -lm -lrt -ldl -lstdc++")

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${${CAPI_LIB}_CFLAGS_OTHER} -O0 -ggdb --coverage -Wall -Werror -fPIE -Wall")

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINKER_FLAGS "${CMAKE_CXX_LINKER_FLAGS} -L${directory} ")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})


SET(EXE_DALI_CORE_CTS_LINKER_FLAGS "-L${${CAPI_LIB}_LIBRARY_DIRS} -ldali-core -ldali-adaptor ")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EXE_DALI_CORE_CTS_LINKER_FLAGS}")
