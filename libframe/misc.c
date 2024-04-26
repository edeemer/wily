/* Copyright (c) 1992 AT&T - All rights reserved. */
#include	<u.h>
#include	<libc.h>
#include	<pwd.h>

#ifdef NO_MEMMOVE
/*
 * memcpy is probably fast, but may not work with overlap
 */
void*
memmove(void *a1, const void *a2, size_t n)
{
	char *s1;
	const char *s2;

	s1 = a1;
	s2 = a2;
	if(s1 > s2)
		goto back;
	if(s1 + n <= s2)
		return memcpy(a1, a2, n);
	while(n > 0) {
		*s1++ = *s2++;
		n--;
	}
	return a1;

back:
	s2 += n;
	if(s2 <= s1)
		return memcpy(a1, a2, n);
	s1 += n;
	while(n > 0) {
		*--s1 = *--s2;
		n--;
	}
	return a1;
}
#endif /* HAVE_MEMMOVE */
