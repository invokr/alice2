#------------------------------------------------------------
# Standard CMake Stuff
#------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 )
PROJECT ( alice2 )

SET ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake )

MESSAGE ( STATUS "Source DIR is ${PROJECT_SOURCE_DIR}" )
MESSAGE ( STATUS "Binary DIR is ${PROJECT_BINARY_DIR}" )
MESSAGE ( STATUS "Build type is ${CMAKE_BUILD_TYPE}" )
MESSAGE ( STATUS "Libs extension is '${SUFFIXLIB}'. Bins extension is '${SUFFIXBIN}'." )
MESSAGE ( STATUS "Installation prefix directory is " ${CMAKE_INSTALL_PREFIX} )
MESSAGE ( STATUS "Host system is " ${CMAKE_HOST_SYSTEM} " with processor " ${CMAKE_HOST_SYSTEM_PROCESSOR} )
MESSAGE ( STATUS "Target system is " ${CMAKE_SYSTEM} " with processor " ${CMAKE_SYSTEM_PROCESSOR} )
MESSAGE ( STATUS "Compiler identified as " ${CMAKE_CXX_COMPILER_ID} )

#------------------------------------------------------------
# Compiler Setup
#------------------------------------------------------------

SET ( CMAKE_CXX_FLAGS "-O3 --std=c++11 -fPIC -Wall" )

#------------------------------------------------------------
# Include and configure all libraries
#------------------------------------------------------------

IF ( UNIX )
    # Protocol Buffers
    FIND_PACKAGE( Protobuf REQUIRED )
    INCLUDE_DIRECTORIES( ${PROTOBUF_INCLUDE_DIR} )
    SET ( PROTOC "protoc" )

    # Snappy
    INCLUDE ( FindSnappy )
    FIND_PACKAGE ( Snappy REQUIRED )
    INCLUDE_DIRECTORIES( ${SNAPPY_INCLUDE_DIR} )
ENDIF ( )

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendors/catch/single_include
    ${CMAKE_SOURCE_DIR}/vendors/sparsehash/src/
)

#------------------------------------------------------------
# Generate protobuffer files and target
#------------------------------------------------------------

SET ( PROTO_S1 ai_activity demo dota_broadcastmessages dota_clientmessages dota_commonmessages dota_modifiers
    dota_usermessages gcsdk_gcmessages netmessages network_connection networkbasetypes usermessages steammessages
)

SET ( PROTO_S2 demo dota_broadcastmessages dota_clientmessages dota_commonmessages dota_modifiers
    dota_usermessages gcsdk_gcmessages netmessages network_connection networkbasetypes usermessages steammessages
)

FOREACH ( PROTO ${PROTO_S1} )
    EXECUTE_PROCESS ( COMMAND ${PROTOC}
        ${CMAKE_SOURCE_DIR}/proto/Source1/${PROTO}.proto
        --cpp_out=${CMAKE_SOURCE_DIR}/src/alice2/proto/source1
        --proto_path=${CMAKE_SOURCE_DIR}/proto/Source1/
    )

    LIST(APPEND PROTO_SOURCES_1 ${CMAKE_SOURCE_DIR}/src/alice2/proto/source1/${PROTO}.pb.cc)
    LIST(APPEND PROTO_HEADERS_1 ${CMAKE_SOURCE_DIR}/src/alice2/proto/source1/${PROTO}.pb.h)
ENDFOREACH()

FOREACH ( PROTO ${PROTO_S2} )
    EXECUTE_PROCESS ( COMMAND ${PROTOC}
        ${CMAKE_SOURCE_DIR}/proto/Source2/${PROTO}.proto
        --cpp_out=${CMAKE_SOURCE_DIR}/src/alice2/proto/source2
        --proto_path=${CMAKE_SOURCE_DIR}/proto/Source2/
    )

    LIST(APPEND PROTO_SOURCES_2 ${CMAKE_SOURCE_DIR}/src/alice2/proto/source2/${PROTO}.pb.cc)
    LIST(APPEND PROTO_HEADERS_2 ${CMAKE_SOURCE_DIR}/src/alice2/proto/source2/${PROTO}.pb.h)
ENDFOREACH()

ADD_LIBRARY( alice2_proto1 SHARED ${PROTO_SOURCES_1} )
ADD_LIBRARY( alice2_proto1_static STATIC ${PROTO_SOURCES_1} )
SET_TARGET_PROPERTIES(alice2_proto1_static PROPERTIES OUTPUT_NAME alice2_proto1)

ADD_LIBRARY( alice2_proto2 SHARED ${PROTO_SOURCES_2} )
ADD_LIBRARY( alice2_proto2_static STATIC ${PROTO_SOURCES_2} )
SET_TARGET_PROPERTIES(alice2_proto2_static PROPERTIES OUTPUT_NAME alice2_proto2)

#------------------------------------------------------------
# Build alice
#------------------------------------------------------------

SET ( ALICE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/alice2/dem.cpp
    ${CMAKE_SOURCE_DIR}/src/alice2/dem_file.cpp
)

ADD_LIBRARY( alice2 SHARED ${ALICE_SOURCES} )
ADD_LIBRARY( alice2_static STATIC ${ALICE_SOURCES} )
SET_TARGET_PROPERTIES(alice2_static PROPERTIES OUTPUT_NAME alice2)

#------------------------------------------------------------
# Build unit test
#------------------------------------------------------------

ADD_EXECUTABLE ( alice_test
    ${CMAKE_SOURCE_DIR}/test/test.cpp
    ${CMAKE_SOURCE_DIR}/test/alice2/util/constexpr_hash.cpp
    ${CMAKE_SOURCE_DIR}/test/alice2/util/delegate.cpp
    ${CMAKE_SOURCE_DIR}/test/alice2/util/dict.cpp
    ${CMAKE_SOURCE_DIR}/test/alice2/util/exception.cpp
    ${CMAKE_SOURCE_DIR}/test/alice2/util/noncopyable.cpp
    ${CMAKE_SOURCE_DIR}/test/alice2/util/varint.cpp
    ${CMAKE_SOURCE_DIR}/test/alice2/util/ztime.cpp
    ${CMAKE_SOURCE_DIR}/test/alice2/dem.cpp
)

TARGET_LINK_LIBRARIES ( alice_test
    ${SNAPPY_LIBRARIES}
    alice2_static
    alice2_proto2_static
)