alibc:
	cp -r source/* .
	gcc -c alib_cb_funcs.c
	gcc -c alib_dir.c
	gcc -c alib_file.c
	gcc -c alib_proc.c
	gcc -c alib_sockets.c
	gcc -c alib_string.c
	gcc -c alib_time.c
	gcc -c alib_types.c
	gcc -c ArrayList.c
	gcc -c BinaryBuffer.c
	gcc -c ClientListener.c
	gcc -c ComDataCheck.c
#	gcc -c CurlObject.c
	gcc -c DList.c
	gcc -c DListItem.c
	gcc -c EpollPack.c
	gcc -c ErrorLogger.c
#	gcc -c EventServer.c
	gcc -c FdClient.c
	gcc -c FdServer.c
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
	gcc -c ThreadedTimerEvent.c
	gcc -c Timer.c
	gcc -c TimerEvent.c
	gcc -c TimerEventHandler.c
#	gcc -c UvTcp.c	
#	gcc -c UvTcpClient.c
#	gcc -c UvTcpServer.c
	gcc -c zip.c
	gcc -c ZipFileIter.c
	gcc -c ancillary/fd_recv.c
	gcc -c ancillary/fd_send.c
	ar -rc libalibc.a *.o	
	rm -r *.o *.c ancillary/

install:
	mkdir -p /usr/local/include/alib-c/
	rm -f /user/local/inlcude/alib-c/*.h
	cp ./includes/*.h /usr/local/include/alib-c/
	cp libalibc.a /usr/local/lib/

all:
	make alibc
	make install
	
arduino:
	cp ./source/alib_cb_funcs.c .
	cp ./source/alib_time.c .
	cp ./source/alib_types.c .
	cp ./source/DList.c .
	cp ./source/DListItem.c .
	cp ./source/Endianess.c .
	cp ./source/ListItem.c .
	cp ./source/ListItemVal.c .
	
clean:
	rm -rf *.o *.c ancillary/ *.a