CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

add_definitions("-std=c++11 -g -Wall")

LINK_LIBRARIES(pthread)

# add_subdirectory(oryx)
# add_subdirectory(task)
AUX_SOURCE_DIRECTORY(./logic LOGIC_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./oryx ORYX_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./task TASK_DIR_SRCS)

SET(CPP_COMPILE_OPT," -g -Wall -pthread -lstdc++ -std=c++11 ")
add_executable(OryxServer ${LOGIC_DIR_SRCS} ${ORYX_DIR_SRCS} ${TASK_DIR_SRCS})

# target_link_libraries(OryxServer oryx)
# target_link_libraries(OryxServer task)
