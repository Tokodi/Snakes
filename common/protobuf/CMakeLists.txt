find_package(Protobuf REQUIRED)

set(lib_name message)

protobuf_generate_cpp(
    PROTO_SRCS
    PROTO_HDRS
    message.proto
)

add_library(${lib_name} STATIC
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_link_libraries(${lib_name}
    ${Protobuf_LIBRARIES}
)

set(generated_protobuf_include_path ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "Path to generated protobuf files.")

