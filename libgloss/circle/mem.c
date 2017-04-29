/*
 * mem.c
 *
 * *_r wrappers for memory management functions, that simply call
 * the Circle memory management functions. Reentrancy is handled
 * in Circle.
 */


#include "config.h"
#include <malloc.h>

_PTR _malloc_r _PARAMS ((struct _reent *r, size_t size))
{
	(void) r;

	return malloc(size);
}

_VOID _free_r _PARAMS ((struct _reent *r, _PTR p))
{
	(void) r;

	free(p);
}

_PTR _realloc_r _PARAMS ((struct _reent *r, _PTR p, size_t size))
{
	(void) r;

	return realloc(p, size);
}

_PTR _calloc_r _PARAMS ((struct _reent *r, size_t nelem, size_t elsize))
{
	(void) r;

	return calloc(nelem, elsize);
}
