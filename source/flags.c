#include "flags.h"

/* Raises a binary flag, if the 'flag' was already raised, no changes
 * will occur to 'flag_pole'.
 *
 * Parameters:
 * 		flag_pole: Pointer to the integer that stores all the related flags.
 * 			If NULL, then 0 will be returned.
 * 		flag: The flag to raise.  This value can be a combination of AND, ORed,
 * 			and XORed values.
 *
 * Returns:
 * 		The value of the flag pole after modification. */
flag_pole flag_raise(flag_pole* flag_pole, size_t flag)
{
	if(!flag_pole)return(0);

	*flag_pole |= flag;
	return(*flag_pole);
}
/* Lowers (or removes) a binary flag. If the 'flag' was not raised, no changes
 * will occur to 'flag_pole'.
 *
 * Parameters:
 * 		flag_pole: Pointer to the integer that stores all the related flags.
 * 			If NULL, then 0 will be returned.
 * 		flag: The flag to raise.  This value can be a combination of AND, ORed,
 * 			and XORed values.
 *
 * Returns:
 * 		The value of the flag pole after modification. */
flag_pole flag_lower(flag_pole* flag_pole, size_t flag)
{
	if(!flag_pole)return(0);

	*flag_pole &= ~flag;
	return(*flag_pole);
}

/* Removes any and all flags from the flag pole.
 *
 * Parameters:
 * 		flag_pole: (OPTIONAL) The flag pole to clear.
 *
 * Returns:
 * 		The value of the flag pole. */
flag_pole flag_clear(flag_pole* flag_pole)
{
	if(flag_pole)
		*flag_pole = 0;
	return(0);
}
