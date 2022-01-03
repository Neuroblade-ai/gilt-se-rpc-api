cmake_minimum_required(VERSION 3.13)
project(gilt_storage_engine_rpc_api)

set(CMAKE_CXX_STANDARD 20)

# Proto file
get_filename_component(management_proto "protos/management.proto" ABSOLUTE)
get_filename_component(management_proto_path "${management_proto}" PATH)

set(MANAGEMENT_GENERATED_PROTO_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/management.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/management.pb.h"
    "${CMAKE_CURRENT_BINARY_DIR}/management.grpc.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/management.grpc.pb.h")

set(PROTO_DEPENDS ${management_proto} ${_PROTOBUF_LIBPROTOBUF} ${_GRPC_CPP_PLUGIN_EXECUTABLE})

add_custom_command(
      OUTPUT ${MANAGEMENT_GENERATED_PROTO_FILES}
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${management_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${management_proto}"
      DEPENDS "${PROTO_DEPENDS}")

set_source_files_properties(${MANAGEMENT_GENERATED_PROTO_FILES} PROPERTIES GENERATED TRUE)

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

include_directories(SYSTEM
    ${ARROW_INCLUDE_DIR}
    ${FMT_INCLUDE_DIR})

# management_proto
add_library(management_proto
  ${MANAGEMENT_GENERATED_PROTO_FILES})
target_link_libraries(management_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# management_helper
# add_library(management_helper
#   "helper.h"
#   "helper.cc")
# target_link_libraries(management_helper
#   management_proto
#   ${_REFLECTION}
#   ${_GRPC_GRPCPP}
#   ${_PROTOBUF_LIBPROTOBUF})

set(client_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/SEClient.cpp)
set(client_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/SEClient.h)

add_library(SEClient
  ${client_sources}
)
add_dependencies(SEClient fmt)

target_link_libraries(SEClient ${FMT_LIBRARIES}
  management_proto
  # management_helper
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  ${ARROW_LIBRARIES})
