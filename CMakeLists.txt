cmake_minimum_required (VERSION 3.10)
project (alibc_project C)

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
	source/ErrorLogger.c
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
	source/StringObject.c
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

install(TARGETS alibc 
		DESTINATION /usr/local/lib/)
