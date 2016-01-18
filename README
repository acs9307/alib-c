A simple object oriented library that contains helper functions and classes.

Never assume an object is threadsafe.  If in doubt, check the associated header file.
Some objects may state that they are thread safe, however the deletion function may not be thread
safe, there is no way to free an object in a thread safe manner. 

Conventions:	
	For a structure to be considered a class, it must follow the class naming convention as well 
	as have	a related 'new()' and 'del()' function which can dynamically allocate memory for
	the object as well as initialize it.  
	
	Most objects are dynamically allocated and must be manually freed when no longer needed.
	This was by design as copying large objects is very inefficient.  It is also best practice
	as most objects have dynamically allocated members, meaning you must uninitialize them anyways.
	This creates consistency and decreases the number of times memory must be copied.
	
	Nameing:
		Class declarations/definitions will ALWAYS have a capital letter for the first letter 
		of every word within the class name.  This is used to help determine how a struct 
		should be used.	

		All class functions will start with the class's name, except lifecycle functions. Parts of
		the function name should be seperated from each other as well as the class name
		using underscores, '_'.
	
		Only 'newClass()' and 'delClass()' functions MUST break the above function naming 
		convention.	Any other lifecycle function may also break the above rules if the 
		function name is a single word, function names such as 'freeAllClass()' are not 
		allowed and must be written as 'Class_free_all()' or 'freeClass_all()'.  Sometimes 
		lifecycle functions that do conform to the lifecycle function naming convention need 
		to be extended.  In these cases, you are allowed to add an extension to the name	
		by adding an underscore plus the extension name, i.e. 'newMyClass_ex()'.  All extended 
		lifecycle functions must accomplish the same basic job. This means that you can't have 
		two functions 'newClass()' and 'newClass_ex()' where the first creates an object and 
		the second prints a string, they must both allocate memory and return the object's pointer.
	
		Class files that have a '_protected' or '_private' appended to the end of them means that 
		their class definition is hidden and members should only be accessed through functions.  
		If they must be accessed directly, simply include the protected/private header file.
		
	Behavior:
		All objects' 'new()' methods must allocate memory for the object, initialize the object's
		members and return, either by return value or by an argument pointer, the newly allocated
		object.
		
		All objects' 'del()' method must take a double pointer to the object, free the object and
		any memory it is responsible for, and set the pointer to NULL.

Notes:
	A long time ago, in a dark corner of the programming universe, I was asked to use libuv for
	a TCP server for several applications.  It was at that time 'UvTcp', 'UvTcpClient', and 
	'UvTcpServer' were born.  NEVER use these unless absolutely necessary.  I thought about 
	deleting them, however they will remain for a short time incase I need them again.  Do
	not expect them to stay in later versions of the library as they are all very buggy mainly
	due to the retarded 'assert()' design by libuv's creators...nothing like having a library
	that crashes your application with no dicernable information as to why it crashed...
