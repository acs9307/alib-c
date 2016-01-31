#ifndef JSON_ARRAY_ITERATOR_IS_DEFINED
#define JSON_ARRAY_ITERATOR_IS_DEFINED

#include <stdio.h>
#include <json-c/json.h>\

//TODO: RENAME TO JsonArray

typedef enum JsonArrayIteratorError
{
	ARRAY_ITER_MEM_ERR = -2,
	ARRAY_ITER_BAD_ARG = -1,
	ARRAY_ITER_OK = 0
}JsonArrayIteratorError;

typedef struct JsonArrayIterator
{
	json_object* j_array;

	/* Should point to one after the last one called
	 * (this is so that the first call to "next" can be fulfilled). */
	int it;
	//Length of the array.
	unsigned int length;
}JsonArrayIterator;

/*******PUBLIC FUNCTIONS*******/
/* Gets the next json_object pointer in the array and returns it.
 * If there are no other json_objects after the current pointer,
 * the return value will be NULL.  Call JsonArrayIterator_set_begin()
 * to set the pointer to the beginning of the list.
 *
 * Returns:
 * 		json_object* on success.
 * 		NULL if 'it' is null or if the iterator is pointing
 * 			to the end of the list.
 */
json_object* JsonArrayIterator_next(JsonArrayIterator* it)
{
	if(!it || it->it == it->length)
		return(NULL);

	return(json_object_array_get_idx(it->j_array, it->it++));
}

/* Gets the previous json_object pointer in the array and returns it.
 * If there are no other json_objects before the current pointer,
 * the return value will be NULL.  Call JsonArrayIterator_set_end()
 * to set the pointer to the end of the list.
 *
 * Returns:
 * 		json_object* on success.
 * 		NULL if 'it' is null or if the iterator is pointing
 * 			to the beginning of the list.
 */
json_object* JsonArrayIterator_prev(JsonArrayIterator* it)
{
	if(!it || it->it == 0)
		return(NULL);

	return(json_object_array_get_idx(it->j_array, --(it->it)));
}

/* Sets the iterator object's pointer to the beginning of the array. */
void JsonArrayIterator_set_begin(JsonArrayIterator* it){if(it)it->it = 0;}
/* Sets the iterator object's pointer to one past the last item of the array. */
void JsonArrayIterator_set_end(JsonArrayIterator* it){if(it)it->it = it->length;}
/******************************/

/*******CONSTRUCTORS*******/
/* Creates a new JsonArrayIterator object with the given j_obj.
 * If the given json_object is not of type array, it will increment
 * through the object until either an array is found or all sub objects have
 * been looked at (if this happens, ARRAY_ITER_BAD_ARG will be returned).
 *
 * 'j_obj''s reference counter will be incremented, therefore the caller must call put on his end.
 *
 * On error, 'it' will be set to null.
 *
 * Returns:
 * 		JsonArrayIteratorError
 */
JsonArrayIteratorError JsonArrayIterator_new(JsonArrayIterator** it, json_object* j_obj)
{
	if(!it || !j_obj)
		return(ARRAY_ITER_BAD_ARG);
	*it = NULL;

	/* If the given object is not an array type, then
	 * recursively call JsonArrayIterator_new until
	 * all json_objects have been looked at or until
	 * the function completes successfully. */
	if(!json_object_is_type(j_obj, json_type_array))
	{
		if(json_object_is_type(j_obj, json_type_object))
		{
			JsonArrayIteratorError jaie;

			struct json_object_iterator iterator = json_object_iter_begin(j_obj);
			struct json_object_iterator end = json_object_iter_end(j_obj);
			for(; !json_object_iter_equal(&iterator, &end) ; json_object_iter_next(&iterator))
			{
				jaie = JsonArrayIterator_new(it, json_object_iter_peek_value(&iterator));
				if(jaie != ARRAY_ITER_BAD_ARG)
					break;
			}
			return(jaie);
		}
		else
			return(ARRAY_ITER_BAD_ARG);
	}

	*it = (JsonArrayIterator*)malloc(sizeof(JsonArrayIterator));
	if(!*it)
		return(ARRAY_ITER_MEM_ERR);

	//We want to increment the reference counter for the object.
	(*it)->j_array = json_object_get(j_obj);
	(*it)->it = 0;
	(*it)->length = json_object_array_length(j_obj);

	return(ARRAY_ITER_OK);
}

/* Deletes a JsonArrayIterator object and sets the pointer to null. */
void JsonArrayIterator_del(JsonArrayIterator** it)
{
	if(!it || !*it)return;

	if((*it)->j_array)
		json_object_put((*it)->j_array);
	free(*it);
	*it = NULL;
}
/**************************/

#endif
