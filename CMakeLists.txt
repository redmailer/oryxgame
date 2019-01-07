CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

add_definitions("-std=c++11 -g -Wall")

include_directories($ENV{C_INCLUDE_PATH})

LINK_LIBRARIES(pthread)

LINK_LIBRARIES($ENV{LD_LIBRARY_PATH}/libprotobuf.so)

#add_subdirectory(oryx)
#add_subdirectory(task)
AUX_SOURCE_DIRECTORY(./oryx ORYX_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./task TASK_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./proto/pb PROTO_DIR_SRCS)


#SET(CPP_COMPILE_OPT," -g -Wall -pthread -lstdc++ -std=c++11 ")
AUX_SOURCE_DIRECTORY(./logic LOGIC_DIR_SRCS)
add_executable(oryxServer ${LOGIC_DIR_SRCS} ${ORYX_DIR_SRCS} ${TASK_DIR_SRCS} ${PROTO_DIR_SRCS})

AUX_SOURCE_DIRECTORY(./clientTest CLIENT_DIR_SRCS)
add_executable(clientTest ${CLIENT_DIR_SRCS} ${ORYX_DIR_SRCS} ${TASK_DIR_SRCS} ${PROTO_DIR_SRCS})

#target_link_libraries(OryxServer oryx)
#target_link_libraries(OryxServer task)