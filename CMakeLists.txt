CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

add_definitions("-std=c++11 -g -Wall")

include_directories(./thirdSource/include)

LINK_LIBRARIES(pthread)
LINK_LIBRARIES("${CMAKE_CURRENT_SOURCE_DIR}/thirdSource/lib/libprotobuf.a")

AUX_SOURCE_DIRECTORY(./oryx ORYX_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./task TASK_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./proto/pb PROTO_DIR_SRCS)


AUX_SOURCE_DIRECTORY(./logic LOGIC_DIR_SRCS)
add_executable(oryxServer ${LOGIC_DIR_SRCS} ${ORYX_DIR_SRCS} ${TASK_DIR_SRCS} ${PROTO_DIR_SRCS})

#AUX_SOURCE_DIRECTORY(./clientTest CLIENT_DIR_SRCS)
#add_executable(clientTest ${CLIENT_DIR_SRCS} ${ORYX_DIR_SRCS} ${TASK_DIR_SRCS} ${PROTO_DIR_SRCS})

