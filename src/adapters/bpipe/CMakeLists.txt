file(GLOB_RECURSE SRC_FILES *.cc)

file(GLOB_RECURSE BPIPE_DIR /opt/local/*/blpapi_session.h)

if(BPIPE_DIR)
  message(STATUS "Found BPIPE " ${BPIPE_DIR})
  get_filename_component(BPIPE_DIR ${BPIPE_DIR} DIRECTORY)
  get_filename_component(BPIPE_DIR ${BPIPE_DIR} DIRECTORY)
  include_directories(${BPIPE_DIR}/include)
  add_library(bpipe MODULE ${SRC_FILES})
  target_link_libraries(bpipe ${BPIPE_DIR}/Linux/libblpapi3_64.so)
endif()

