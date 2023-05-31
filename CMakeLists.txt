cmake_minimum_required(VERSION 3.4)
project(EndpointDetector)

include(GNUInstallDirs)

# set version
set(VERSION_MAJOR 2)
set(VERSION_MINOR 1)
set(VERSION_PATCH 11)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("* System: ${CMAKE_SYSTEM_NAME}")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	message("* Library architecture: ${CMAKE_LIBRARY_ARCHITECTURE}")

	if(${CMAKE_LIBRARY_ARCHITECTURE} MATCHES "gnueabihf")
		set(TYCHE "tyche/linux/armhf")
	elseif(${CMAKE_LIBRARY_ARCHITECTURE} MATCHES "aarch64")
		set(TYCHE "tyche/linux/aarch64")
	elseif(${CMAKE_LIBRARY_ARCHITECTURE} MATCHES "x86_64")
		set(TYCHE "tyche/linux/x86_64")
	else()
		message(SEND_ERROR "not supported library architecture - ${CMAKE_LIBRARY_ARCHITECTURE}")
	endif()

	# RPATH is useful only for testing without installation.
	# Please use the '-DPACKAGING' option for debian packaging.
	if (NOT PACKAGING)
		link_libraries(-Wl,--rpath=${PROJECT_BINARY_DIR}/src)
	endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set(TYCHE "tyche/macOS")
	if (NOT PACKAGING)
		link_libraries(-Wl,-rpath,${PROJECT_BINARY_DIR}/src)
	endif()
else()
	message(SEND_ERROR "not supported system - ${CMAKE_SYSTEM_NAME}")
endif()

message("* Tyche path: ${TYCHE}")

# Global include directories
include_directories(include ${TYCHE}/Libraries/include)

add_subdirectory(src)
add_subdirectory(test)

# Install header files
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nugu-epd/)

# Install pkgconfig
configure_file(nugu-epd.pc.in ${PROJECT_BINARY_DIR}/nugu-epd.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/nugu-epd.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/)
