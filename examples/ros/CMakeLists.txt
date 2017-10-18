
if ( roslib_FOUND )
	add_executable(rosNodeTalker RosNodeTalker.cpp)
	target_link_libraries(rosNodeTalker eeros ${EEROS_LIBS})
	
	add_custom_command(OUTPUT copyHalTest3Config POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${EEROS_SOURCE_DIR}/examples/ros/HalConfigRos.json" "${EEROS_BINARY_DIR}/examples/ros")
	add_custom_command(OUTPUT copyReadMe POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${EEROS_SOURCE_DIR}/examples/ros/ReadMe_RosExample.txt" "${EEROS_BINARY_DIR}/examples/ros")
	add_executable(rosExample RosExample.cpp copyHalTest3Config copyReadMe)
	target_link_libraries(rosExample eeros ${EEROS_LIBS})
endif()