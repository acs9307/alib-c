#include "CurlObject.h"

/*******INITIALIZATION*******/
/* Initializes Curl.  Must be called before anything other CurlObject
 * functions.  CurlObject_uninit must be called when no more operations are needed.
 * Returns CURLE_OK when successful, anything else is an error code.*/
CURLcode CurlObject_init(){return(curl_global_init(CURL_GLOBAL_ALL));}

/* Uninitializes Curl.  Used when no more operations are needed with
 * the CurlObject.  CurlObject_init() must be called before CurlObject_uninit
 * can be called.*/
void CurlObject_uninit(){curl_global_cleanup();}
/****************************/

/*******PRIVATE FUNCTIONS*******/
static size_t parse_data(void* buffer, size_t size, size_t count, void* user_obj)
{
	CurlObject* c_obj = user_obj;

	//If no CurlObject is given return an error.
	if(!c_obj)return(0);

	if(!c_obj->buff)
	{
		c_obj->buff = evbuffer_new();

		//Buffer could not be made, return an error.
		if(!c_obj->buff)
			return(0);
	}

	evbuffer_add(c_obj->buff, buffer, count * size);
	return(count);
}
/*******************************/

/*******PUBLIC MEMBER FUNCTIONS*******/
/* Sends a get request to the CurlObject's URL and fills
 * the CurlObject's buffer with the response.
 *
 * Return Codes:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_get(CurlObject* c_obj)
{
	if(!c_obj)
		return(CURL_OBJECT_BAD_ARG);

	//Clear the buffer if one still exists.
	if(c_obj->buff)
	{
		evbuffer_free(c_obj->buff);
		c_obj->buff = NULL;
	}

	return(curl_easy_perform(c_obj->curl));
}

/* Sends a post request to the CurlObject's URL and fills
 * the CurlObject's buffer with the response.
 * Return Codes:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_post(CurlObject* c_obj, char** headers,
		unsigned int header_count, char* data, unsigned long data_len)
{
	struct curl_slist* list = NULL;
	unsigned int i = 0;
	int err;

	if(!c_obj || (!headers && header_count) || (!data && data_len > 0))
		return(CURL_OBJECT_BAD_ARG);

	if(c_obj->buff)
	{
		evbuffer_free(c_obj->buff);
		c_obj->buff = NULL;
	}

	for(i = 0; i < header_count; ++headers)
		list = curl_slist_append(list, *headers);
	curl_easy_setopt(c_obj->curl, CURLOPT_HTTPHEADER, list);

	if(data)
	{
		curl_easy_setopt(c_obj->curl, CURLOPT_POSTFIELDSIZE, data_len);
		curl_easy_setopt(c_obj->curl, CURLOPT_COPYPOSTFIELDS, data);
	}

	err = curl_easy_perform(c_obj->curl);
	curl_slist_free_all(list);

	return(err);
}

/* Returns a StringObject of the received data from the connection.
 *
 * If no data is immediately available, the call will fail.
 * Returned value MUST be freed by the caller.
 *
 * Returns:
 * 		NULL: Failure or error.
 * 		StringObject*: A newly allocated string object containing
 * 			the received data. */
StringObject* CurlObject_recv(CurlObject* c_obj)
{
	StringObject* str;
	const size_t buff_size = 8 * 1024;
	char buff[8 * 1024];
	size_t recv_count;

	if(!c_obj)return(NULL);

	str = newStringObject();
	if(!str)return(NULL);

	while(curl_easy_recv(c_obj->curl, buff, buff_size, &recv_count) == CURLE_OK)
		str->append_count(str, buff, recv_count);

	return(str);
}
/*************************************/

/*******PUBLIC ACCESSOR FUNCTIONS*******/
/* Set the url for the given CurlObject.
 *
 * Returns:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_set_url(CurlObject* c_obj, const char* url)
{
	int err;

	if(!c_obj || !url)
		return(CURL_OBJECT_BAD_ARG);

	err = curl_easy_setopt(c_obj->curl, CURLOPT_URL, url);
	if(err == CURLE_OK)
		c_obj->url = (char*)url;
	return(err);
}
/* Gets the URL that the CurlObject is associated with. */
const char* CurlObject_get_url(CurlObject* c_obj)
{
	if(c_obj)
		return(c_obj->url);
	else
		return(NULL);
}

/* Sets the output type for the CurlObject.
 *
 * Returns:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_set_output_type(CurlObject* c_obj, CurlObjectOutput output)
{
	int err = 0;

	if(!c_obj)
		return(CURL_OBJECT_BAD_ARG);

	if(output != OUTPUT_DEBUG)
	{
		err = curl_easy_setopt(c_obj->curl, CURLOPT_DEBUGFUNCTION, 0L);
		if(err)return(err);
	}
	if(output != OUTPUT_VERBOSE)
	{
		err = curl_easy_setopt(c_obj->curl, CURLOPT_VERBOSE, 0L);
		if(err)return(err);
	}

	if(output == OUTPUT_DEBUG)
		err = curl_easy_setopt(c_obj->curl, CURLOPT_DEBUGFUNCTION, 1L);
	else if(output == OUTPUT_VERBOSE)
		err = curl_easy_setopt(c_obj->curl, CURLOPT_VERBOSE, 1L);
	return(err);
}

/* Returns the CurlObject's output type.
 * If 'c_obj' is null, OUTPUT_DEFAULT will be returned.
 */
CurlObjectOutput CurlObject_get_output_type(CurlObject* c_obj)
{
	if(!c_obj)return(OUTPUT_DEFAULT);

	return(c_obj->output_type);
}
/***************************************/

/*******CONSTRUCTION*******/
/* Creates a new CurlObject object and returns an error code
 * on error. If an error occurs, c_obj will be set to null.
 *
 * Return Codes:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_new(CurlObject** c_obj, const char* url)
{
	int err;

	if(!c_obj)
		return(CURL_OBJECT_BAD_ARG);

	//Create a new object
	*c_obj = NULL;
	*c_obj = (CurlObject*)malloc(sizeof(CurlObject));
	if(!*c_obj)
		return(CURL_OBJECT_MEM_ERR);

	//Setup curl
	(*c_obj)->curl = curl_easy_init();
	if(!(*c_obj)->curl)
	{
		free(*c_obj);
		*c_obj = NULL;
		return(CURL_OBJECT_MEM_ERR);
	}

	//Set curl options.
	if(url != NULL)
	{
		err = curl_easy_setopt((*c_obj)->curl, CURLOPT_URL, url);
		if(err != CURLE_OK)
		{
			curl_easy_cleanup((*c_obj)->curl);
			free(*c_obj);
			*c_obj = NULL;
			return(err);
		}
	}
	err = curl_easy_setopt((*c_obj)->curl, CURLOPT_WRITEFUNCTION, parse_data);
	if(err != CURLE_OK)
	{
		curl_easy_cleanup((*c_obj)->curl);
		free(*c_obj);
		*c_obj = NULL;
		return(err);
	}
	err = curl_easy_setopt((*c_obj)->curl, CURLOPT_WRITEDATA, *c_obj);
	if(err != CURLE_OK)
	{
		curl_easy_cleanup((*c_obj)->curl);
		free(*c_obj);
		*c_obj = NULL;
		return(err);
	}

	//Setup other members
	(*c_obj)->headers = NULL;
	(*c_obj)->buff = NULL;
	(*c_obj)->url = (char*)url;
	(*c_obj)->output_type = OUTPUT_DEFAULT;

	//Setup Function Pointers
	(*c_obj)->Get = &CurlObject_get;

	return(CURL_OBJECT_OK);
}

/* Frees memory allocated by the given CurlObject and
 * sets 'c_obj' to point to NULL. */
void CurlObject_del(CurlObject** c_obj)
{
	if(!c_obj || !*c_obj)return;

	if((*c_obj)->curl)
		curl_easy_cleanup((*c_obj)->curl);
	if((*c_obj)->headers)
		curl_slist_free_all((*c_obj)->headers);
	if((*c_obj)->buff)
		evbuffer_free((*c_obj)->buff);

	free(*c_obj);
	*c_obj = NULL;
}
/**************************/
