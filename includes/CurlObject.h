#ifndef CURL_OBJECT_IS_DEFINED
#define CURL_OBJECT_IS_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <event.h>
#include <curl/curl.h>

#include "alib_error.h"
#include "StringObject.h"

/*******ERROR CODES*******/
/* This is only added for backwards compatibility. */
typedef enum CurlObjectError
{
	CURL_OBJECT_MEM_ERR = ALIB_MEM_ERR,
	CURL_OBJECT_BAD_ARG = ALIB_BAD_ARG,
	CURL_OBJECT_OK = ALIB_OK
}CurlObjectError;
/*************************/

/*******TYPE CODES*******/
typedef enum CurlObjectOutput
{
	OUTPUT_DEFAULT = 0,
	OUTPUT_VERBOSE = 1,
	OUTPUT_DEBUG = 2
}CurlObjectOutput;
/************************/

typedef struct CurlObject
{
	CURL* curl;
	struct curl_slist* headers;
	struct evbuffer* buff;

	char* url;
	CurlObjectOutput output_type;

	/***Function Pointers***/
	/* Sends a get request to the CurlObject's URL and fills
	 * the CurlObject's buffer with the response.
	 *
	 * Return Codes:
	 * 		CurlObjectError
	 * 		CURLcode
	 */
	int (*Get)(struct CurlObject*);

}CurlObject;

/*******INITIALIZATION*******/
/* Initializes Curl.  Must be called before anything other CurlObject
 * functions.  CurlObject_uninit must be called when no more operations are needed.
 * Returns CURLE_OK when successful, anything else is an error code.*/
CURLcode CurlObject_init();

/* Uninitializes Curl.  Used when no more operations are needed with
 * the CurlObject.  CurlObject_init() must be called before CurlObject_uninit
 * can be called.*/
void CurlObject_uninit();
/****************************/

/*******PUBLIC MEMBER FUNCTIONS*******/
/* Sends a get request to the CurlObject's URL and fills
 * the CurlObject's buffer with the response.
 *
 * Return Codes:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_get(CurlObject* c_obj);

/* Sends a post request to the CurlObject's URL and fills
 * the CurlObject's buffer with the response.
 * Return Codes:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_post(CurlObject* c_obj, char** headers,
		unsigned int header_count, char* data, unsigned long data_len);

/* Returns a StringObject of the received data from the connection.
 *
 * If no data is immediately available, the call will fail.
 * Returned value MUST be freed by the caller.
 *
 * Returns:
 * 		NULL: Failure or error.
 * 		StringObject*: A newly allocated string object containing
 * 			the received data. */
StringObject* CurlObject_recv(CurlObject* c_obj);
/*************************************/

/*******PUBLIC ACCESSOR FUNCTIONS*******/
/* Set the url for the given CurlObject.
 *
 * Returns:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_set_url(CurlObject* c_obj, const char* url);
/* Gets the URL that the CurlObject is associated with. */
const char* CurlObject_get_url(CurlObject* c_obj);

/* Sets the output type for the CurlObject.
 *
 * Returns:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_set_output_type(CurlObject* c_obj, CurlObjectOutput output);

/* Returns the CurlObject's output type.
 * If 'c_obj' is null, OUTPUT_DEFAULT will be returned.
 */
CurlObjectOutput CurlObject_get_output_type(CurlObject* c_obj);
/***************************************/

/*******CONSTRUCTION*******/
/* Creates a new CurlObject object and returns an error code
 * on error. If an error occurs, c_obj will be set to null.
 *
 * Return Codes:
 * 		CurlObjectError
 * 		CURLcode
 */
int CurlObject_new(CurlObject** c_obj, const char* url);

/* Frees memory allocated by the given CurlObject and
 * sets 'c_obj' to point to NULL. */
void CurlObject_del(CurlObject** c_obj);
/**************************/

#endif
