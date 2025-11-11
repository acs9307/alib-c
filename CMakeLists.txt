cmake_minimum_required (VERSION 3.10)
project (alibc_project C)

set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

# Option to build tests
option(BUILD_TESTS "Build test suite" ON)

# Option to build with ErrorLogger (requires json-c)
option(BUILD_ERROR_LOGGER "Build with ErrorLogger support (requires json-c)" OFF)

# Check for json-c if ErrorLogger is enabled
if(BUILD_ERROR_LOGGER)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(JSON_C REQUIRED json-c)
	set(ERROR_LOGGER_SOURCE source/ErrorLogger.c)
	message(STATUS "Building with ErrorLogger support")
else()
	set(ERROR_LOGGER_SOURCE "")
	message(STATUS "Building without ErrorLogger (use -DBUILD_ERROR_LOGGER=ON to enable)")
endif()

add_library(alibc 
	source/alib_cb_funcs.c
	source/alib_dir.c
	source/alib_file.c
	source/alib_math.c
	source/alib_proc.c
	source/alib_sockets.c
	source/alib_string.c
	source/alib_time.c
	source/alib_types.c
	source/ArrayList.c
	source/BinaryBuffer.c
	source/ClientListener.c
	source/ComDataCheck.c
#	source/CurlObject.c
	source/DList.c
	source/DListItem.c
	source/EpollPack.c
	${ERROR_LOGGER_SOURCE}
#	source/EventServer.c
	source/FdClient.c
	source/FdServer.c
	source/ListHistory.c
	source/ListHistoryItem.c
	source/ListItem.c
	source/ListItemVal.c
	source/MemPool.c
	source/MutexObject.c
	source/proc_waiter.c
	source/RBuff.c
	source/RBuffIt.c
	source/server_structs.c
	source/signal_handler.c
	source/String.c
	source/StrParser.c
	source/StrRef.c
	source/TcpClient.c
	source/tcp_functions.c
	source/TcpServer.c
	source/ThreadedTimerEvent.c
	source/Timer.c
	source/TimerEvent.c
	source/TimerEventHandler.c
#	source/UvTcp.c	
#	source/UvTcpClient.c
#	source/UvTcpServer.c
	source/zip.c
	source/ZipFileIter.c
	source/ancillary/fd_recv.c
	source/ancillary/fd_send.c
)

target_include_directories(alibc PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	includes
)

# Link json-c if ErrorLogger is enabled
if(BUILD_ERROR_LOGGER)
	target_link_libraries(alibc ${JSON_C_LIBRARIES})
	target_include_directories(alibc PRIVATE ${JSON_C_INCLUDE_DIRS})
	target_compile_definitions(alibc PRIVATE HAVE_ERROR_LOGGER)
endif()

install(TARGETS alibc
	DESTINATION /usr/local/lib/)
install(DIRECTORY includes/ DESTINATION /usr/local/include/alib-c)

# Enable testing and add tests subdirectory
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
	message(STATUS "Building with tests enabled")
else()
	message(STATUS "Building without tests (use -DBUILD_TESTS=ON to enable)")
endif()
