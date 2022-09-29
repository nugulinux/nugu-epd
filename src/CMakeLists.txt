set(TARGET_LIB nugu-epd)
file(GLOB_RECURSE SRC *.c)

# build
add_library(${TARGET_LIB} SHARED ${SRC})

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	target_link_libraries(${TARGET_LIB} PRIVATE
		-lstdc++
		-L${CMAKE_SOURCE_DIR}/${TYCHE}/Libraries
		-lTycheEpd -lTycheCommon -lTycheSpeex)
else()
	target_link_libraries(${TARGET_LIB} PRIVATE
		-Wl,--version-script=${CMAKE_SOURCE_DIR}/nugu-epd.map
		-L${CMAKE_SOURCE_DIR}/${TYCHE}/Libraries
		-lTycheEpd -lTycheCommon -lTycheSpeex)
endif()

# install
set_target_properties(${TARGET_LIB}
	PROPERTIES VERSION ${VERSION} SOVERSION ${VERSION_MAJOR} OUTPUT_NAME ${TARGET_LIB})
install(TARGETS ${TARGET_LIB} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
