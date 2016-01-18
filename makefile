libalibc.a:
	gcc -c alib_cb_funcs.c
	gcc -c alib_dir.c
	gcc -c alib_file.c
	gcc -c alib_sockets.c
	gcc -c alib_string.c
	gcc -c alib_types.c
	gcc -c ArrayList.c
	gcc -c BinaryBuffer.c
	gcc -c ClientListener.c
	gcc -c CurlObject.c
	gcc -c DList.c
	gcc -c DListItem.c
	gcc -c EpollPack.c
	gcc -c ErrorLogger.c
	gcc -c EventServer.c
	gcc -c FdClient.c
	gcc -c FdServer.c
	gcc -c flags.c
	gcc -c ListHistory.c
	gcc -c ListHistoryItem.c
	gcc -c ListItem.c
	gcc -c ListItemVal.c
	gcc -c MemPool.c
	gcc -c MutexObject.c
	gcc -c proc_waiter.c
	gcc -c server_structs.c
	gcc -c signal_handler.c
	gcc -c String.c
	gcc -c StringObject.c
	gcc -c TcpClient.c
	gcc -c tcp_functions.c
	gcc -c TcpServer.c
	gcc -c UvTcp.c	
	gcc -c UvTcpClient.c
	gcc -c UvTcpServer.c
	gcc -c ancillary/fd_recv.c
	gcc -c ancillary/fd_send.c
	ar -rc libalibc.a *.o /usr/local/lib/libcurl.a /usr/local/lib/libevent.a /usr/local/lib/libuv.a	
	#ar -rc libalibc.a alib_cb_funcs.o alib_dir.o alib_file.o alib_sockets.o alib_string.o alib_types.o ArrayList.o BinaryBuffer.o ClientListener.o CurlObject.o DList.c DListItem.c ErrorLogger.o EventServer.o FdClient.o FdServer.o flags.o ListHistory.o ListHistoryItem.o ListItem.o ListItemVal.o MemPool.o MutexObject.o proc_waiter.o server_structs.o signal_handler.o String.o StringObject.o TcpClient.o tcp_functions.o TcpServer.o UvTcp.o UvTcpClient.o UvTcpServer.o fd_recv.o fd_send.o /usr/local/lib/libcurl.a /usr/local/lib/libevent.a /usr/local/lib/libuv.a	
	rm *.o
